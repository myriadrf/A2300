//
// Copyright 2010-2012 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Note: Local modifications for A2300 by Loctronix Corporation.

#define SRPH_DONT_CHECK_SEQUENCE
#include "Uhd/transport/super_recv_packet_handler.hpp"
#define SSPH_DONT_PAD_TO_ONE
#include "Uhd/transport/super_send_packet_handler.hpp"
//#include "usrp1_calc_mux.hpp"
#include "a2300_impl.hpp"
#include <uhd/utils/msg.hpp>
#include <uhd/utils/tasks.hpp>
#include <uhd/utils/safe_call.hpp>
#include <uhd/transport/bounded_buffer.hpp>
#include <boost/math/special_functions/sign.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/make_shared.hpp>

#define bmFR_RX_FORMAT_SHIFT_SHIFT 0
#define bmFR_RX_FORMAT_WIDTH_SHIFT 4
#define bmFR_TX_FORMAT_16_IQ       0
#define bmFR_RX_FORMAT_WANT_Q      (0x1  <<  9)
#define FR_RX_FREQ_0               34
#define FR_RX_FREQ_1               35
#define FR_RX_FREQ_2               36
#define FR_RX_FREQ_3               37
#define FR_INTERP_RATE             32
#define FR_DECIM_RATE              33
#define FR_RX_MUX                  38
#define FR_TX_MUX                  39
#define FR_TX_FORMAT               48
#define FR_RX_FORMAT               49
#define FR_TX_SAMPLE_RATE_DIV      0
#define FR_RX_SAMPLE_RATE_DIV      1
#define GS_TX_UNDERRUN             0
#define GS_RX_OVERRUN              1
#define VRQ_GET_STATUS             0x80

using namespace uhd;
using namespace uhd::usrp;
using namespace uhd::transport;


static const size_t alignment_padding = 512;

/***********************************************************************
 * Helper struct to associate an offset with a buffer
 **********************************************************************/
struct offset_send_buffer{
    offset_send_buffer(void){
        /* NOP *///CJC
    }

    offset_send_buffer(managed_send_buffer::sptr buff, size_t offset = 0):
        buff(buff), offset(offset)
    {
        /* NOP */
    }

    //member variables
    managed_send_buffer::sptr buff;
    size_t offset; /* in bytes */
};

/***********************************************************************
 * Reusable managed send buffer to handle aligned commits
 **********************************************************************/
class offset_managed_send_buffer : public managed_send_buffer{
public:
    typedef boost::function<void(offset_send_buffer&, offset_send_buffer&, size_t)> commit_cb_type;

    offset_managed_send_buffer(const commit_cb_type &commit_cb):
        _commit_cb(commit_cb)
    {
        /* NOP */
    }

    void release(void){
        this->_commit_cb(_curr_buff, _next_buff, size());
    }

    sptr get_new(
        offset_send_buffer &curr_buff,
        offset_send_buffer &next_buff
    ){
        _curr_buff = curr_buff;
        _next_buff = next_buff;
        return make(this,
            _curr_buff.buff->cast<char *>() + _curr_buff.offset,
            _curr_buff.buff->size()         - _curr_buff.offset
        );
    }

private:
    offset_send_buffer _curr_buff, _next_buff;
    commit_cb_type _commit_cb;
};

/***********************************************************************
 * BS VRT packer/unpacker functions (since samples don't have headers)
 **********************************************************************/
static void usrp1_bs_vrt_packer(
    boost::uint32_t *,
    vrt::if_packet_info_t &if_packet_info
){
    if_packet_info.num_header_words32 = 0;
    if_packet_info.num_packet_words32 = if_packet_info.num_payload_words32;
}

static void usrp1_bs_vrt_unpacker(
    const boost::uint32_t *,
    vrt::if_packet_info_t &if_packet_info
){
    if_packet_info.packet_type = vrt::if_packet_info_t::PACKET_TYPE_DATA;
    if_packet_info.num_payload_words32 = if_packet_info.num_packet_words32;
    if_packet_info.num_payload_bytes = if_packet_info.num_packet_words32*sizeof(boost::uint32_t);
    if_packet_info.num_header_words32 = 0;
    if_packet_info.packet_count = 0;
    if_packet_info.sob = false;
    if_packet_info.eob = false;
    if_packet_info.has_sid = false;
    if_packet_info.has_cid = false;
    if_packet_info.has_tsi = false;
    if_packet_info.has_tsf = false;
    if_packet_info.has_tlr = false;
}

/***********************************************************************
 * IO Implementation Details
 **********************************************************************/
struct a2300_impl::io_impl
{
    io_impl(zero_copy_if::sptr data_transport):
        data_transport(data_transport),
        curr_buff(offset_send_buffer(data_transport->get_send_buff())),
        omsb(boost::bind(&a2300_impl::io_impl::commit_send_buff, this, _1, _2, _3))
    {
        /* NOP */
    }

    ~io_impl(void){
        UHD_SAFE_CALL(flush_send_buff();)
    }

    zero_copy_if::sptr data_transport;

    //wrapper around the actual send buffer interface
    //all of this to ensure only aligned lengths are committed
    //NOTE: you must commit before getting a new buffer
    //since the vrt packet handler obeys this, we are ok
    offset_send_buffer curr_buff;
    offset_managed_send_buffer omsb;
    void commit_send_buff(offset_send_buffer&, offset_send_buffer&, size_t);
    void flush_send_buff(void);
    managed_send_buffer::sptr get_send_buff(double timeout){
        //try to get a new managed buffer with timeout
        offset_send_buffer next_buff(data_transport->get_send_buff(timeout));
        if (not next_buff.buff.get()) return managed_send_buffer::sptr(); /* propagate timeout here */

        //make a new managed buffer with the offset buffs
        return omsb.get_new(curr_buff, next_buff);
    }

    task::sptr vandal_task;
    boost::system_time last_send_time;
};

/*!
 * Perform an actual commit on the send buffer:
 * Copy the remainder of alignment to the next buffer.
 * Commit the current buffer at multiples of alignment.
 */
void a2300_impl::io_impl::commit_send_buff(
    offset_send_buffer &curr,
    offset_send_buffer &next,
    size_t num_bytes
){
    //total number of bytes now in the current buffer
    size_t bytes_in_curr_buffer = curr.offset + num_bytes;

    //calculate how many to commit and remainder
    size_t num_bytes_remaining = bytes_in_curr_buffer % alignment_padding;
    size_t num_bytes_to_commit = bytes_in_curr_buffer - num_bytes_remaining;

    //copy the remainder into the next buffer
    std::memcpy(
        next.buff->cast<char *>() + next.offset,
        curr.buff->cast<char *>() + num_bytes_to_commit,
        num_bytes_remaining
    );

    //update the offset into the next buffer
    next.offset += num_bytes_remaining;

    //commit the current buffer
    curr.buff->commit(num_bytes_to_commit);

    //store the next buffer for the next call
    curr_buff = next;
}

/*!
 * Flush the current buffer by padding out to alignment and committing.
 */
void a2300_impl::io_impl::flush_send_buff(void){
    //calculate the number of bytes to alignment
    size_t bytes_to_pad = (-1*curr_buff.offset)%alignment_padding;

    //send at least alignment_padding to guarantee zeros are sent
    if (bytes_to_pad == 0) bytes_to_pad = alignment_padding;

    //get the buffer, clear, and commit (really current buffer)
    managed_send_buffer::sptr buff = this->get_send_buff(.1);
    if (buff.get() != NULL){
        std::memset(buff->cast<void *>(), 0, bytes_to_pad);
        buff->commit(bytes_to_pad);
    }
}

/***********************************************************************
 * Initialize internals within this file
 **********************************************************************/
void a2300_impl::io_init(void){

    _io_impl = UHD_PIMPL_MAKE(io_impl, (_data_transport));


    rx_stream_on_off(false);
    tx_stream_on_off(false);
    _io_impl->flush_send_buff();

    //create a new vandal thread to poll xerflow conditions
    _io_impl->vandal_task = task::make(boost::bind(
        &a2300_impl::vandal_conquest_loop, this
    ));
}

void a2300_impl::rx_stream_on_off(bool enb){
    //drain any junk in the receive transport after stop streaming command
    while(not enb and _data_transport->get_recv_buff().get() != NULL){
        /* NOP */
    }
}

void a2300_impl::tx_stream_on_off(bool enb){
    _io_impl->last_send_time = boost::get_system_time();
}

/*!
 * Casually poll the overflow and underflow registers.
 * On an underflow, push an async message into the queue and print.
 * On an overflow, interleave an inline message into recv and print.
 * This procedure creates "soft" inline and async user messages.
 */
void a2300_impl::vandal_conquest_loop(void){

    //initialize the async metadata
    async_metadata_t async_metadata;
    async_metadata.channel = 0;
    async_metadata.has_time_spec = true;
    async_metadata.event_code = async_metadata_t::EVENT_CODE_UNDERFLOW;

    //initialize the inline metadata
    rx_metadata_t inline_metadata;
    inline_metadata.has_time_spec = true;
    inline_metadata.error_code = rx_metadata_t::ERROR_CODE_OVERFLOW;

    //start the polling loop...
    try{ while (not boost::this_thread::interruption_requested()){
        boost::uint8_t underflow = 0, overflow = 0;
//
//        //shutoff transmit if it has been too long since send() was called
//        if (_tx_enabled and (boost::get_system_time() - _io_impl->last_send_time) > boost::posix_time::milliseconds(100)){
//            this->tx_stream_on_off(false);
//        }
//
//        //always poll regardless of enabled so we can clear the conditions
//        _fx2_ctrl->usrp_control_read(
//            VRQ_GET_STATUS, 0, GS_TX_UNDERRUN, &underflow, sizeof(underflow)
//        );
//        _fx2_ctrl->usrp_control_read(
//            VRQ_GET_STATUS, 0, GS_RX_OVERRUN, &overflow, sizeof(overflow)
//        );
//
//        //handle message generation for xerflow conditions
//        if (_tx_enabled and underflow){
//            async_metadata.time_spec = _soft_time_ctrl->get_time();
//            _soft_time_ctrl->get_async_queue().push_with_pop_on_full(async_metadata);
//            UHD_MSG(fastpath) << "U";
//        }
//        if (_rx_enabled and overflow){
//            inline_metadata.time_spec = _soft_time_ctrl->get_time();
//            _soft_time_ctrl->get_inline_queue().push_with_pop_on_full(inline_metadata);
//            UHD_MSG(fastpath) << "O";
//        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(50));
    }}
    catch(const boost::thread_interrupted &){} //normal exit condition
    catch(const std::exception &e){
        UHD_MSG(error) << "The vandal caught an unexpected exception " << e.what() << std::endl;
    }
}

/***********************************************************************
 * RX streamer wrapper that talks to soft time control
 **********************************************************************/
class a2300_recv_packet_streamer : public sph::recv_packet_handler, public rx_streamer{
public:
	a2300_recv_packet_streamer(const size_t max_num_samps, soft_time_ctrl::sptr stc){
        _max_num_samps = max_num_samps;
        _stc = stc;
    }

    size_t get_num_channels(void) const{
        return this->size();
    }

    size_t get_max_num_samps(void) const{
        return _max_num_samps;
    }

    size_t recv(
        const rx_streamer::buffs_type &buffs,
        const size_t nsamps_per_buff,
        uhd::rx_metadata_t &metadata,
        const double timeout,
        const bool one_packet
    ){
        //interleave a "soft" inline message into the receive stream:
        if (_stc->get_inline_queue().pop_with_haste(metadata)) return 0;

        size_t num_samps_recvd = sph::recv_packet_handler::recv(
            buffs, nsamps_per_buff, metadata, timeout, one_packet
        );

        return _stc->recv_post(metadata, num_samps_recvd);
    }

    void issue_stream_cmd(const stream_cmd_t &stream_cmd)
    {
        _stc->issue_stream_cmd(stream_cmd);
    }

private:
    size_t _max_num_samps;
    soft_time_ctrl::sptr _stc;
};

/***********************************************************************
 * TX streamer wrapper that talks to soft time control
 **********************************************************************/
class a2300_send_packet_streamer : public sph::send_packet_handler, public tx_streamer{
public:
    a2300_send_packet_streamer(const size_t max_num_samps, soft_time_ctrl::sptr stc, boost::function<void(bool)> tx_enb_fcn){
        _max_num_samps = max_num_samps;
        this->set_max_samples_per_packet(_max_num_samps);
        _stc = stc;
        _tx_enb_fcn = tx_enb_fcn;
    }

    size_t get_num_channels(void) const{
        return this->size();
    }

    size_t get_max_num_samps(void) const{
        return _max_num_samps;
    }

    size_t send(
        const tx_streamer::buffs_type &buffs,
        const size_t nsamps_per_buff,
        const uhd::tx_metadata_t &metadata,
        const double timeout_
    ){
        double timeout = timeout_; //rw copy
        _stc->send_pre(metadata, timeout);

        _tx_enb_fcn(true); //always enable (it will do the right thing)
        size_t num_samps_sent = sph::send_packet_handler::send(
            buffs, nsamps_per_buff, metadata, timeout
        );

        //handle eob flag (commit the buffer, //disable the DACs)
        //check num samps sent to avoid flush on incomplete/timeout
        if (metadata.end_of_burst and num_samps_sent == nsamps_per_buff){
            async_metadata_t metadata;
            metadata.channel = 0;
            metadata.has_time_spec = true;
            metadata.time_spec = _stc->get_time();
            metadata.event_code = async_metadata_t::EVENT_CODE_BURST_ACK;
            _stc->get_async_queue().push_with_pop_on_full(metadata);
            _tx_enb_fcn(false);
        }

        return num_samps_sent;
    }

    bool recv_async_msg(
        async_metadata_t &async_metadata, double timeout = 0.1
    ){
        return _stc->get_async_queue().pop_with_timed_wait(async_metadata, timeout);
    }

private:
    size_t _max_num_samps;
    soft_time_ctrl::sptr _stc;
    boost::function<void(bool)> _tx_enb_fcn;
};



/***********************************************************************
 * Async Data
 **********************************************************************/
bool a2300_impl::recv_async_msg(
    async_metadata_t &async_metadata, double timeout
){
    boost::this_thread::disable_interruption di; //disable because the wait can throw
    return _soft_time_ctrl->get_async_queue().pop_with_timed_wait(async_metadata, timeout);
}

/***********************************************************************
 * Receive streamer
 **********************************************************************/
rx_streamer::sptr a2300_impl::get_rx_stream(const uhd::stream_args_t &args_)
{
    stream_args_t args = args_;

    //setup defaults for unspecified values
    if (args.otw_format.empty()) args.otw_format = "sc16";
    args.channels = args.channels.empty()? std::vector<size_t>(1, 0) : args.channels;

    boost::shared_ptr<a2300_recv_packet_streamer> my_streamer;
    for (size_t stream_i = 0; stream_i < args.channels.size(); stream_i++)
    {
        const size_t chan = args.channels[stream_i];
        RadioPeripheral &perif = m_perifs[chan];

        if (args.otw_format != "sc16")
        {
            throw uhd::value_error("A2300 RX cannot handle requested wire format: " + args.otw_format);
        }

        //calculate packet size
//        static const size_t hdr_size = 0
//            + vrt::max_if_hdr_words32*sizeof(boost::uint32_t)
//            + sizeof(vrt::if_packet_info_t().tlr) //forced to have trailer
//            - sizeof(vrt::if_packet_info_t().cid) //no class id ever used
//            - sizeof(vrt::if_packet_info_t().tsi) //no int time ever used
//        ;

        //calculate packet size
        const size_t bpp = _data_transport->get_recv_frame_size(); //-hdr_size;
        const size_t bpi = convert::get_bytes_per_item(args.otw_format);
        size_t spp = unsigned(args.args.cast<double>("spp", bpp/bpi));
        spp = std::min<size_t>(2000, spp); //magic maximum for framing at full rate


        //make the new streamer given the samples per packet
        if (not my_streamer)
        {
        	my_streamer = boost::make_shared<a2300_recv_packet_streamer>(spp, _soft_time_ctrl);

        	//init some streamer stuff
            my_streamer->resize(args.channels.size());
            my_streamer->set_tick_rate(_master_clock_rate);
            my_streamer->set_vrt_unpacker(&usrp1_bs_vrt_unpacker);
            my_streamer->set_xport_chan_get_buff(0, boost::bind(
                &uhd::transport::zero_copy_if::get_recv_buff, _io_impl->data_transport, _1
            ));
        }

        //set the converter
        uhd::convert::id_type id;
        id.input_format = args.otw_format + "_item16_usrp1";
        id.num_inputs = 1;
        id.output_format = args.cpu_format;
        id.num_outputs = args.channels.size();
        my_streamer->set_converter(id);


//        perif.framer->clear();
//        perif.framer->set_nsamps_per_packet(spp);
//        perif.framer->set_sid(sid);
//        perif.framer->setup(args);
        perif.Ddc().ConfigStreaming(args);
        //_demux->realloc_sid(sid);

//        my_streamer->set_xport_chan_get_buff(stream_i, boost::bind(
//           &recv_packet_demuxer_3000::get_recv_buff, _demux, sid, _1
//        ), true /*flush*/);
        my_streamer->set_overflow_handler(stream_i, boost::bind(
            &a2300_impl::handle_overflow, this, chan
        ));
        my_streamer->set_issue_stream_cmd(stream_i, boost::bind(
            &DspConfig::IssueStreamCommand, &(perif.Ddc()), _1
        ));
        perif.RxStreamer(my_streamer); //store weak pointer

        //sets all tick and samp rates on this streamer
        this->update_tick_rate(this->get_tick_rate());
        m_tree->access<double>(str(boost::format("/mboards/0/rx_dsps/%u/rate/value") % chan)).update();
    }
    this->update_enables();

    return my_streamer;
}

void a2300_impl::handle_overflow(const size_t i)
{
    boost::shared_ptr<sph::recv_packet_streamer> my_streamer =
            boost::dynamic_pointer_cast<sph::recv_packet_streamer>(m_perifs[i].RxStreamer().lock());

    if (my_streamer->get_num_channels() == 2) //MIMO time
    {
        //find out if we were in continuous mode before stopping
        const bool in_continuous_streaming_mode = true;//_m_perifs[i].framer->in_continuous_streaming_mode();
        //stop streaming
        my_streamer->issue_stream_cmd(stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);

        //flush demux
        //_demux->realloc_sid(B200_RX_DATA0_SID);
        //_demux->realloc_sid(B200_RX_DATA1_SID);
        //flush actual transport
        while (_data_transport->get_recv_buff(0.001)){}

        //restart streaming
        if (in_continuous_streaming_mode)
        {
            stream_cmd_t stream_cmd(stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
            stream_cmd.stream_now = false;
//TODO            stream_cmd.time_spec = m_perifs[i].time64->get_time_now() + time_spec_t(0.01);
            my_streamer->issue_stream_cmd(stream_cmd);
        }
    }
    //else
    //	m_perifs[i].framer->handle_overflow();
}



/***********************************************************************
 * Transmit streamer
 **********************************************************************/
tx_streamer::sptr a2300_impl::get_tx_stream(const uhd::stream_args_t &args_){
    stream_args_t args = args_;

    //setup defaults for unspecified values
     if (args.otw_format.empty()) args.otw_format = "sc16";
     args.channels = args.channels.empty()? std::vector<size_t>(1, 0) : args.channels;

     boost::shared_ptr<a2300_send_packet_streamer> my_streamer;
     for (size_t stream_i = 0; stream_i < args.channels.size(); stream_i++)
     {
         const size_t chan = args.channels[stream_i];
         RadioPeripheral &perif = m_perifs[chan];
         if (args.otw_format != "sc16"){
             throw uhd::value_error("A2300 TX cannot handle requested wire format: " + args.otw_format);
         }

         //calculate packet size
         static const size_t hdr_size = 0;
//             + vrt::max_if_hdr_words32*sizeof(boost::uint32_t)
//             //+ sizeof(vrt::if_packet_info_t().tlr) //forced to have trailer
//             - sizeof(vrt::if_packet_info_t().cid) //no class id ever used
//             - sizeof(vrt::if_packet_info_t().tsi) //no int time ever used
         ;

         static const size_t bpp = _data_transport->get_send_frame_size() - hdr_size;
         const size_t spp = bpp/convert::get_bytes_per_item(args.otw_format);

         //make the new streamer given the samples per packet
         if (not my_streamer)
		 {
             boost::function<void(bool)> tx_fcn = boost::bind(&a2300_impl::tx_stream_on_off, this, _1);
             my_streamer = boost::make_shared<a2300_send_packet_streamer>(spp, _soft_time_ctrl, tx_fcn);

             my_streamer->resize(args.channels.size());

             //init some streamer stuff
             my_streamer->set_tick_rate(_master_clock_rate);
             my_streamer->set_vrt_packer(&usrp1_bs_vrt_packer);
             my_streamer->set_xport_chan_get_buff(stream_i, boost::bind(
                 &zero_copy_if::get_send_buff, _data_transport, _1 ));

             my_streamer->set_enable_trailer(false); //TODO not implemented trailer support yet

		 }

         //set the converter
         uhd::convert::id_type id;
         id.input_format = args.cpu_format;
         id.num_inputs = args.channels.size();
         id.output_format = args.otw_format + "_item16_usrp1";
         id.num_outputs = 1;
         my_streamer->set_converter(id);

         //TODO setup framed data.
         //perif.deframer->clear();
         //perif.deframer->setup(args);
         //perif.duc->setup(args);

         //TODO
         //my_streamer->set_async_receiver(boost::bind(
         //    &async_md_type::pop_with_timed_wait, _async_task_data->async_md, _1, _2
         //));
         //my_streamer->set_xport_chan_sid(stream_i, true, chan?B200_TX_DATA1_SID:B200_TX_DATA0_SID);
         perif.TxStreamer(my_streamer); //store weak pointer

         //sets all tick and samp rates on this streamer
         this->update_tick_rate(this->get_tick_rate());
         m_tree->access<double>(str(boost::format("/mboards/0/tx_dsps/%u/rate/value") % chan)).update();
     }
     this->update_enables();

     return my_streamer;
}

/***********************************************************************
 * Tick rate comprehension below
 **********************************************************************/
double a2300_impl::set_tick_rate(const double rate)
{
//    UHD_MSG(status) << "Asking for clock rate " << rate/1e6 << " MHz\n";
//    _tick_rate = _codec_ctrl->set_clock_rate(rate);
//    UHD_MSG(status) << "Actually got clock rate " << _tick_rate/1e6 << " MHz\n";
//
//    //reset after clock rate change
//    this->reset_codec_dcm();


	//BOOST_FOREACH(RadioPeripheral &perif, m_perifs)
    //{
    	//TODO Update
        //perif.time64->set_tick_rate(_tick_rate);
        //perif.time64->self_test();
    //}
    return rate;
}

double a2300_impl::get_tick_rate()
{
	return A2300_MAX_SAMPLING_FREQ;
}

void a2300_impl::update_tick_rate(const double rate)
{
    BOOST_FOREACH(RadioPeripheral &perif, m_perifs)
    {
        boost::shared_ptr<a2300_recv_packet_streamer> my_streamer =
            boost::dynamic_pointer_cast<a2300_recv_packet_streamer>(perif.RxStreamer().lock());
        if (my_streamer) my_streamer->set_tick_rate(rate);

        perif.SetTickRate( rate);
    }
    BOOST_FOREACH(RadioPeripheral &perif, m_perifs)
    {
        boost::shared_ptr<a2300_recv_packet_streamer> my_streamer =
            boost::dynamic_pointer_cast<a2300_recv_packet_streamer>(perif.TxStreamer().lock());
        if (my_streamer) my_streamer->set_tick_rate(rate);

        perif.SetTickRate( rate);
    }
}
