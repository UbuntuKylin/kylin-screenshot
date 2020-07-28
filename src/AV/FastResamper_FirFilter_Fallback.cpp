#include "FastResampler_FirFilter.h"

void FastResampler_FirFilter2_C1_Fallback(unsigned int channels, unsigned int filter_length, float* coef1, float* coef2, float frac, float* input, float* output) {
    Q_UNUSED(channels);
    float sum[4] = {0.0f};
    for(unsigned int i = 0; i < filter_length / 4; ++i) {
        float filter_value[4] = {
            coef1[0] + (coef2[0] - coef1[0]) * frac,
            coef1[1] + (coef2[1] - coef1[1]) * frac,
            coef1[2] + (coef2[2] - coef1[2]) * frac,
            coef1[3] + (coef2[3] - coef1[3]) * frac,
        };
        coef1 += 4; coef2 += 4;
        sum[0] += input[0] * filter_value[0];
        sum[1] += input[1] * filter_value[1];
        sum[2] += input[2] * filter_value[2];
        sum[3] += input[3] * filter_value[3];
        input += 4;
    }
    output[0] = sum[0] + sum[1] + sum[2] + sum[3];
}

void FastResampler_FirFilter2_C2_Fallback(unsigned int channels, unsigned int filter_length, float* coef1, float* coef2, float frac, float* input, float* output) {
    Q_UNUSED(channels);
    float sum[4] = {0.0f};
    for(unsigned int i = 0; i < filter_length / 4; ++i) {
        float filter_value[4] = {
            coef1[0] + (coef2[0] - coef1[0]) * frac,
            coef1[1] + (coef2[1] - coef1[1]) * frac,
            coef1[2] + (coef2[2] - coef1[2]) * frac,
            coef1[3] + (coef2[3] - coef1[3]) * frac,
        };
        coef1 += 4; coef2 += 4;
        sum[0] += input[0] * filter_value[0];
        sum[1] += input[1] * filter_value[0];
        sum[2] += input[2] * filter_value[1];
        sum[3] += input[3] * filter_value[1];
        sum[0] += input[4] * filter_value[2];
        sum[1] += input[5] * filter_value[2];
        sum[2] += input[6] * filter_value[3];
        sum[3] += input[7] * filter_value[3];
        input += 8;
    }
    output[0] = sum[0] + sum[2];
    output[1] = sum[1] + sum[3];
}

void FastResampler_FirFilter2_Cn_Fallback(unsigned int channels, unsigned int filter_length, float* coef1, float* coef2, float frac, float* input, float* output) {
    Q_UNUSED(channels);
    for(unsigned int c = 0; c < channels; ++c) {
        float sum[4] = {0.0f};
        float *input2 = input + c;
        for(unsigned int i = 0; i < filter_length / 4; ++i) {
            float filter_value[4] = {
                coef1[0] + (coef2[0] - coef1[0]) * frac,
                coef1[1] + (coef2[1] - coef1[1]) * frac,
                coef1[2] + (coef2[2] - coef1[2]) * frac,
                coef1[3] + (coef2[3] - coef1[3]) * frac,
            };
            coef1 += 4; coef2 += 4;
            sum[0] += *input2 * filter_value[0]; input2 += channels;
            sum[1] += *input2 * filter_value[1]; input2 += channels;
            sum[2] += *input2 * filter_value[2]; input2 += channels;
            sum[3] += *input2 * filter_value[3]; input2 += channels;
        }
        output[c] = sum[0] + sum[1] + sum[2] + sum[3];
    }
}
