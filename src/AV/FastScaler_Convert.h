#ifndef FASTSCALER_CONVERT_H
#define FASTSCALER_CONVERT_H

#include <cstdint>

void Convert_BGRA_YUV444_Fallback(unsigned int w, unsigned int h, const uint8_t* in_data, int in_stride, uint8_t* const out_data[3], const int out_stride[3]);
void Convert_BGRA_YUV422_Fallback(unsigned int w, unsigned int h, const uint8_t* in_data, int in_stride, uint8_t* const out_data[3], const int out_stride[3]);
void Convert_BGRA_YUV420_Fallback(unsigned int w, unsigned int h, const uint8_t* in_data, int in_stride, uint8_t* const out_data[3], const int out_stride[3]);
void Convert_BGRA_NV12_Fallback(unsigned int w, unsigned int h, const uint8_t* in_data, int in_stride, uint8_t* const out_data[2], const int out_stride[2]);
void Convert_BGRA_BGR_Fallback(unsigned int w, unsigned int h, const uint8_t* in_data, int in_stride, uint8_t* out_data, int out_stride);

#if SSR_USE_X86_ASM
void Convert_BGRA_YUV444_SSSE3(unsigned int w, unsigned int h, const uint8_t* in_data, int in_stride, uint8_t* const out_data[3], const int out_stride[3]);
void Convert_BGRA_YUV422_SSSE3(unsigned int w, unsigned int h, const uint8_t* in_data, int in_stride, uint8_t* const out_data[3], const int out_stride[3]);
void Convert_BGRA_YUV420_SSSE3(unsigned int w, unsigned int h, const uint8_t* in_data, int in_stride, uint8_t* const out_data[3], const int out_stride[3]);
void Convert_BGRA_NV12_SSSE3(unsigned int w, unsigned int h, const uint8_t* in_data, int in_stride, uint8_t* const out_data[2], const int out_stride[2]);
void Convert_BGRA_BGR_SSSE3(unsigned int w, unsigned int h, const uint8_t* in_data, int in_stride, uint8_t* out_data, int out_stride);
#endif


#endif // FASTSCALER_CONVERT_H
