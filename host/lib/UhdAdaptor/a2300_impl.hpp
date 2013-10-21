// Name:  a2300_impl.hpp
//
// Copyright 2013 Loctronix Corporation
// http://www.loctronix.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_A2300_IMPL_HPP
#define INCLUDED_A2300_IMPL_HPP

#include <uhd/device.hpp>
#include <uhd/property_tree.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/utils/pimpl.hpp>
#include <uhd/utils/tasks.hpp>
#include <uhd/types/dict.hpp>
#include <uhd/types/otw_type.hpp>
#include <uhd/types/sensors.hpp>
#include <uhd/types/clock_config.hpp>
#include <uhd/types/stream_cmd.hpp>
#include <uhd/usrp/mboard_eeprom.hpp>
#include <uhd/usrp/subdev_spec.hpp>
#include <uhd/transport/usb_zero_copy.hpp>
#include <uhd/transport/bounded_buffer.hpp>
#include "Uhd/usrp/usrp1/soft_time_ctrl.hpp"
#include "Uhd/usrp/usrp1/dsp_core_a2300.hpp"

#include <boost/foreach.hpp>
#include <boost/weak_ptr.hpp>
#include <complex>

#include "a2300_iface.hpp"
#include <A2300/A2300InterfaceDefs.h>
#include <A2300/A2300_Defs.h>

#include "Dci/DciMsg.h"
#include "Dci/DciConversation.h"
#include "Dci/InfrastructureMsgs.h"
#include "Dci/WcaMsgs.h"
#include "Dci/StandardMsgs.h"

/***********************************************************************
 * The A2300 Capability Constants
 * USRP1 implementation guts:
 * The implementation details are encapsulated here.
 * Handles properties on the mboard, dboard, dsps...
 */
class a2300_impl : public uhd::device {
public:

    //constructors
	a2300_impl(const uhd::device_addr_t &);
    virtual ~a2300_impl(void);

    virtual uhd::property_tree::sptr  get_tree(void) const;

    //the io interface
    virtual  uhd::rx_streamer::sptr get_rx_stream(const uhd::stream_args_t &args);
    virtual uhd::tx_streamer::sptr get_tx_stream(const uhd::stream_args_t &args);
    virtual bool recv_async_msg(uhd::async_metadata_t &, double);

private:
    const static int BW_TSIZE = 16;
    const static double m_bandwidthTableMHz[];
	uhd::property_tree::sptr m_tree;
	
    //controllers
    A2300_iface::sptr _dci0_ctrl;
    uhd::usrp::soft_time_ctrl::sptr _soft_time_ctrl;
	bool _is_setup;
	bool _continuous_streaming;

    //transports
    uhd::transport::usb_zero_copy::sptr _data_transport;
    uhd::transport::usb_zero_copy::sptr _idc0_transport;
    uhd::transport::usb_zero_copy::sptr _idc1_transport;



    void InitializePropertyTree(uhd::property_tree::sptr _tree);
    void InitializePropertyTreeExt(uhd::property_tree::sptr _tree, const uhd::fs_path path, int idComponent, int rxTxFlag);
    void setup_radio(uhd::property_tree::sptr _tree, const size_t dspno);

    void * FindDevice(uint16 vid, uint16 pid, int usbAddress);
    bool FindEndPoint(void *pDevice, int epId);

    double _master_clock_rate; //clock rate shadow

    //weak pointers to streamers for update purposes
    boost::weak_ptr<uhd::rx_streamer> _rx_streamer;
    boost::weak_ptr<uhd::tx_streamer> _tx_streamer;

    const uhd::usrp::mboard_eeprom_t & get_mb_eeprom(void);
    void set_mb_eeprom(const uhd::usrp::mboard_eeprom_t &);
    void update_rx_subdev_spec(const uhd::usrp::subdev_spec_t &);
    void update_tx_subdev_spec(const uhd::usrp::subdev_spec_t &);
    void update_time_source(const std::string &);
    void update_clock_source(const std::string &source);
    void update_antenna_sel(const size_t which, const std::string &ant);
    uhd::sensor_value_t get_ref_locked(void);

    //perifs in the radio core
    struct radio_perifs_t
    {
    	/*
        radio_ctrl_core_3000::sptr ctrl;
        time_core_3000::sptr time64;
        rx_vita_core_3000::sptr framer;
        rx_dsp_core_3000::sptr ddc;
        tx_vita_core_3000::sptr deframer;
        tx_dsp_core_3000::sptr duc;
        */
    	// Use alternate states for now.
    	int ctrl;   // Component ID.
    	int framer; // Component ID, A2300 does not have a Framer.
    	dsp_core_a2300::sptr ddc;    // Data Download.
    	int deframer;
    	dsp_core_a2300::sptr duc;    // Data Upload.
        boost::weak_ptr<uhd::rx_streamer> rx_streamer;
        boost::weak_ptr<uhd::tx_streamer> tx_streamer;
        bool ant_rx2;
    };
    std::vector<radio_perifs_t> _radio_perifs;
    void setup_radio(const size_t which_radio);
    void update_enables(void);

    //handle io stuff
    UHD_PIMPL_DECL(io_impl) _io_impl;
    void io_init(void);
    void rx_stream_on_off(bool);
    void tx_stream_on_off(bool);
    void handle_overrun(size_t);

    void vandal_conquest_loop(void);

    //Property control
    void   send_idle_msg();
    std::string get_fpga_version();

    void   set_bandwidth(int idComponent, int idRxTx, double value);
    double get_bandwidth(int idComponent, int idRxTx);

    double set_freq(int idComponent, int idRxTx, double value);
    double get_freq(int idComponent, int idRxTx);

    void   set_gain(int idComponent, int idRxTx, int value);
    int    get_gain(int idComponent, int idRxTx);

    uhd::meta_range_t get_freq_range(void);
    uhd::meta_range_t get_gain_range(void);

    // Generic message processor.
    bool   ProcessDciMessage(byte *buff, int msgLen);

    // Helper Functions.
    int GetBandwidthIndex( double bwMHz);

    void update_tick_rate(const double);
    void update_rx_samp_rate(const size_t, const double);
    void update_tx_samp_rate(const size_t, const double);

    double _tick_rate;
    double get_tick_rate(void){return _tick_rate;}
    double set_tick_rate(const double rate);

    void set_time(const uhd::time_spec_t &time);

private:
    /***********************************************************************
     * ad9361 stolen from.
     **********************************************************************/
    //! get the freq range for the frontend which
    static uhd::meta_range_t get_rf_freq_range(void)
    {
    	return uhd::meta_range_t(A2300_MIN_FREQ, A2300_MAX_FREQ);
    }

    //! get the filter range for the frontend which
    static uhd::meta_range_t get_bw_filter_range(void) // const std::string &/*which*/)
    {
    	return uhd::meta_range_t(A2300_MIN_BW, A2300_MAX_BW);
    }

    //! Get a list of gain names for RX or TX
     std::vector<std::string> get_gain_names(const std::string &/*which*/);
     uhd::meta_range_t get_freq_range(const std::string &which);
     uhd::meta_range_t get_gain_range(const std::string &which);
     uhd::meta_range_t get_bw_filter_range(const std::string &/*which*/);
     uhd::meta_range_t get_clock_rate_range(void);
     double set_bw_filter(const std::string &/*which*/, const double /*bw*/);

    // rx_vita_core_3000
    void issue_stream_command(int idComponent, int idRxTx, const uhd::stream_cmd_t &stream_cmd);
};

#endif /* INCLUDED_A2300_IMPL_HPP */
