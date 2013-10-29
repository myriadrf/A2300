// Name:  RfConfig.h
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

    void Initialize(int idRf, int idComponent, bool bTx, const uhd::fs_path pathroot, A2300_iface::sptr dci_ctrl, uhd::property_tree::sptr tree);

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
    int    m_idComponent;
    A2300_iface::sptr  m_dci_ctrl;

};

#endif /* RFCONFIG_H_ */
