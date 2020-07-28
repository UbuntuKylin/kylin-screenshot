#ifndef OUTPUTSETTINGS_H
#define OUTPUTSETTINGS_H

#include <QString>
#include <vector>
#include <map>
#include <libavutil/pixfmt.h>
#include <libavutil/samplefmt.h>

struct OutputSettings
{
    QString file;
    QString container_avname;

    QString video_codec_avname;
    unsigned int video_kbit_rate;
    std::vector<std::pair<QString, QString>> video_options;
    unsigned int video_width, video_height;
    unsigned int video_frame_rate;
    bool video_allow_frame_skipping;

    QString audio_codec_avname;
    unsigned int audio_kbit_rate;
    std::vector<std::pair<QString, QString>> audio_options;
    unsigned int audio_channels, audio_sample_rate;
};


struct OutputFormat
{
    bool m_video_enabled;
    unsigned int m_video_width, m_video_height;
    unsigned int m_video_frame_rate;
    AVPixelFormat m_video_pixel_format;

    bool m_audio_enabled;
    unsigned int m_audio_channels, m_audio_sample_rate;
    unsigned int m_audio_frame_size;
    AVSampleFormat m_audio_sample_format;
};



#endif // OUTPUTSETTINGS_H
