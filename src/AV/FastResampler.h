#ifndef FASTRESAMPLER_H
#define FASTRESAMPLER_H


#include "src/core/Global.h"
#include "src/common/TempBuffer.h"
#include "src/common/QueueBuffer.h"

#include "FastResampler_FirFilter.h"

#include <utility>

class FastResampler {

private:
#if SSR_USE_X86_ASM
    // CPU feature detection
    bool m_use_sse2;
#endif

    // settings
    unsigned int m_channels;
    float m_gain;
    double m_resample_ratio, m_drift_ratio;

    // filter coefficient sets
    unsigned int m_filter_length, m_filter_rows;
    TempBuffer<float> m_filter_coefficients;

    // resampler state
    double m_time;
    QueueBuffer<float> m_samples_memory;

    // function pointers
    FirFilter2Ptr m_firfilter2_ptr;

public:
    FastResampler(unsigned int channels, float gain);

    // Processes input audio and writes the resampled audio to a queue. 'samples_in' can be NULL to flush the resampler.
    unsigned int Resample(double resample_ratio, double drift_ratio, const float* samples_in, unsigned int sample_count_in, TempBuffer<float>* samples_out, unsigned int sample_offset_out);

    // Returns the total resampler latency in input samples.
    double GetInputLatency();

    // Returns the total resampler latency in output samples.
    double GetOutputLatency();

private:
    void UpdateFilterCoefficients();
    void ResetResamplerState();

    std::pair<unsigned int, unsigned int> ResampleBatch(float* samples_in, unsigned int sample_count_in, float* samples_out);

};


#endif // FASTRESAMPLER_H
