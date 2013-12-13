/** Name: DspConfig.h
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

#ifndef DSPCONFIG_H_
#define DSPCONFIG_H_

#include <uhd/config.hpp>
#include <uhd/stream.hpp>
#include <uhd/types/ranges.hpp>
#include <uhd/property_tree.hpp>
#include "a2300_iface.hpp"
#include <boost/function.hpp>
#include <boost/bind.hpp>

/**
 * Class implements DSP Configuration helpers for Asr2300.
 */
class DspConfig
{
	typedef boost::function<void(const double )> UpdateSampleRateHandler;

public:

	DspConfig();
	~DspConfig();

    void Initialize( int idDsp,
    		byte idComponent, const uhd::fs_path pathroot,
    		A2300_iface::sptr dci_ctrl, UpdateSampleRateHandler handler, uhd::property_tree::sptr tree);

    void IssueStreamCommand( const uhd::stream_cmd_t &stream_cmd);
    void ConfigStreaming(const uhd::stream_args_t &stream_args);
    void SetTickRate(const double rate);
protected:

    void SetLinkRate(const double rate);

    byte SetConfiguration( const byte config);
    double SetHostRate(const double rate);
    double SetFrequency(const double freq);

    uhd::meta_range_t GetHostRates(void);

    double GetScalingAdjustment(void);

    uhd::meta_range_t GetFreqRange(void);

    void UpdateScalar( void);

private:

    int    m_idDsp;
    byte   m_idComponent;
    bool   m_bContinuousStreaming;
    double m_tick_rate;
    double m_link_rate;
    double m_scaling_adjustment;
    double m_dsp_extra_scaling;
    double m_host_extra_scaling;
    UpdateSampleRateHandler  m_handlerSampleRate;
    A2300_iface::sptr  m_dci_ctrl;

};

#endif /* DSPCONFIG_H_ */
