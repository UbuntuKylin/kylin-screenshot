#ifndef ENUM_H
#define ENUM_H

namespace ssr {

enum class enum_video_area {
    VIDEO_AREA_SCREEN,
    VIDEO_AREA_FIXED,
    VIDEO_AREA_CURSOR,
    VIDEO_AREA_COUNT

};

//output
enum class enum_container {
    CONTAINER_MKV,
    CONTAINER_MP4,
    CONTAINER_WEBM,
    CONTAINER_OGG,
    CONTAINER_OTHER,
    CONTAINER_COUNT // must be last

};
enum class enum_video_codec {
    VIDEO_CODEC_H264,
    VIDEO_CODEC_VP8,
    VIDEO_CODEC_THEORA,
    VIDEO_CODEC_OTHER,
    VIDEO_CODEC_COUNT // must be last
};
enum class enum_audio_codec {
    AUDIO_CODEC_VORBIS,
    AUDIO_CODEC_MP3,
    AUDIO_CODEC_AAC,
    AUDIO_CODEC_UNCOMPRESSED,
    AUDIO_CODEC_OTHER,
    AUDIO_CODEC_COUNT // must be last
};

enum enum_audio_backend {
#if SSR_USE_ALSA
    AUDIO_BACKEND_ALSA,
#endif
#if SSR_USE_PULSEAUDIO
    AUDIO_BACKEND_PULSEAUDIO,
#endif
#if SSR_USE_JACK
    AUDIO_BACKEND_JACK,
#endif
    AUDIO_BACKEND_COUNT // must be last
};

}
#endif // ENUM_H
