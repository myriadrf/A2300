//
// Copyright 2011-2013 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "dsp_core_a2300.hpp"
#include <A2300/A2300_Defs.h>

#include <uhd/types/dict.hpp>
#include <uhd/exception.hpp>
#include <uhd/utils/msg.hpp>
#include <uhd/utils/safe_call.hpp>
#include <uhd/utils/algorithm.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/thread/thread.hpp> //thread sleep
#include <boost/math/special_functions/round.hpp>
#include <boost/math/special_functions/sign.hpp>
#include <algorithm>
#include <cmath>

#include "Dci/DciMsg.h"
#include "Dci/DciConversation.h"
#include "Dci/InfrastructureMsgs.h"
#include "Dci/WcaMsgs.h"
#include "Dci/StandardMsgs.h"

#define REG_DSP_RX_FREQ       _dsp_base + 0
#define REG_DSP_RX_SCALE_IQ   _dsp_base + 4
#define REG_DSP_RX_DECIM      _dsp_base + 8
#define REG_DSP_RX_MUX        _dsp_base + 12

#define FLAG_DSP_RX_MUX_SWAP_IQ   (1 << 0)
#define FLAG_DSP_RX_MUX_REAL_MODE (1 << 1)

template <class T> T ceil_log2(T num){
    return std::ceil(std::log(num)/std::log(T(2)));
}

using namespace uhd;

class dsp_core_a2300_impl : public dsp_core_a2300 {
public:
    dsp_core_a2300_impl() :
    	_tick_rate(A2300_DEFAULT_TICK_RATE),
         _link_rate(A2300_DEFAULT_TICK_RATE),
         _scaling_adjustment(1.0),
         _dsp_extra_scaling(1.0),
         _host_extra_scaling(1.0),
         _fxpt_scalar_correction(0.0)
    {
        //init to something so update method has reasonable defaults
        _scaling_adjustment = 1.0;
        _dsp_extra_scaling = 1.0;
        //this->set_tick_rate(A2300_DEFAULT_TICK_RATE);
    }

    ~dsp_core_a2300_impl(void)
    {
        UHD_SAFE_CALL
        (
            //NOP
        )
    }

    void set_mux(const std::string &mode, const bool fe_swapped){
        static const uhd::dict<std::string, boost::uint32_t> mode_to_mux = boost::assign::map_list_of
            ("IQ", 0)
            ("QI", FLAG_DSP_RX_MUX_SWAP_IQ)
            ("I", FLAG_DSP_RX_MUX_REAL_MODE)
            ("Q", FLAG_DSP_RX_MUX_SWAP_IQ | FLAG_DSP_RX_MUX_REAL_MODE)
        ;
        //CJC _iface->poke32(REG_DSP_RX_MUX, mode_to_mux[mode] ^ (fe_swapped? FLAG_DSP_RX_MUX_SWAP_IQ : 0));
    }

    void set_tick_rate(const double rate){
        _tick_rate = rate;
    }

    void set_link_rate(const double rate){
        //_link_rate = rate/sizeof(boost::uint32_t); //in samps/s
        _link_rate = rate/sizeof(boost::uint16_t); //in samps/s (allows for 8sc)
    }

    uhd::meta_range_t get_host_rates(void){
        meta_range_t range;
        for (int rate = 512; rate > 256; rate -= 4){
            range.push_back(range_t(_tick_rate/rate));
        }
        for (int rate = 256; rate > 128; rate -= 2){
            range.push_back(range_t(_tick_rate/rate));
        }
        for (int rate = 128; rate >= int(std::ceil(_tick_rate/_link_rate)); rate -= 1){
            range.push_back(range_t(_tick_rate/rate));
        }
        return range;
    }

    double set_host_rate(const double rate){
        const size_t decim_rate = boost::math::iround(_tick_rate/this->get_host_rates().clip(rate, true));
        size_t decim = decim_rate;

        //determine which half-band filters are activated
        int hb0 = 0, hb1 = 0;
        if (decim % 2 == 0){
            hb0 = 1;
            decim /= 2;
        }
        if (decim % 2 == 0){
            hb1 = 1;
            decim /= 2;
        }

        //CJC _iface->poke32(REG_DSP_RX_DECIM, (hb1 << 9) | (hb0 << 8) | (decim & 0xff));

        if (decim > 1 and hb0 == 0 and hb1 == 0)
        {
            UHD_MSG(warning) << boost::format(
                "The requested decimation is odd; the user should expect CIC rolloff.\n"
                "Select an even decimation to ensure that a halfband filter is enabled.\n"
                "decimation = dsp_rate/samp_rate -> %d = (%f MHz)/(%f MHz)\n"
            ) % decim_rate % (_tick_rate/1e6) % (rate/1e6);
        }

        // Calculate CIC decimation (i.e., without halfband decimators)
        // Calculate closest multiplier constant to reverse gain absent scale multipliers
        const double rate_pow = std::pow(double(decim & 0xff), 4);
        _scaling_adjustment = std::pow(2, ceil_log2(rate_pow))/(1.65*rate_pow);
        this->update_scalar();

        return _tick_rate/decim_rate;
    }

    void update_scalar(void){
    	printf("update_scalar(0.0)\n");
#if 0
        const double factor = 1.0 + std::max(ceil_log2(_scaling_adjustment), 0.0);
    	printf("... _dsp_extra_scaling = %lf, factor = %lf\n", _dsp_extra_scaling, factor);

        const double target_scalar = (1 << 17)*_scaling_adjustment/_dsp_extra_scaling/factor;
        const boost::int32_t actual_scalar = boost::math::iround(target_scalar);
        printf("... extra_scalar = %d\n", actual_scalar);
#endif
        // CJC Overrice!
        _fxpt_scalar_correction = 0.0; // target_scalar/actual_scalar*factor; //should be small
        //CJC  _iface->poke32(REG_DSP_RX_SCALE_IQ, actual_scalar);
    }

    double get_scaling_adjustment(void){
    	return 1.0;
        //return _fxpt_scalar_correction*_host_extra_scaling/32767.;
    }

    double set_freq(const double freq_){
    	printf("dsp set_frequency(%lf)\n", freq_);

    	if( freq_ == 0.0 )
    	    volatile int i = 0;

    	// Round to KHz.
    	unsigned actual_freq = (unsigned)( freq_ / 1000.0 );
    	return( (double)actual_freq * 1000.0 );
    }

    uhd::meta_range_t get_freq_range(void)
    {
    	const double step_size = 1000.0;  // 1Khz steps.
    	// 0.3 to 3.8 GHz.
    	return uhd::meta_range_t( A2300_MIN_FREQ, A2300_MAX_FREQ, step_size);
        //return uhd::meta_range_t(-_tick_rate/2, +_tick_rate/2, _tick_rate/std::pow(2.0, 32));
    }

    void setup(const uhd::stream_args_t &stream_args){

        if (stream_args.otw_format == "sc16"){
            _dsp_extra_scaling = 1.0;
            _host_extra_scaling = 1.0;
        }
        else if (stream_args.otw_format == "sc8"){
            double peak = stream_args.args.cast<double>("peak", 1.0);
            peak = std::max(peak, 1.0/256);
            _host_extra_scaling = peak*256;
            _dsp_extra_scaling = peak;
        }
        else if (stream_args.otw_format == "sc12"){
            double peak = stream_args.args.cast<double>("peak", 1.0);
            peak = std::max(peak, 1.0/16);
            _host_extra_scaling = peak*16;
            _dsp_extra_scaling = peak;
        }
        else if (stream_args.otw_format == "fc32"){
            _host_extra_scaling = 1.0;
            _dsp_extra_scaling = 1.0;
        }
        else
    	{
        	//throw uhd::value_error("USRP RX cannot handle requested wire format: " + stream_args.otw_format);
        	printf("USRP RX cannot handle requested wire format: %s\n", stream_args.otw_format.c_str());
        	_dsp_extra_scaling = 1.0;
        	_host_extra_scaling = 1.0;
    	}

        _host_extra_scaling *= stream_args.args.cast<double>("fullscale", 1.0);

        this->update_scalar();
    }

private:
    double _tick_rate, _link_rate;
    double _scaling_adjustment, _dsp_extra_scaling, _host_extra_scaling, _fxpt_scalar_correction;
};

dsp_core_a2300::sptr dsp_core_a2300::make()
{
    return sptr(new dsp_core_a2300_impl());
}


