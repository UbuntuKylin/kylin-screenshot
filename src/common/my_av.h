#pragma once

extern "C" {
#include <libavutil/rational.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
//#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/pixfmt.h>
#include <libavutil/samplefmt.h>
}
#include <exception>

// convert weird types from libav/ffmpeg to doubles
inline double ToDouble(const AVRational& r) {
    return (double) r.num / (double) r.den;
}

class LibavException : public std::exception {
public:
    inline virtual const char * what() const throw() override {
        return "LibavException";
    }
};
class ResamplerException : public std::exception {
public:
    inline virtual const char* what() const throw() override {
        return "ResamplerException";
    }
};

#if SSR_USE_PULSEAUDIO
class PulseAudioException : public std::exception {
public:
    inline virtual const char* what() const throw() override {
        return "PulseAudioException";
    }
};
#endif
