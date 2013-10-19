#include "a2300_impl.hpp"
#include <A2300/A2300_Defs.h>

#include <uhd/utils/log.hpp>
#include <uhd/utils/safe_call.hpp>
#include <uhd/transport/usb_control.hpp>
#include <uhd/utils/msg.hpp>
#include <uhd/exception.hpp>
#include <uhd/utils/static.hpp>
#include <uhd/utils/images.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/types/stream_cmd.hpp>

#include <libusb-1.0/libusb.h>

#include "Dci/DciMsg.h"
#include "Dci/DciConversation.h"
#include "Dci/InfrastructureMsgs.h"
#include "Dci/WcaMsgs.h"
#include "Dci/StandardMsgs.h"

#include <boost/format.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/math/special_functions/round.hpp>
#include <cstdio>

using namespace uhd;

/*******************************************************************
* class ad9361_ctrl
******************************************************************/
//! get the gain range for a particular gain element
uhd::meta_range_t a2300_impl::get_gain_range(void)
{
    return uhd::meta_range_t(0.0, 30.0, 3.0);  // DB.
}

//! Get a list of gain names for RX or TX
 std::vector<std::string> a2300_impl::get_gain_names(const std::string &/*which*/)
 {
     return std::vector<std::string>(1, "PGA");
 }

 //! get the gain range for a particular gain element
 uhd::meta_range_t a2300_impl::get_gain_range(const std::string &which)
 {
	 // Db.
     return uhd::meta_range_t(0.0, 30.0, 3.0);
 }

 //! get the freq range
 uhd::meta_range_t a2300_impl::get_freq_range(void)
 {
     return uhd::meta_range_t(A2300_MIN_FREQ, A2300_MAX_FREQ);
 }

 //! get the clock rate range for the frontend
 uhd::meta_range_t a2300_impl::get_clock_rate_range(void)
 {
	 // (32 / 4) to (32,4096) in powers of 2,
	 return uhd::meta_range_t(A2300_MIN_CLOCK, A2300_MAX_CLOCK);
 }

 //! set the filter bandwidth for the frontend
 double a2300_impl::set_bw_filter(const std::string &/*which*/, const double /*bw*/)
 {
     return 56e6; //TODO
 }

/*******************************************************************
* Update methods for time
******************************************************************/
void a2300_impl::set_time(const uhd::time_spec_t &time)
{
	printf("set_time\n");
	//boost::mutex::scoped_lock lock(_mutex);
	//_time = time;
	//_use_time = _time != uhd::time_spec_t(0.0);
	//if (_use_time) _timeout = MASSIVE_TIMEOUT; //permanently sets larger timeout
}

/*******************************************************************
* rx_vita_core_3000.cpp
******************************************************************/
void a2300_impl::issue_stream_command(int idComponent, int idRxTx, const uhd::stream_cmd_t &stream_cmd)
{
	if (not _is_setup)
	{
		UHD_MSG(warning) << "rx vita core 3000 issue stream command - not setup yet!";
		return;
	}
	UHD_ASSERT_THROW(stream_cmd.num_samps <= 0x0fffffff);
	_continuous_streaming = (stream_cmd.stream_mode == stream_cmd_t::STREAM_MODE_START_CONTINUOUS);

	//setup the mode to instruction flags
	typedef boost::tuple<bool, bool, bool, bool> inst_t;
	static const uhd::dict<stream_cmd_t::stream_mode_t, inst_t> mode_to_inst = boost::assign::map_list_of
															//reload, chain, samps, stop
		(stream_cmd_t::STREAM_MODE_START_CONTINUOUS,   inst_t(true,  true,  false, false))
		(stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS,    inst_t(false, false, false, true))
		(stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE, inst_t(false, false, true,  false))
		(stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE, inst_t(false, true,  true,  false))
	;

	//setup the instruction flag values
	bool inst_reload, inst_chain, inst_samps, inst_stop;
	boost::tie(inst_reload, inst_chain, inst_samps, inst_stop) = mode_to_inst[stream_cmd.stream_mode];

	//calculate the word from flags and length
	boost::uint32_t cmd_word = 0;
	cmd_word |= boost::uint32_t((stream_cmd.stream_now)? 1 : 0) << 31;
	cmd_word |= boost::uint32_t((inst_chain)?            1 : 0) << 30;
	cmd_word |= boost::uint32_t((inst_reload)?           1 : 0) << 29;
	cmd_word |= boost::uint32_t((inst_stop)?             1 : 0) << 28;
	cmd_word |= (inst_samps)? stream_cmd.num_samps : ((inst_stop)? 0 : 1);

	//issue the stream command
//	_iface->poke32(REG_CTRL_CMD, cmd_word);
	const boost::uint64_t ticks = (stream_cmd.stream_now)? 0 : stream_cmd.time_spec.to_ticks(_tick_rate);
	printf("Ticks = %lu\n", (long)ticks);
//	_iface->poke32(REG_CTRL_TIME_HI, boost::uint32_t(ticks >> 32));
//	_iface->poke32(REG_CTRL_TIME_LO, boost::uint32_t(ticks >> 0)); //latches the command
}


