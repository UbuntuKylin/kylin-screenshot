#ifndef X11INPUT_H
#define X11INPUT_H

#include "src/core/Global.h"

#include "src/AV/SourceSink.h"
#include "src/common/MutexDataPair.h"
class X11Input : public VideoSource
{
private:
    struct Rect {
        unsigned int m_x1, m_y1, m_x2, m_y2;
        inline Rect() {}
        inline Rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) : m_x1(x1), m_y1(y1), m_x2(x2), m_y2(y2) {}
    };
    struct SharedData {
        unsigned int m_current_width, m_current_height;
    };
    typedef MutexDataPair<SharedData>::Lock SharedLock;

private:
    unsigned int m_x, m_y, m_width, m_height;
    bool m_record_cursor, m_follow_cursor, m_follow_fullscreen;

    std::atomic<uint32_t> m_frame_counter;
    int64_t m_fps_last_timestamp;
    uint32_t m_fps_last_counter;
    double m_fps_current;

    Display *m_x11_display;
    int m_x11_screen;
    Window m_x11_root;
    Visual *m_x11_visual;
    int m_x11_depth;
    bool m_x11_use_shm;
    XImage *m_x11_image;
    XShmSegmentInfo m_x11_shm_info;
    bool m_x11_shm_server_attached;

    Rect m_screen_bbox;
    std::vector<Rect> m_screen_rects;
    std::vector<Rect> m_screen_dead_space;

    std::thread m_thread;
    MutexDataPair<SharedData> m_shared_data;
    std::atomic<bool> m_should_stop, m_error_occurred;


public:
    X11Input(unsigned int x, unsigned int y, unsigned int width, unsigned int height, bool record_cursor, bool follow_cursor, bool follow_fullscreen);
    ~X11Input();

    // Reads the current size of the stream.
    // This function is thread-safe.
    void GetCurrentSize(unsigned int* width, unsigned int* height);

    // Returns the total number of captured frames.
    // This function is thread-safe.
    double GetFPS();

private:
    void Init();
    void Free();

private:
    void AllocateImage(unsigned int width, unsigned int height);
    void FreeImage();
    void UpdateScreenConfiguration();

private:
    void InputThread();
};

#endif // X11INPUT_H
