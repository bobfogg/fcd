/* -*- mode:c++; c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Vamp

    An API for audio analysis and feature extraction plugins.

    Centre for Digital Music, Queen Mary, University of London.
    Copyright 2006 Chris Cannam.

    FindPulseFDBatch.h
    Copyright 2012 John Brzustowski

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the names of the Centre for
    Digital Music; Queen Mary, University of London; and Chris Cannam
    shall not be used in advertising or otherwise to promote the sale,
    use or other dealings in this Software without prior written
    authorization.
*/

#ifndef _FIND_PULSEFDBATCH_PLUGIN_H_
#define _FIND_PULSEFDBATCH_PLUGIN_H_

#include <boost/circular_buffer.hpp>
#include "vamp-sdk/Plugin.h"
#include "PulseFinder.h"
#include <complex>
#include <fftw3.h>

/**
 * Look for pulses from Lotek tags - Frequency Domain version
*/

class FindPulseFDBatch : public Vamp::Plugin
{
public:
    FindPulseFDBatch(float inputSampleRate);
    virtual ~FindPulseFDBatch();

    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();

    InputDomain getInputDomain() const { return TimeDomain; }
    size_t getMinChannelCount() const {return 2;}
    size_t getMaxChannelCount() const {return 2;}
    size_t getPreferredStepSize() const {return 8192;}
    size_t getPreferredBlockSize() const {return 8192;}
    std::string getIdentifier() const;
    std::string getName() const;
    std::string getDescription() const;
    std::string getMaker() const;
    int getPluginVersion() const;
    std::string getCopyright() const;

    ParameterList getParameterDescriptors() const;
    float getParameter(std::string id) const;
    void setParameter(std::string id, float value);

    OutputList getOutputDescriptors() const;

    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);

    FeatureSet getRemainingFeatures();

protected:
    size_t m_channels;
    size_t m_stepSize;
    size_t m_blockSize;
    
    // paramters
    float m_plen;        // width of pulse we're trying to detect, in ms
    float m_min_pulse_power_dB; // min power for pulse to be accepted
    int m_fft_win_size;  // number of consecutive samples in non-overlapping FFT windows
    float m_min_freq;  // only accept pulses from bins whose centre frequency is at least this
    float m_max_freq;  // only accept pulses from bins whose centre frequency is at most this

    // parameter defaults
    static float m_default_plen;                   // width of pulse we're trying to detect, in ms
    static float m_default_min_pulse_power_dB;     // min value of pulse power to be accepted
    static int   m_default_fft_win_size;           // number of consecutive samples in non-overlapping FFT wi
    static float m_default_min_freq;  // only accept pulses from bins whose centre frequency is at least this
    static float m_default_max_freq;  // only accept pulses from bins whose centre frequency is at most this

    // internal registers
    float m_probe_scale; // divisor to convert raw probe value to power
    float m_min_probe; // scaled value of m_min_pulse_power_dB
    float *m_windowed[2 * 2]; // windowed data in time domain (one buffer for each phase of overlapping window sequence)
    fftwf_complex *m_fft[2]; // DFT of power for each channel
    fftwf_plan m_plan[2 * 2]; // lazily generate a plan for both input phase windows
    bool m_have_fft_plan;
    int m_pf_size; // size of peak finder moving average window (in units of fft windows)
    float m_min_pulse_power; // minimum pulse power to be accepted (raw units)

    int m_num_windowed_samples[2];  // number of samples put in m_windowed array since last fft; one for each phase window
    int m_first_freq_bin; // index of first frequency bin to monitor
    int m_last_freq_bin; // index of last frequency bin to monitor

    std::vector < PulseFinder < float > > m_freq_bin_pulse_finder;
};


#endif // _FIND_PULSEFDBATCH_PLUGIN_H_