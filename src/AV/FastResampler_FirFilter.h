#ifndef FASTRESAMPLER_FIRFILTER_H
#define FASTRESAMPLER_FIRFILTER_H

#include "src/core/Global.h"

typedef void (*FirFilter2Ptr)(unsigned int, unsigned int, float*, float*, float, float*, float*);

void FastResampler_FirFilter2_C1_Fallback(unsigned int channels, unsigned int filter_length, float* coef1, float* coef2, float frac, float* input, float* output);
void FastResampler_FirFilter2_C2_Fallback(unsigned int channels, unsigned int filter_length, float* coef1, float* coef2, float frac, float* input, float* output);
void FastResampler_FirFilter2_Cn_Fallback(unsigned int channels, unsigned int filter_length, float* coef1, float* coef2, float frac, float* input, float* output);

#if SSR_USE_X86_ASM
void FastResampler_FirFilter2_C1_SSE2(unsigned int channels, unsigned int filter_length, float* coef1, float* coef2, float frac, float* input, float* output);
void FastResampler_FirFilter2_C2_SSE2(unsigned int channels, unsigned int filter_length, float* coef1, float* coef2, float frac, float* input, float* output);
void FastResampler_FirFilter2_Cn_SSE2(unsigned int channels, unsigned int filter_length, float* coef1, float* coef2, float frac, float* input, float* output);
#endif


#endif // FASTRESAMPLER_FIRFILTER_H
