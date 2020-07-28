#ifndef FASTSCALER_H
#define FASTSCALER_H

#include "src/core/Global.h"

class FastScaler
{
private:
#if SSR_USE_X86_ASM
    bool m_warn_aligment;
#endif

    bool m_warn_swscale;
    SwsContext *m_sws_context;

public:
    FastScaler();
    ~FastScaler();
    void Scale(unsigned int in_width, unsigned int in_height, AVPixelFormat in_format, const uint8_t* const* in_data, const int* in_stride,
                unsigned int out_width, unsigned int out_height, AVPixelFormat out_format, uint8_t* const* out_data, const int* out_stride);


private:
    void Convert_BGRA_YUV444(unsigned int width, unsigned int height, const uint8_t* in_data, int in_stride, uint8_t* const out_data[3], const int out_stride[3]);
    void Convert_BGRA_YUV422(unsigned int width, unsigned int height, const uint8_t* in_data, int in_stride, uint8_t* const out_data[3], const int out_stride[3]);
    void Convert_BGRA_YUV420(unsigned int width, unsigned int height, const uint8_t* in_data, int in_stride, uint8_t* const out_data[3], const int out_stride[3]);
    void Convert_BGRA_NV12(unsigned int width, unsigned int height, const uint8_t* in_data, int in_stride, uint8_t* const out_data[2], const int out_stride[2]);
    void Convert_BGRA_BGR(unsigned int width, unsigned int height, const uint8_t* in_data, int in_stride, uint8_t* out_data, int out_stride);
    void Scale_BGRA(unsigned int in_width, unsigned int in_heidht, const uint8_t* in_data, int in_stride,
                    unsigned int out_width, unsigned int out_height, uint8_t* out_data, int out_stride);
};

#endif // FASTSCALER_H
