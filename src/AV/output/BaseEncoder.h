#ifndef BASEENCODER_H
#define BASEENCODER_H

#include <deque>
#include <memory>

#include "src/AV/AVWrapper.h"
#include "src/common/MutexDataPair.h"
#include "src/AV/output/Muxer.h"

int ParseCodecOptionInt(const QString& key, const QString& value, int min, int max, int multiply = 1);
double ParseCodecOptionDouble(const QString& key, const QString& value, double min, double max, double multiply = 1.0);

class Muxer;

class BaseEncoder
{

private:
    struct SharedData {
        std::deque<std::unique_ptr<AVFrameWrapper>> m_frame_queue;
        uint64_t m_total_frames, m_total_packets;
        double m_stats_actual_frame_rate;
        int64_t m_stats_previous_pts;
        int64_t m_stats_previous_frames;
    };
    typedef MutexDataPair<SharedData>::Lock SharedLock;

private:
    Muxer *m_muxer;
    AVStream *m_stream;
    AVCodecContext *m_codec_context;
    bool m_codec_opened;

    std::thread m_thread;
    MutexDataPair<SharedData> m_shared_data;
    std::atomic<bool> m_should_stop, m_should_finish, m_is_done, m_error_occurred;

protected:
    BaseEncoder(Muxer* muxer, AVStream* stream, AVCodecContext* codec_context, AVCodec* codec, AVDictionary** options);

public:
    virtual ~BaseEncoder();

protected:
    void StartThread();
    void StopThread();

public:
    double GetActualFrameRate();

    uint64_t GetTotalFrames();

    unsigned int GetFrameLatency();

    unsigned int GetQueuedFrameCount();

    unsigned int GetQueuedPacketCount();

public:
    void AddFrame(std::unique_ptr<AVFrameWrapper> frame);

    void Finish();

    void Stop();

    inline bool IsDone() {return m_is_done; }

    inline bool HasErrorOccurred() {return m_error_occurred; }

    inline Muxer* GetMuxer() {return m_muxer; }

    inline AVStream* GetStream() {return m_stream; }

    inline AVCodecContext* GetCodecContext() {return m_codec_context; }

protected:
    virtual bool EncodeFrame(AVFrameWrapper* frame) = 0;

    void IncrementPacketCounter();

private:
    void Init(AVCodec* codec, AVDictionary** options);
    void Free();

    void EncoderThread();










};

#endif // BASEENCODER_H
