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
#include <boost/math/special_functions/round.hpp>
#include <boost/math/special_functions/sign.hpp>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>
//#include <boost/filesystem.hpp>
#include <uhd/exception.hpp>
#include <uhd/utils/msg.hpp>

using namespace uhd;

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
    m_cfgDspTx.Initialize(iargs.idxTx, iargs.compidTx, pathDspTx, dci_ctrl, tree);
    m_cfgDspRx.Initialize(iargs.idxRx, iargs.compidRx, pathDspRx, dci_ctrl, tree);

    ////////////////////////////////////////////////////////////////////
    // radio control
    ////////////////////////////////////////////////////////////////////
    //CJC const boost::uint32_t sid = (dspno == 0)? B200_CTRL0_MSG_SID : B200_CTRL1_MSG_SID;
    //CJC perif.ctrl = radio_ctrl_core_3000::make(false/*lilE*/, _ctrl_transport, zero_copy_if::sptr()/*null*/, sid);
    //CJC perif.ctrl->hold_task(_async_task);
    //CJC _async_task_data->radio_ctrl[dspno] = perif.ctrl; //weak
    tree->access<time_spec_t>(pathroot / "time" / "cmd");
    //CJC    	.subscribe(boost::bind(&a2300_impl::set_time, _1));
    //CJC this->register_loopback_self_test(perif.ctrl);
    //CJC perif.atr = gpio_core_200_32wo::make(perif.ctrl, TOREG(SR_ATR));

     ////////////////////////////////////////////////////////////////////
    // create time control objects
    ////////////////////////////////////////////////////////////////////
    //CJC time_core_3000::readback_bases_type time64_rb_bases;
    //CJC time64_rb_bases.rb_now = RB64_TIME_NOW;
    //CJC time64_rb_bases.rb_pps = RB64_TIME_PPS;
    //CJC perif.time64 = time_core_3000::make(perif.ctrl, TOREG(SR_TIME), time64_rb_bases);


}
