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

#include "RadioPeripheral.h"

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

    double get_tick_rate();
    double set_tick_rate(const double rate);
    void   update_tick_rate(const double rate);

private:
    bool _is_setup;
    uhd::property_tree::sptr m_tree;
	
    //controllers
    A2300_iface::sptr _dci0_ctrl;
    uhd::usrp::soft_time_ctrl::sptr _soft_time_ctrl;

    //transports
    uhd::transport::usb_zero_copy::sptr _data_transport;
    uhd::transport::usb_zero_copy::sptr _idc0_transport;
    uhd::transport::usb_zero_copy::sptr _idc1_transport;



    void * FindDevice(uint16 vid, uint16 pid, int usbAddress);
    bool FindEndPoint(void *pDevice, int epId);

    double _master_clock_rate; //clock rate shadow

    const uhd::usrp::mboard_eeprom_t & get_mb_eeprom(void);
    void set_mb_eeprom(const uhd::usrp::mboard_eeprom_t &);
    void update_rx_subdev_spec(const uhd::usrp::subdev_spec_t &);
    void update_tx_subdev_spec(const uhd::usrp::subdev_spec_t &);
    void update_time_source(const std::string &);
    void update_clock_source(const std::string &source);

//    void update_antenna_sel(const size_t which, const std::string &ant);
    uhd::sensor_value_t get_ref_locked(void);


    //Create RF Peripherials
    std::vector<RadioPeripheral> m_perifs;

    void handle_overflow(const size_t i);

    //handle io stuff
    UHD_PIMPL_DECL(io_impl) _io_impl;
    void io_init(void);
    void rx_stream_on_off(bool);
    void tx_stream_on_off(bool);
    void handle_overrun(size_t);
    void vandal_conquest_loop(void);

    //Property control
    void   send_idle_msg();

    std::string get_fw_version();
    std::string get_fpga_version();

    void update_enables(void);

    void set_time(const uhd::time_spec_t &time);

private:

     //uhd::meta_range_t get_clock_rate_range(void);

};

#endif /* INCLUDED_A2300_IMPL_HPP */
