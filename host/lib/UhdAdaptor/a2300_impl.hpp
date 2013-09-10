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

#include "a2300_iface.hpp"
#include "Uhd/usrp/usrp1/soft_time_ctrl.hpp"

#include <uhd/device.hpp>
#include <uhd/property_tree.hpp>
#include <uhd/utils/pimpl.hpp>
#include <uhd/types/dict.hpp>
#include <uhd/types/otw_type.hpp>
#include <uhd/types/clock_config.hpp>
#include <uhd/types/stream_cmd.hpp>
#include <uhd/transport/usb_zero_copy.hpp>

#include <boost/foreach.hpp>
#include <boost/weak_ptr.hpp>
#include <complex>

#include "Dci/DciMsg.h"
#include "Dci/DciConversation.h"
#include "Dci/InfrastructureMsgs.h"
#include "Dci/WcaMsgs.h"
#include "Dci/StandardMsgs.h"

/*!
 * USRP1 implementation guts:
 * The implementation details are encapsulated here.
 * Handles properties on the mboard, dboard, dsps...
 */
class a2300_impl : public uhd::device {
public:

    //constructors
	a2300_impl(const uhd::device_addr_t &);
    virtual ~a2300_impl(void);

    //the io interface
    uhd::rx_streamer::sptr get_rx_stream(const uhd::stream_args_t &args);
    uhd::tx_streamer::sptr get_tx_stream(const uhd::stream_args_t &args);
    bool recv_async_msg(uhd::async_metadata_t &, double);

private:
    const static int BW_TSIZE = 16;
    const static double m_bandwidthTableMHz[];

	uhd::property_tree::sptr m_tree;

    //device properties interface
    uhd::property_tree::sptr get_tree(void) const{
        return m_tree;
    }

    void InitializePropertyTree();
    void InitializePropertyTreeExt(const uhd::fs_path path, int idComponent, int rxTxFlag);

    void * FindDevice(uint16 vid, uint16 pid, int usbAddress);
    bool FindEndPoint(void *pDevice, int epId);

    A2300_iface::sptr _dci0_ctrl;
    uhd::usrp::soft_time_ctrl::sptr _soft_time_ctrl;
    uhd::transport::usb_zero_copy::sptr _data_transport;
    uhd::transport::usb_zero_copy::sptr _idc0_transport;
    uhd::transport::usb_zero_copy::sptr _idc1_transport;

    double _master_clock_rate; //clock rate shadow

    //weak pointers to streamers for update purposes
    boost::weak_ptr<uhd::rx_streamer> _rx_streamer;
    boost::weak_ptr<uhd::tx_streamer> _tx_streamer;

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

    void   set_bandwidth(int idComponent, int idRxTx, int value);
    int    get_bandwidth(int idComponent, int idRxTx);

    void   set_frequency(int idComponent, int idRxTx, int value);
    int    get_frequency(int idComponent, int idRxTx);

    void   set_gain(int idComponent, int idRxTx, int value);
    int    get_gain(int idComponent, int idRxTx);

    // Generic message processor.
    bool   ProcessDciMessage(byte *buff, int msgLen);

    // Helper Functions.
    int GetBandwidthIndex( double bwMHz);
};

#endif /* INCLUDED_A2300_IMPL_HPP */
