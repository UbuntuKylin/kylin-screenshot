#ifndef VIDEOENCODER_H
#define VIDEOENCODER_H

#include "BaseEncoder.h"


class VideoEncoder: public BaseEncoder {
private:
    struct PixelFormatData {
        QString m_name;
        AVPixelFormat m_format;
        bool m_is_yuv;
    };

private:
    static const std::vector<PixelFormatData> SUPPORTED_PIXEL_FORMATS;

private:
#if !SSR_USE_AVCODEC_ENCODE_VIDEO2
    std::vector<uint8_t> m_temp_buffer;
#endif

public:
    VideoEncoder(Muxer* muxer, AVStream* stream, AVCodecContext* codec_context, AVCodec* codec, AVDictionary** options);
    ~VideoEncoder();

    AVPixelFormat GetPixelFormat();

    unsigned int GetWidth();
    unsigned int GetHeight();
    unsigned int GetFrameRate();

public:
    static bool AVCodecIsSupported(const QString& codec_name);
    static void PrepareStream(AVStream *stream, AVCodecContext* codec_context, AVCodec* codec, AVDictionary** options, const std::vector<std::pair<QString, QString>>& codec_options,
                              unsigned int bit_rate, unsigned int width, unsigned int height, unsigned int frame_rate);

private:
    virtual bool EncodeFrame(AVFrameWrapper* frame) override;
};

#endif // VIDEOENCODER_H
