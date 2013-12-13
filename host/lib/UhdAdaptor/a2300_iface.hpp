// Name:  a2300_iface.hpp
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

#ifndef INCLUDED_A2300_IFACE_HPP
#define INCLUDED_A2300_IFACE_HPP

#include <uhd/types/serial.hpp>
#include <uhd/transport/usb_zero_copy.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <Dci/DciConversation.h>

#define SPI_ENABLE_FPGA  0x01
#define SPI_FMT_HDR_MASK (3 << 5)
#define SPI_FMT_HDR_0    (0 << 5)
#define SPI_FMT_HDR_1    (1 << 5)
#define SPI_FMT_HDR_2    (2 << 5)
#define SPI_FMT_LSB      (1 << 7)
#define SPI_FMT_MSB      (0 << 7)
#define SPI_FMT_xSB_MASK (1 << 7)
#define VRQ_SPI_READ     0x82
#define VRQ_SPI_WRITE    0x09
#define VRQ_FW_COMPAT    0x83

struct libusb_device;

/*!
 * The A2300_iface  interface class:
 * Provides a set of functions to implementation layer.
 * This includes reading and writing DCI commands.
 */
class A2300_iface :  boost::noncopyable{
public:
    typedef boost::shared_ptr<A2300_iface> sptr;

    virtual ~A2300_iface() {};

    /*!
     * Make a implementation layer interface
     *
     * \param idConversation the DCI conversation identifier
     * \param handle a device handle that uniquely identifying the device
     * \param recv_interface an integer specifiying an IN interface number
     * \param recv_endpoint an integer specifiying an IN endpoint number
     * \param send_interface an integer specifiying an OUT interface number
     * \param send_endpoint an integer specifiying an OUT endpoint number
     * \param hints optional parameters to pass to the underlying transport
     * \return a new zero copy usb object
     */
    static sptr make(
    	uhd::transport::usb_zero_copy::sptr transport,
        const byte idConversation
    );

    virtual void SendIdleMsg(double timeout) = 0;

	/*
	 * DCI Message Receive.
	 */
	virtual uhd::transport::managed_recv_buffer::sptr ReceiveDciMessage( double timeout) = 0;

	virtual uhd::transport::managed_send_buffer::sptr GetSendDciMessageBuffer( double timeout) = 0;
	virtual void CommitDciMessageBuffer( uhd::transport::managed_send_buffer::sptr mbuff, size_t msgLen, bool bAck ) = 0;

	virtual void Test() {};
};


#endif /* INCLUDED_USRP1_IFACE_HPP */
