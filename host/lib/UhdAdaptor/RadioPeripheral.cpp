// Name:  RadioPeripheral.cpp
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

#include "RadioPeripheral.h"

#include <A2300/A2300_Defs.h>
#include <A2300/A2300InterfaceDefs.h>
#define SRPH_DONT_CHECK_SEQUENCE
#include "Uhd/transport/super_recv_packet_handler.hpp"
#define SSPH_DONT_PAD_TO_ONE
#include "Uhd/transport/super_send_packet_handler.hpp"
//#include "usrp1_calc_mux.hpp"
#include <uhd/utils/msg.hpp>
#include <uhd/utils/tasks.hpp>
#include <uhd/utils/safe_call.hpp>
#include <uhd/exception.hpp>
#include <boost/thread/thread.hpp>
//#include <uhd/transport/usb_zero_copy.hpp>
//#include <uhd/transport/bounded_buffer.hpp>
#include <boost/make_shared.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/math/special_functions/sign.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>



//#include <boost/filesystem.hpp>


using namespace uhd;
//using namespace uhd::usrp;
using namespace uhd::transport;

RadioPeripheral::RadioPeripheral() {
	// TODO Auto-generated constructor stub

}

RadioPeripheral::~RadioPeripheral() {
	// TODO Auto-generated destructor stub
}

void RadioPeripheral::Initialize(
		const InitArgs& iargs, const uhd::fs_path pathroot,
		A2300_iface::sptr dci_ctrl,
		uhd::property_tree::sptr tree)
{
	//Save the parameters.
	m_tree = tree;
	m_args = iargs;

	//Construct unique path.
    const fs_path pathFeTx = pathroot / "dboards/A/tx_frontends";
    const fs_path pathFeRx = pathroot / "dboards/A/rx_frontends";
    const fs_path pathDspTx = pathroot / "tx_dsps";
    const fs_path pathDspRx = pathroot / "rx_dsps";

    //Create RF configuration interface mappings.
    m_cfgRfTx.Initialize(iargs.idxRf, iargs.compidRF, true,  pathFeTx, dci_ctrl, tree);
    m_cfgRfRx.Initialize(iargs.idxRf, iargs.compidRF, false, pathFeRx, dci_ctrl, tree);

    //Create DSP configuration interface mappings.
    m_cfgDspTx.Initialize(iargs.idxTx, iargs.compidTx, pathDspTx, dci_ctrl,
    		boost::bind(&RadioPeripheral::OnUpdateTxSampleRate, this, _1), tree);
    m_cfgDspRx.Initialize(iargs.idxRx, iargs.compidRx, pathDspRx, dci_ctrl,
    		boost::bind(&RadioPeripheral::OnUpdateRxSampleRate, this, _1), tree);

    ////////////////////////////////////////////////////////////////////
    // radio control
    ////////////////////////////////////////////////////////////////////
    // const boost::uint32_t sid = (dspno == 0)? B200_CTRL0_MSG_SID : B200_CTRL1_MSG_SID;
    // perif.ctrl = radio_ctrl_core_3000::make(false/*lilE*/, _ctrl_transport, zero_copy_if::sptr()/*null*/, sid);
    // perif.ctrl->hold_task(_async_task);
    // _async_task_data->radio_ctrl[dspno] = perif.ctrl; //weak
    tree->access<time_spec_t>(pathroot / "time" / "cmd");
    //    	.subscribe(boost::bind(&RadioPeripheral::set_time, _1));
    // this->register_loopback_self_test(perif.ctrl);
    // perif.atr = gpio_core_200_32wo::make(perif.ctrl, TOREG(SR_ATR));

     ////////////////////////////////////////////////////////////////////
    // create time control objects
    ////////////////////////////////////////////////////////////////////
    // time_core_3000::readback_bases_type time64_rb_bases;
    // time64_rb_bases.rb_now = RB64_TIME_NOW;
    // time64_rb_bases.rb_pps = RB64_TIME_PPS;
    // perif.time64 = time_core_3000::make(perif.ctrl, TOREG(SR_TIME), time64_rb_bases);
}

void RadioPeripheral::SetTickRate( double rate)
{
	m_cfgDspTx.SetTickRate(rate);
	m_cfgDspRx.SetTickRate(rate);

	//TODO add timer64 tick rate support
    //perif.framer->set_tick_rate(_tick_rate);

}

void RadioPeripheral::OnUpdateTxSampleRate( const double rate)
{
	boost::shared_ptr<sph::send_packet_handler> my_streamer =
		boost::dynamic_pointer_cast<sph::send_packet_handler>(tx_streamer.lock());
	if (not my_streamer) return;
	my_streamer->set_samp_rate(rate);

	//TODO Adjust scaling factor as needed.
	//const double adj = m_perifs[dspno].duc->get_scaling_adjustment();
	//my_streamer->set_scale_factor(adj);
}

void RadioPeripheral::OnUpdateRxSampleRate( const double rate)
{
	boost::shared_ptr<sph::recv_packet_handler> my_streamer =
		boost::dynamic_pointer_cast<sph::recv_packet_handler>(rx_streamer.lock());
	if (not my_streamer) return;
	my_streamer->set_samp_rate(rate);

	//TODO Adjust scaling factor as needed.
	//const double adj = m_perifs[dspno].ddc->get_scaling_adjustment();
	//my_streamer->set_scale_factor(adj);

}




