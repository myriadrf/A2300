/** Name: RadioPeripheral.h
*
* Copyright(c) 2013 Loctronix Corporation
* http://www.loctronix.com
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef RADIOPERIPHERAL_H_
#define RADIOPERIPHERAL_H_

#include <uhd/stream.hpp>
#include <uhd/types/stream_cmd.hpp>
#include <uhd/device.hpp>
#include <uhd/property_tree.hpp>
#include <boost/weak_ptr.hpp>

#include "RfConfig.h"
#include "DspConfig.h"

/**
 * Class implements a complete RF TX/RX signal processing chain on the 2300.
 */
class RadioPeripheral
{
public:
    struct InitArgs
    {
    	int idxPeriph; // Peripheral ID.
    	int idxRx;  //RX DSP Channel index
    	int idxTx;	//TX DSP Channel index
    	int idxRf;	//RF Front end index
    	byte compidRx; 	//RX DSP WCA Component ID
    	byte compidTx;	//TX DSP WCA Component ID
    	byte compidRF;	//RF WCA Component ID
    };

public:
	RadioPeripheral();
	~RadioPeripheral();

	void Initialize(
			const InitArgs& iargs, const uhd::fs_path pathroot,
    		A2300_iface::sptr dci_ctrl,
    		uhd::property_tree::sptr tree);

	DspConfig&  Ddc() { return m_cfgDspRx;}
	DspConfig&  Duc() { return m_cfgDspTx;}
	RfConfig&   Rx()  { return m_cfgRfRx;}
	RfConfig&   Tx()  { return m_cfgRfTx;}

	void TxStreamer( boost::weak_ptr<uhd::tx_streamer> txstreamer){ tx_streamer = txstreamer;}
	boost::weak_ptr<uhd::tx_streamer> TxStreamer(){ return tx_streamer;}

	void RxStreamer( boost::weak_ptr<uhd::rx_streamer> rxstreamer){ rx_streamer = rxstreamer;}
	boost::weak_ptr<uhd::rx_streamer> RxStreamer(){ return rx_streamer;}

	void SetTickRate( double rate);
protected:
	void OnUpdateTxSampleRate( const double rate);
	void OnUpdateRxSampleRate( const double rate);



private:
	InitArgs  						  m_args;

	boost::weak_ptr<uhd::rx_streamer> rx_streamer;
    boost::weak_ptr<uhd::tx_streamer> tx_streamer;

    uhd::property_tree::sptr 		  m_tree;
    DspConfig						  m_cfgDspRx;
    DspConfig						  m_cfgDspTx;
    RfConfig						  m_cfgRfRx;
    RfConfig						  m_cfgRfTx;
};

#endif /* RADIOPERIPHERAL_H_ */
