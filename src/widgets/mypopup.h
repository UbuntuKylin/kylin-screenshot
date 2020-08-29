/*
 * Copyright: 2020 KylinSoft Co., Ltd.
 * Authors:
 *   huanhuan zhang <zhanghuanhuan@kylinos.cn>
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 */
#ifndef MYPOPUP_H
#define MYPOPUP_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include "src/core/Global.h"
#include "src/common/WidgetWrapper.h"
#include  "screenrecorder.h"
class ScreenRecorder;
class mypopup : public QWidget
{
    Q_OBJECT

public:
    explicit mypopup(ScreenRecorder* ssr, QWidget *parent = nullptr);
    ~mypopup();

    void init();
    void InputInit();
    void OutputInit();
//    inline Ui::mypopup *getUI() {return ui;}

    void LoadScreenConfigurations();
    void LoadAVProfileSettings(QSettings *settings);
public:
    QString GetFileProtocol();
    QString GetContainerAVName();
    QString GetVideoCodecAVName();
    QString GetAudioCodecAVName();
    QString GetAudioKBitRateName();

private:
    ScreenRecorder  *ssr;
//    std::vector<ContainerData> m_containers, m_containers_av;
    std::vector<ScreenLabelWindow*> m_screen_labels;

    ssr::enum_container m_old_container;
    unsigned int m_old_container_av;

private:
    QVBoxLayout *mainLayout;
    QLabel *label,*label_2,*label_3,*label_4,*label_5,*label_6,*label_7;
    QComboBox *m_comboBox_audio_codec, *m_comboBox_audiorate, *m_comboBox_container,
              *m_comboBox_video_codec, *m_comboBox_videofrate, *m_comboBox_videores;
    QPushButton *m_pushButton_storelocation;
    QLabel *m_label_container_av_not_shown;
    QComboBox *m_combobox_container_av_not_shown;
    QLineEdit *m_lineedit_file_not_shown;
public slots:
    void OnUpdateVideoAreaFields();
    void OnUpdateSuffixAndContainerFields();
    void OnUpdateContainerFields();

private slots:
    void OnIdentifyScreens();
    void OnStopIdentifyScreens();
   // void OnBrowse();
public:
    ssr::enum_container GetContainer();
    unsigned int GetContainerAV();
    inline void SetContainer(ssr::enum_container container);
    inline void SetContainerAV(unsigned int container);
    inline QString GetFile() { return m_lineedit_file_not_shown->text(); }
    inline void SetFile(const QString& file) { m_lineedit_file_not_shown->setText(file); }
    ssr::enum_video_codec GetVideoCodec();
    void SetVideoCodec(ssr::enum_video_codec video_codec);
//    ssr::enum_video_codec GetVideoCodecAV();
    ssr::enum_audio_codec GetAudioCodec();
    void SetAudioCodec(ssr::enum_audio_codec audio_codec);

//    ssr::enum_audio_codec GetAudioCodecAV();

    int GetAudioKBitRate();
//    void SetVideoKBitRate(unsigned int kbit_rate);


};

#endif // MYPOPUP_H
