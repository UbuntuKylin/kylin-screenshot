#ifndef AUDIOENCODER_H
#define AUDIOENCODER_H

#include "src/AV/output/BaseEncoder.h"

class AudioEncoder : public BaseEncoder {

private:
    struct SampleFormatData {
      QString m_name;
      AVSampleFormat m_format;
    };

private:
    static const std::vector<SampleFormatData> SUPPORTED_SAMPLE_FORMATS;
    static const unsigned int DEFAULT_FRAME_SAMPLES;

private:
#if !SSR_USE_AVCODEC_ENCODE_AUDIO2
    std::vector<uint8_t> m_temp_buffer;
#endif

public:
    AudioEncoder(Muxer* muxer, AVStream* stream, AVCodecContext* codec_context, AVCodec* codec, AVDictionary** options);
    ~AudioEncoder();

    unsigned int GetFrameSize();

    AVSampleFormat GetSampleFormat();

    unsigned int GetChannels();

    unsigned int GetSampleRate();

public:
    static bool AVCodecIsSupported(const QString& codec_name);
    static void PrepareStream(AVStream* stream, AVCodecContext* codec_context, AVCodec* codec, AVDictionary** options, const std::vector<std::pair<QString, QString>>& codec_options,
                              unsigned int bit_rate, unsigned int channels, unsigned int sample_rate);

private:
    virtual bool EncodeFrame(AVFrameWrapper *frame) override;

};

#endif // AUDIOENCODER_H
