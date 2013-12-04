/** Name: DspConfig.cpp
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

#include "DspConfig.h"
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

#include "DciProperty.h"


using namespace uhd;

template <class T> T ceil_log2(T num){
    return std::ceil(std::log(num)/std::log(T(2)));
}

DspConfig::DspConfig()
    :
    	 m_idDsp(-1),
    	 m_idComponent(-1),
    	 m_tick_rate(A2300_DEFAULT_TICK_RATE),
         m_link_rate(A2300_DEFAULT_TICK_RATE),
         m_scaling_adjustment(1.0),
         m_dsp_extra_scaling(1.0),
         m_host_extra_scaling(1.0)
{
	// TODO Auto-generated constructor stub

}

DspConfig::~DspConfig() {
	// TODO Auto-generated destructor stub
}

void DspConfig::Initialize(
		int idDsp, int idComponent, const fs_path pathroot, A2300_iface::sptr dci_ctrl,
		UpdateSampleRateHandler handler,
		uhd::property_tree::sptr tree)
{
	//Save the relevant information.
	fs_path pathdsp = pathroot / str(boost::format("%u") % idDsp);
	m_idDsp 		= idDsp;
	m_idComponent 	= idComponent;
	m_dci_ctrl  	= dci_ctrl;
	m_handlerSampleRate = handler;

    //Set up Tree.
    //tree->access<double>(pathroot / "tick_rate")
    //		.subscribe(boost::bind(&DspConfig::SetTickRate, this, _1));


	//Set up DDUC configuration register properties.
	tree->create<byte>(pathdsp / "configreg" )
			.subscribe(boost::bind(&DspConfig::SetConfiguration, this,  _1))
			.set(0); //Disable it.

	//Set up Host Sampling rate range and value properties.
	tree->create<meta_range_t>(pathdsp / "rate" / "range")
    		.publish(boost::bind(&DspConfig::GetHostRates, this));

    tree->create<double>(pathdsp / "rate" / "value")
    		.subscribe(boost::bind(&DspConfig::SetHostRate, this,  _1))
        	.set(2e6);

    //Setup DDC Frequency control range and value settings.
    tree->create<meta_range_t>(pathdsp / "freq" / "range")
        .publish(boost::bind(&DspConfig::GetFreqRange, this));

    tree->create<double>(pathdsp / "freq" / "value")
					.subscribe(boost::bind(&DspConfig::SetFrequency, this, _1))
					//.publish(boost::bind(&DspConfig::GetFrequency, this, idDdcComp))
					.set(0.0);



	tree->create<stream_cmd_t>(pathdsp / "stream_cmd")
        .subscribe(boost::bind(&DspConfig::IssueStreamCommand, this, _1));



    this->UpdateScalar();
}

void DspConfig::ConfigStreaming(const uhd::stream_args_t &stream_args)
{
	//Determine scaling of values.
    if (stream_args.otw_format == "sc16")
    {
        m_dsp_extra_scaling = 1.0;
        m_dsp_extra_scaling = 1.0;
    }
    else if (stream_args.otw_format == "sc8")
    {
        double peak = stream_args.args.cast<double>("peak", 1.0);
        peak = std::max(peak, 1.0/256);
        m_dsp_extra_scaling = peak*256;
        m_dsp_extra_scaling = peak;
    }
    else
	{
    	m_dsp_extra_scaling = 1.0;
    	m_dsp_extra_scaling = 1.0;
	}

    //m_dsp_extra_scaling *= stream_args.args.cast<double>("fullscale", 1.0);

}


void DspConfig::SetTickRate(const double rate)
{
    m_tick_rate = rate;
}

void DspConfig::SetLinkRate(const double rate){
    m_link_rate = rate/sizeof(boost::uint16_t); //in samps/s (allows for 8sc)
}

uhd::meta_range_t DspConfig::GetHostRates(void){
    meta_range_t range;
    for (int rate = 512; rate > 256; rate -= 4){
        range.push_back(range_t(m_tick_rate/rate));
    }
    for (int rate = 256; rate > 128; rate -= 2){
        range.push_back(range_t(m_tick_rate/rate));
    }
    for (int rate = 128; rate >= int(std::ceil(m_tick_rate/m_link_rate)); rate -= 1){
        range.push_back(range_t(m_tick_rate/rate));
    }
    return range;
}




double DspConfig::SetHostRate(const double rate){
    const size_t decim_rate = boost::math::iround(m_tick_rate/this->GetHostRates().clip(rate, true));
    int32 decim = decim_rate;

    //determine which half-band filters are activated
    int hb0 = 0, hb1 = 0;
    if (decim % 2 == 0){
        hb0 = 1;
        decim /= 2;
    }

    //Current design has one half band filter.
//    if (decim % 2 == 0){
//        hb1 = 1;
//        decim /= 2;
//    }

    //Set the decimation rate and turn on halband filters on ASR-2300.
    //UHD_MSG(status) << boost::format( "DDUC Decimation = %d") % decim;
    // DSP_DDUC_SAMPRATE(WcaWriteWordRegister)
    // Decimation / Integration sampling rate conversion .  This is the
    // rate for down/up sampling sampling the 32 MHz sampling rate.  Valid
    // range is 2 to 4096 equivalent to 8 MHz to  3906.25 KHz.
	DciProperty prop(m_idComponent, m_dci_ctrl, A2300_WAIT_TIME);
	prop.SetProperty<uint16>(DSP_DDUC_SAMPRATE,  (uint16) decim );




	if (decim > 1 and hb0 == 0 and hb1 == 0)
    {
        UHD_MSG(warning) << boost::format(
            "The requested decimation is odd; the user should expect CIC rolloff.\n"
            "Select an even decimation to ensure that a halfband filter is enabled.\n"
            "decimation = dsp_rate/samp_rate -> %d = (%f MHz)/(%f MHz)\n"
        ) % decim_rate % (m_tick_rate/1e6) % (rate/1e6);
    }

    // Calculate CIC decimation (i.e., without halfband decimators)
    // Calculate closest multiplier constant to reverse gain absent scale multipliers
    const double rate_pow = std::pow(double(decim & 0xff), 4);
    m_scaling_adjustment = std::pow(2, ceil_log2(rate_pow))/(1.65*rate_pow);
    this->UpdateScalar();

    m_handlerSampleRate( rate);

    return m_tick_rate/decim_rate;
}

void DspConfig::UpdateScalar(void){

    const double factor = 1.0 + std::max(ceil_log2(m_scaling_adjustment), 0.0);
	//printf("... m_dsp_extra_scaling = %lf, factor = %lf\n", m_dsp_extra_scaling, factor);

    const double target_scalar = (1 << 17)*m_scaling_adjustment/m_dsp_extra_scaling/factor;
    const boost::int32_t actual_scalar = boost::math::iround(target_scalar);
    //printf("... extra_scalar = %d\n", actual_scalar);

    //_fxpt_scalar_correction = 0.0; // target_scalar/actual_scalar*factor; //should be small

    //TODO Update scaling parameters
    //Send DCI message.
}


double DspConfig::GetScalingAdjustment(void){
	return 1.0;
    //return _fxpt_scalar_correction*m_dsp_extra_scaling/32767.;
}

double DspConfig::SetFrequency(const double freq)
{
	uint32  deltaphase = (uint32)  boost::math::iround(freq/A2300_MAX_SAMPLING_FREQ * 2147483648);

	DciProperty prop(m_idComponent, m_dci_ctrl, A2300_WAIT_TIME);
	prop.SetProperty<uint32>(DSP_DDUC_PHASERATE, deltaphase );

	return ( freq );
}

uhd::meta_range_t DspConfig::GetFreqRange(void)
{
	return uhd::meta_range_t( A2300_DDUC_FREQ_MIN, A2300_DDUC_FREQ_MAX, A2300_DDUC_FREQ_STEP);
}


byte DspConfig::SetConfiguration( const byte config)
{
	DciProperty prop(m_idComponent, m_dci_ctrl, A2300_WAIT_TIME);
	prop.SetProperty<byte>(DSP_DDUC_CTRL, config);
	return config;
}


void DspConfig::IssueStreamCommand( const uhd::stream_cmd_t &stream_cmd)
{
	if ( this->m_idComponent < 0)
	{
		UHD_MSG(warning) << "ASR-2300 DSP Configuration not setup yet!";
		return;
	}
	UHD_ASSERT_THROW(stream_cmd.num_samps <= 0x0fffffff);
	m_bContinuousStreaming = (stream_cmd.stream_mode == stream_cmd_t::STREAM_MODE_START_CONTINUOUS);

	//setup the mode to instruction flags
	typedef boost::tuple<bool, bool, bool, bool> inst_t;
	static const uhd::dict<stream_cmd_t::stream_mode_t, inst_t> mode_to_inst = boost::assign::map_list_of
															//reload, chain, samps, stop
		(stream_cmd_t::STREAM_MODE_START_CONTINUOUS,   inst_t(true,  true,  false, false))
		(stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS,    inst_t(false, false, false, true))
		(stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE, inst_t(false, false, true,  false))
		(stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_MORE, inst_t(false, true,  true,  false))
	;

	//setup the instruction flag values
	bool inst_reload, inst_chain, inst_samps, inst_stop;
	boost::tie(inst_reload, inst_chain, inst_samps, inst_stop) = mode_to_inst[stream_cmd.stream_mode];

	//calculate the word from flags and length
	boost::uint32_t cmd_word = 0;
	cmd_word |= boost::uint32_t((stream_cmd.stream_now)? 1 : 0) << 31;
	cmd_word |= boost::uint32_t((inst_chain)?            1 : 0) << 30;
	cmd_word |= boost::uint32_t((inst_reload)?           1 : 0) << 29;
	cmd_word |= boost::uint32_t((inst_stop)?             1 : 0) << 28;
	cmd_word |= (inst_samps)? stream_cmd.num_samps : ((inst_stop)? 0 : 1);


	//TODO map this call to the Property Tree so configuration state remains in synch.
	if( stream_cmd.stream_now)
		SetConfiguration( 0x01);


}
