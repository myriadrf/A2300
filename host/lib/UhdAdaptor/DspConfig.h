// Name:  DspConfig.h
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

#ifndef DSPCONFIG_H_
#define DSPCONFIG_H_

#include <uhd/config.hpp>
#include <uhd/stream.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/property_tree.hpp>
#include "a2300_iface.hpp"

/**
 * Class implements DSP Configuration helpers for Asr2300.
 */
class DspConfig {
public:
	DspConfig();
	~DspConfig();

    void Initialize( int idDsp,
    		int idComponent, const uhd::fs_path pathroot,
    		A2300_iface::sptr dci_ctrl, uhd::property_tree::sptr tree);

    void ConfigStreaming(const uhd::stream_args_t &stream_args);

protected:
    void SetTickRate(const double rate);
    void SetLinkRate(const double rate);

    double SetHostRate(const double rate);
    double SetFrequency(const double freq);


    uhd::meta_range_t GetHostRates(void);

    double GetScalingAdjustment(void);

    uhd::meta_range_t GetFreqRange(void);




    void IssueStreamCommand( const uhd::stream_cmd_t &stream_cmd);

    void UpdateScalar( void);

private:
    int    m_idDsp;
    int    m_idComponent;
    bool   m_bContinuousStreaming;
    double m_tick_rate, m_link_rate;
    double m_scaling_adjustment, m_dsp_extra_scaling, m_host_extra_scaling;
    A2300_iface::sptr  m_dci_ctrl;
};

#endif /* DSPCONFIG_H_ */
