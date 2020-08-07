// Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
//              2020, KylinSoft Co., Ltd.
// This file is part of Kylin-Screenshot.
//
//     Kylin-Screenshot is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     Kylin-Screenshot is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with Kylin-Screenshot.  If not, see <http://www.gnu.org/licenses/>.
#ifndef SCREENRECORDER_H
#define SCREENRECORDER_H

#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QWidget>
#include <QCheckBox>

#include "mypopup.h"
#include "src/AV/output/OutputSettings.h"
#include <memory>
#include "src/AV/output/OutputManager.h"
#include "src/core/Global.h"
#include "src/common/WidgetWrapper.h"
#include "src/AV/input/X11Input.h"
#if SSR_USE_PULSEAUDIO
#include "PulseAudioInput.h"
#endif
class mypopup;
class X11Input;

class input_widgets : public QWidget{};

class output_widgets : public QWidget{};

class record_widgets : public QWidget{};

class ScreenRecorder : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenRecorder(QWidget *parent = nullptr);
    //~ScreenRecorder();
    void Init();

    void Input_init();
    void Output_init();
    void Prepare_Record();

    void LoadSettings();

    void LoadInputSettings(QSettings* settings);
    void LoadOutputSettings(QSettings* settings);

    //record
    void StartInput();
    void StopInput();

    void StartOutput();
    void StopOutput(bool final);
    void StopPage(bool save);
    //void UpdateSysTray();
    void FinishOutput();
    void UpdateRecordButton();
    bool IsBusy();
    //QString GetContainerAVName();

public:
    enum enum_container {
        CONTAINER_MKV,
        CONTAINER_MP4,
        CONTAINER_WEBM,
        CONTAINER_OGG,
        CONTAINER_OTHER,
        CONTAINER_COUNT // must be last
    };
    enum enum_video_codec {
        VIDEO_CODEC_H264,
        VIDEO_CODEC_VP8,
        VIDEO_CODEC_THEORA,
        VIDEO_CODEC_OTHER,
        VIDEO_CODEC_COUNT // must be last
    };
    enum enum_audio_codec {
        AUDIO_CODEC_VORBIS,
        AUDIO_CODEC_MP3,
        AUDIO_CODEC_AAC,
        AUDIO_CODEC_UNCOMPRESSED,
        AUDIO_CODEC_OTHER,
        AUDIO_CODEC_COUNT // must be last
    };
    enum enum_h264_preset {
        H264_PRESET_ULTRAFAST,
        H264_PRESET_SUPERFAST,
        H264_PRESET_VERYFAST,
        H264_PRESET_FASTER,
        H264_PRESET_FAST,
        H264_PRESET_MEDIUM,
        H264_PRESET_SLOW,
        H264_PRESET_SLOWER,
        H264_PRESET_VERYSLOW,
        H264_PRESET_PLACEBO,
        H264_PRESET_COUNT // must be last
    };

private slots:
    void on_m_toolButton_options_clicked();
    void on_m_pushbutton_video_select_rectangle_clicked();
    void on_m_pushbutton_video_select_window_clicked();
    void OnUpdateInformation();
    void OnRecordStart();
    void OnRecordPause();
    void OnRecordStartPause();
    void OnBrowse();
private:
    void LoadInputProfileSettings(QSettings* settings);
    void LoadOutputProfileSettings(QSettings* settings);

public:
#if SSR_USE_PULSEAUDIO
    QString GetPulseAudioSourceName();
#endif

private:
    void StartGrabbing();
    void StopGrabbing();
    void UpdateRubberBand();
    void SetVideoAreaFromRubberBand();
    void LoadAVProfileSettings(QSettings *settings);
#if SSR_USE_PULSEAUDIO
    void LoadPulseAudioSources();
#endif
    //record
    void UpdateInput();
    QString GetFileProtocol();

public slots:
    void OnUpdateRecordingFrame();
    void OnUpdateVideoAreaFields();
    void OnUpdateSuffixAndContainerFields();
    void OnUpdateContainerFields();
protected:
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

private:
    mypopup *mp;
    QSettings settings;
    bool options_show;
    bool m_wait_saving;
    bool m_video_area_follow_fullscreen;
    unsigned int m_video_x, m_video_y, m_video_in_width, m_video_in_height;
    unsigned int m_video_frame_rate;

    bool m_video_record_cursor;
    bool m_audio_enabled;
    unsigned int m_audio_channels, m_audio_sample_rate;

    OutputSettings m_output_settings;
    std::unique_ptr<OutputManager> m_output_manager;
    QButtonGroup *m_buttongroup_video_area;
    QLabel *m_label_video_x, *m_label_video_y, *m_label_video_w, *m_label_video_h;
    QSpinBox *m_spinbox_video_x, *m_spinbox_video_y, *m_spinbox_video_w, *m_spinbox_video_h;
    QPushButton *m_pushbutton_start, *m_pushbutton_stop, *m_pushbutton_options;
    QPushButton *m_pushbutton_video_select_rectangle, *m_pushbutton_video_select_window;
    bool m_grabbing, m_selecting_window;
    std::unique_ptr<RecordingFrameWindow>  m_rubber_band, m_recording_frame;
    QRect m_rubber_band_rect, m_select_window_outer_rect, m_select_window_inner_rect;
    QLabel *label_frame_rate;
    //音频比特率
    QLabel *m_label_video_kbit_rate;
    QLineEdit *m_lineedit_video_kbit_rate;
    //是品真理
    QSpinBox *m_spinbox_video_frame_rate;
    //录制光标
    QCheckBox *m_checkbox_record_cursor;
    //录制声音
    QCheckBox *m_checkbox_audio_enable;
    QComboBox *m_combobox_container;
    //另存为
    QLineEdit *m_lineedit_file;
    QComboBox *m_combobox_container_av;
    QPushButton *m_pushButton_storelocation;
    QComboBox *m_combobox_video_codec;
#if SSR_USE_PULSEAUDIO
    bool m_pulseaudio_available;
    std::vector<PulseAudioInput::Source> m_pulseaudio_sources;
#endif
    QLineEdit *m_lineedit_file_not_shown;
    ssr::enum_audio_backend m_audio_backend;
    std::unique_ptr<X11Input> m_x11_input;
#if SSR_USE_PULSEAUDIO
    QString m_pulseaudio_source;
    std::unique_ptr<PulseAudioInput> m_pulseaudio_input;
#endif
//    VideoPreviewer *m_video_previewer;
private:
    //output
    struct ContainerData {
        QString name, avname;
        QStringList suffixes;
        QString filter;
        std::set<ssr::enum_video_codec> supported_video_codecs;
        std::set<ssr::enum_audio_codec> supported_audio_codecs;
        inline bool operator<(const ContainerData& other) const { return (avname < other.avname); }
    };
    struct VideoCodecData {
        QString name, avname;
        inline bool operator<(const VideoCodecData& other) const { return (avname < other.avname); }
    };
    struct AudioCodecData {
        QString name, avname;
        inline bool operator<(const AudioCodecData& other) const { return (avname < other.avname); }
    };
    struct AudioKBitRate {
        QString rate;
        inline bool operator<(const AudioKBitRate& other) const { return (rate < other.rate); }
    };

    std::vector<ContainerData> m_containers, m_containers_av;
    std::vector<VideoCodecData> m_video_codecs, m_video_codecs_av;
    std::vector<AudioCodecData> m_audio_codecs, m_audio_codecs_av;
    std::vector<AudioKBitRate> m_audio_kbit_rates;

private:
    //record
    bool m_output_started , m_page_started;
    ssr::enum_video_area m_video_area;

    QString m_file_base;
    QString m_file_protocol;

    bool m_add_timestamp;

    bool m_input_started;
    bool m_recorded_something;

    QTimer *m_timer_schedule, *m_timer_update_info;

    enum_container m_old_container;
    unsigned int m_old_container_av;

public:
    inline ssr::enum_video_area GetVideoArea() { return (ssr::enum_video_area) clamp(m_buttongroup_video_area->checkedId(), 0, int(ssr::enum_video_area::VIDEO_AREA_COUNT) - 1); }

    void SetVideoArea(ssr::enum_video_area area);
//    void SetVideoAreaScreen(unsigned int screen);
    QString GetContainerAVName();

    void SetVideoX(unsigned int x);
    void SetVideoY(unsigned int y);
    void SetVideoW(unsigned int w);
    void SetVideoH(unsigned int h);
    unsigned int GetVideoX();
    unsigned int GetVideoY();
    unsigned int GetVideoW();
    unsigned int GetVideoH();

    inline std::vector<ContainerData> GetContainers() { return m_containers; }
    inline std::vector<ContainerData> GetContainersAV() { return m_containers_av; }
    inline std::vector<VideoCodecData> GetVideoCodecs() { return m_video_codecs; }
    inline std::vector<VideoCodecData> GetVideoCodecsAV() { return m_video_codecs_av; }
    inline std::vector<AudioCodecData> GetAudioCodecs() { return m_audio_codecs; }
    inline std::vector<AudioCodecData> GetAudioCodecsAV() { return m_audio_codecs_av; }
    inline std::vector<AudioKBitRate> GetAudioKBitRates() { return m_audio_kbit_rates; }
    inline bool GetAudioEnabled() { return m_checkbox_audio_enable->isChecked(); }
    inline bool GetVideoRecordCursor() { return m_checkbox_record_cursor->isChecked(); }
    //获取音频比特率
    inline unsigned int GetVideoKBitRate() { return m_lineedit_video_kbit_rate->text().toUInt(); }

    inline enum_container GetContainer()
    {
        return (enum_container) clamp(m_combobox_container->currentIndex(), 0, CONTAINER_COUNT - 1);
    }

    inline QString GetFile() { return m_lineedit_file->text(); }
    inline unsigned int GetContainerAV() { return clamp(m_combobox_container_av->currentIndex(), 0, (int) m_containers_av.size() - 1); }
    inline unsigned int GetVideoFrameRate() { return m_spinbox_video_frame_rate->value(); }
    inline void SetVideoFrameRate(unsigned int frame_rate) { m_spinbox_video_frame_rate->setValue(frame_rate); }

    inline void SetContainer(enum_container container);
    inline void SetContainerAV(unsigned int container);

#if SSR_USE_PULSEAUDIO
    inline unsigned int GetPulseAudioSource() { return clamp(m_combobox_pulseaudio_source->currentIndex(), 0, (int) m_pulseaudio_sources.size() - 1); }
#endif
    inline void SetFile(const QString& file) { m_lineedit_file->setText(file); }

private:
    QLineEdit *m_lineedit_audio_options_not_shown;
#if SSR_USE_PULSEAUDIO
    QComboBox *m_combobox_pulseaudio_source;
#endif
public:
    inline void SetAudioOptions(const QString& options) { m_lineedit_audio_options_not_shown->setText(options); }
    inline QString GetAudioOptions()
    {
        return m_lineedit_audio_options_not_shown->text();
    }
};

#endif // SCREENRECORDER_H
