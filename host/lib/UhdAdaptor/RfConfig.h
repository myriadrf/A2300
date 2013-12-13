/** Name: RfConfig.h
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

#ifndef RFCONFIG_H_
#define RFCONFIG_H_

#include <uhd/config.hpp>
#include <uhd/stream.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/property_tree.hpp>
#include "a2300_iface.hpp"

/**
 * Class implements RF Configuration helpers for the Asr2300.
 */
class RfConfig {
public:
	RfConfig();
	~RfConfig();

    void Initialize(int idRf, byte idComponent, bool bTx, const uhd::fs_path pathroot, A2300_iface::sptr dci_ctrl, uhd::property_tree::sptr tree);

protected:
    void 				SetPgaGain( const double gain);
    uhd::meta_range_t 	GetPgaGainRange(void);

    void 				SetRfBandwidth( const double bandwidth);
    uhd::meta_range_t 	GetRfBandwidthRange(void);
    int 				GetBandwidthIndex( const double bandwidth);

    void 				SetRfFrequency( const double freq);
    uhd::meta_range_t 	GetRfFrequencyRange(void);

    void   SetPathProfile( const std::string& sPathId);
private:
    int    m_idRf;
    bool   m_bIsTx;
    byte   m_idComponent;
    A2300_iface::sptr  m_dci_ctrl;

};

#endif /* RFCONFIG_H_ */
