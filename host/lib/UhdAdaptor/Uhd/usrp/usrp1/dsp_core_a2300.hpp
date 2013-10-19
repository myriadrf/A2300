#ifndef INCLUDED_ASR2300_DSP_CORE_A2300_HPP
#define INCLUDED_ASR2300_DSP_CORE_A2300_HPP

#include <uhd/config.hpp>
#include <uhd/stream.hpp>
#include <uhd/types/ranges.hpp>
#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>

class dsp_core_a2300 : boost::noncopyable{
public:
    typedef boost::shared_ptr<dsp_core_a2300> sptr;

    static sptr make();

    virtual void set_tick_rate(const double rate) = 0;

    virtual void set_link_rate(const double rate) = 0;

    virtual double set_host_rate(const double rate) = 0;

    virtual uhd::meta_range_t get_host_rates(void) = 0;

    virtual double get_scaling_adjustment(void) = 0;

    virtual uhd::meta_range_t get_freq_range(void) = 0;

    virtual double set_freq(const double freq) = 0;

    virtual void setup(const uhd::stream_args_t &stream_args) = 0;
};

#endif /* INCLUDED_ASR2300_DSP_CORE_A2300_HPP */

 
