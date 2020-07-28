#ifndef FASTSCALER_SCALE_H
#define FASTSCALER_SCALE_H

#include <cstdint>

void Scale_BGRA_Fallback(unsigned int in_w, unsigned int in_h, const uint8_t* in_data, int in_stride,
                         unsigned int out_w, unsigned int out_h, uint8_t* out_data, int out_stride);

#if SSR_USE_X86_ASM
void Scale_BGRA_SSSE3(unsigned int in_w, unsigned int in_h, const uint8_t* in_data, int in_stride,
                      unsigned int out_w, unsigned int out_h, uint8_t* out_data, int out_stride);
#endif

#endif // FASTSCALER_SCALE_H
