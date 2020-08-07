/*
 *
 * Copyright: 2020 KylinSoft Co., Ltd.
 * Authors:
 *   huanhuan zhang <zhanghuanhuan@cs2c.com.cn.com>
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
#include "mypopup.h"
//#include "Options_Widgets.h"
//#include "WidgetWrapper.h"

#include <QFileDialog>
#include <QGroupBox>

#include "src/common/utils.h"

#include "src/common/EnumStrings.h"
#include "src/common/CommandLineOptions.h"

mypopup::mypopup(ScreenRecorder *ssr, QWidget *parent) :
    QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint|Qt::WindowStaysOnTopHint);
    init();
    this->ssr = static_cast<ScreenRecorder*>(ssr);
    InputInit();
    OutputInit();
}
//初始化部件
void mypopup::init()
{
    mainLayout = new QVBoxLayout(this);
    label = new QLabel("label");
    mainLayout->addWidget(label);
    m_pushButton_storelocation = new QPushButton;
    mainLayout->addWidget(m_pushButton_storelocation);
    label_2 = new QLabel("label_2");
    mainLayout->addWidget(label_2);
    m_comboBox_audio_codec = new QComboBox;
    mainLayout->addWidget(m_comboBox_audio_codec);
    label_3 = new QLabel("label_3");
    mainLayout->addWidget(label_3);
    m_comboBox_audiorate = new QComboBox;
    mainLayout->addWidget(m_comboBox_audiorate);
    label_4 = new QLabel("label_4");
    mainLayout->addWidget(label_4);
    m_comboBox_container= new QComboBox;
    mainLayout->addWidget(m_comboBox_container);
    label_5 = new QLabel("label_5");
    mainLayout->addWidget(label_5);
    m_comboBox_video_codec = new QComboBox;
    mainLayout->addWidget(m_comboBox_video_codec);
    label_6 = new QLabel("label_6");
    mainLayout->addWidget(label_6);
    m_comboBox_videofrate = new QComboBox;
    mainLayout->addWidget(m_comboBox_videofrate);
    label_7 = new QLabel("label_7");
    mainLayout->addWidget(label_7);
    m_comboBox_videores = new QComboBox ;
    mainLayout->addWidget(m_comboBox_videores);
}
mypopup::~mypopup()
{
}

void mypopup::InputInit()
{

    connect(m_comboBox_videores, SIGNAL(activated(int)), this, SLOT(OnUpdateVideoAreaFields()));
    //connect(m_comboBox_videores, SIGNAL(popupShown()), this, SLOT(OnIdentifyScreens()));
    //connect(m_comboBox_videores, SIGNAL(popupHidden()), this, SLOT(OnStopIdentifyScreens()));
    LoadScreenConfigurations();
}

void mypopup::OutputInit()
{
    m_old_container = (ssr::enum_container)0;
    m_old_container_av = 0;
//    connect(m_pushButton_storelocation, SIGNAL(clicked()), this, SLOT(OnBrowse()));
    m_combobox_container_av_not_shown = new QComboBox;
    connect(m_comboBox_container, SIGNAL(activated(int)), this, SLOT(OnUpdateSuffixAndContainerFields()));
    //connect(m_pushButton_storelocation, SIGNAL(clicked()), this, SLOT(OnBrowse()));

    m_lineedit_file_not_shown = new QLineEdit;

    //video
    auto ssr_video_codecs = ssr->GetVideoCodecs();
    for (int i = 0; i < int(ssr::enum_video_codec::VIDEO_CODEC_COUNT); ++i) {
        m_comboBox_video_codec->addItem(ssr_video_codecs[i].name);
    }
    //audio
    auto ssr_audio_codecs = ssr->GetAudioCodecs();
    for (int i = 0; i < int(ssr::enum_audio_codec::AUDIO_CODEC_COUNT); ++i) {
        m_comboBox_audio_codec->addItem(ssr_audio_codecs[i].name);
    }
    //audio kb rate
    auto ssr_kbit_rates = ssr->GetAudioKBitRates();
    for (unsigned int i = 0; i < sizeof(ssr_kbit_rates)/sizeof(ssr_kbit_rates[0]); ++i) {
        m_comboBox_audiorate->addItem(ssr_kbit_rates[i].rate);
    }
    QSettings settings(CommandLineOptions::GetSettingsFile(), QSettings::IniFormat);
    LoadAVProfileSettings(&settings);
}

void mypopup::LoadScreenConfigurations()
{
    std::vector<QRect> screen_geometries = GetScreenGeometries();
    QRect combined_geometry = CombineScreenGeometries(screen_geometries);
    m_comboBox_videores->clear();
    m_comboBox_videores->addItem(tr("All screens: %1x%2", "This appears in the screen selection combobox")
                                .arg(combined_geometry.width()).arg(combined_geometry.height()));
    for(size_t i = 0; i < screen_geometries.size(); ++i) {
        QRect &geometry = screen_geometries[i];
        m_comboBox_videores->addItem(tr("Screen %1: %2x%3 at %4,%5", "This appears in the screen selection combobox")
                                    .arg(i + 1).arg(geometry.width()).arg(geometry.height()).arg(geometry.x()).arg(geometry.y()));
    }
    // update the video x/y/w/h in case the position or size of the selected screen changed
    //    OnUpdateVideoAreaFields();
}

void mypopup::LoadAVProfileSettings(QSettings *settings)
{
    // choose default container and codecs
    ssr::enum_container default_container = (ssr::enum_container) 0;
    auto ssr_containers = ssr->GetContainers();
    auto ssr_video_codecs = ssr->GetVideoCodecs();
    auto ssr_audio_codecs = ssr->GetAudioCodecs();
    for(unsigned int i = 0; i < int(ssr::enum_container::CONTAINER_OTHER); ++i) {
        if(AVFormatIsInstalled(ssr_containers[i].avname)) {
            default_container = (ssr::enum_container) i;
            break;
        }
    }
    ssr::enum_video_codec default_video_codec = (ssr::enum_video_codec) 0;
    for(unsigned int i = 0; i < int(ssr::enum_video_codec::VIDEO_CODEC_OTHER); ++i) {
        if(AVCodecIsInstalled(ssr_video_codecs[i].avname) && ssr_containers[int(default_container)].supported_video_codecs.count((ssr::enum_video_codec) i)) {
            default_video_codec = (ssr::enum_video_codec) i;
            break;
        }
    }
    ssr::enum_audio_codec default_audio_codec = (ssr::enum_audio_codec) 0;
    for(unsigned int i = 0; i < int(ssr::enum_video_codec::VIDEO_CODEC_OTHER); ++i) {
        if(AVCodecIsInstalled(ssr_audio_codecs[i].avname) && ssr_containers[int(default_container)].supported_audio_codecs.count((ssr::enum_audio_codec) i)) {
            default_audio_codec = (ssr::enum_audio_codec) i;
            break;
        }
    }
    // choose default file name
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QString dir_videos = QDesktopServices::storageLocation(QDesktopServices::MoviesLocation);
    QString dir_documents = QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation);
#else
    QString dir_videos = QStandardPaths::writableLocation(QStandardPaths::MoviesLocation);
    QString dir_documents = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
#endif
    QString dir_home = QDir::homePath();
    QString best_dir = (QDir(dir_videos).exists())? dir_videos : (QDir(dir_documents).exists())? dir_documents : dir_home;
    QString default_file = best_dir + "/simplescreenrecorder." + ssr_containers[int(default_container)].suffixes[0];

    // load settings
    SetFile(settings->value("output/file", default_file).toString());
//    SetSeparateFiles(settings->value("output/separate_files", false).toBool());
//    SetAddTimestamp(settings->value("output/add_timestamp", true).toBool());
    SetContainer(StringToEnum(settings->value("output/container", QString()).toString(), default_container));
//    mp->SetContainerAV(FindContainerAV(settings->value("output/container_av", QString()).toString()));

    SetVideoCodec(StringToEnum(settings->value("output/video_codec", QString()).toString(), default_video_codec));
//    SetVideoCodecAV(FindVideoCodecAV(settings->value("output/video_codec_av", QString()).toString()));
//    SetVideoKBitRate(settings->value("output/video_kbit_rate", 5000).toUInt());
//    SetH264CRF(settings->value("output/video_h264_crf", 23).toUInt());
//    SetH264Preset((enum_h264_preset) settings->value("output/video_h264_preset", H264_PRESET_SUPERFAST).toUInt());
//    SetVP8CPUUsed(settings->value("output/video_vp8_cpu_used", 5).toUInt());
//    SetVideoOptions(settings->value("output/video_options", "").toString());
//    SetVideoAllowFrameSkipping(settings->value("output/video_allow_frame_skipping", true).toBool());

    SetAudioCodec(StringToEnum(settings->value("output/audio_codec", QString()).toString(), default_audio_codec));
//    SetAudioCodecAV(FindAudioCodecAV(settings->value("output/audio_codec_av", QString()).toString()));
//    ssr->SetAudioKBitRate(settings->value("output/audio_kbit_rate", 128).toUInt());
    ssr->SetAudioOptions(settings->value("output/audio_options", "").toString());

    // update things
    OnUpdateContainerFields();
}


QString mypopup::GetFileProtocol()
{
    QRegExp protocol_regex("^([a-z0-9]+)://", Qt::CaseInsensitive, QRegExp::RegExp);
    if(protocol_regex.indexIn(GetFile()) < 0) {
        return QString();
    }
    return protocol_regex.cap(1);
}

QString mypopup::GetContainerAVName() {
    ssr::enum_container container = GetContainer();
    auto ssr_containers = ssr->GetContainers();
    auto ssr_containers_av = ssr->GetContainersAV();
    if(container != ssr::enum_container::CONTAINER_OTHER)
        return ssr_containers[int(container)].avname;
    else
        return ssr_containers_av[GetContainerAV()].avname;
}

QString mypopup::GetVideoCodecAVName() {
    ssr::enum_video_codec video_codec = GetVideoCodec();
    auto ssr_video_codecs = ssr->GetVideoCodecs();
    auto ssr_video_codecs_av = ssr->GetVideoCodecsAV();
    if(video_codec != ssr::enum_video_codec::VIDEO_CODEC_OTHER)
        return ssr_video_codecs[int(video_codec)].avname;
//    else
//        return ssr_video_codecs_av[GetVideoCodecAV()].avname;
    return NULL;
}

QString mypopup::GetAudioCodecAVName() {
    ssr::enum_audio_codec audio_codec = GetAudioCodec();
    auto ssr_audio_codecs = ssr->GetAudioCodecs();
    if(audio_codec != ssr::enum_audio_codec::AUDIO_CODEC_OTHER)
        return ssr_audio_codecs[int(audio_codec)].avname;
//    else
//        return m_audio_codecs_av[GetAudioCodecAV()].avname;
    return NULL;
}

QString mypopup::GetAudioKBitRateName()
{
    int audio_kbirt_rate = GetAudioKBitRate();
    auto audio_kbit_rates = ssr->GetAudioKBitRates();
    return audio_kbit_rates[audio_kbirt_rate].rate;
}


void mypopup::OnUpdateVideoAreaFields()
{
    switch(ssr->GetVideoArea()) {
    case ssr::enum_video_area::VIDEO_AREA_SCREEN: {
        int sc = m_comboBox_videores->currentIndex();
        std::vector<QRect> screen_geometries = GetScreenGeometries();
        QRect rect;
        if (sc > 0 && sc <= (int)screen_geometries.size()) {
            rect = screen_geometries[sc - 1];
        } else {
            rect = CombineScreenGeometries(screen_geometries);
        }
        ssr->SetVideoX(rect.left());
        ssr->SetVideoY(rect.top());
        ssr->SetVideoW(rect.width());
        ssr->SetVideoH(rect.height());
        break;
    }
    case ssr::enum_video_area::VIDEO_AREA_FIXED: {
        break;
    }
    case ssr::enum_video_area::VIDEO_AREA_CURSOR: {
        break;
    }
    default:
        break;
    }
}

static bool MatchSuffix(const QString& suffix, const QStringList& suffixes) {
    return ((suffix.isEmpty() && suffixes.isEmpty()) || suffixes.contains(suffix, Qt::CaseInsensitive));
}

void mypopup::OnUpdateSuffixAndContainerFields()
{
    // change file extension
    ssr::enum_container new_container = GetContainer();
    unsigned int new_container_av = GetContainerAV();
    auto ssr_containers_av = ssr->GetContainersAV();
    auto ssr_containers = ssr->GetContainers();

    if(GetFileProtocol().isNull()) {
        QStringList old_suffixes = (m_old_container == ssr::enum_container::CONTAINER_OTHER)?
                    ssr_containers_av[m_old_container_av].suffixes : ssr_containers[int(m_old_container)].suffixes;
        QStringList new_suffixes = (new_container == ssr::enum_container::CONTAINER_OTHER)?
                    ssr_containers_av[new_container_av].suffixes : ssr_containers[int(new_container)].suffixes;
        QString file = GetFile();
        if(!file.isEmpty()) {
            QFileInfo fi(file);
            if(MatchSuffix(fi.suffix(), old_suffixes) && !MatchSuffix(fi.suffix(), new_suffixes)) {
                if(new_suffixes.isEmpty())
                    m_lineedit_file_not_shown->setText(fi.path() + "/" + fi.completeBaseName());
                else
                    m_lineedit_file_not_shown->setText(fi.path() + "/" + fi.completeBaseName() + "." + new_suffixes[0]);
            }
        }
    }
    // update fields
    OnUpdateContainerFields();
}

void mypopup::OnUpdateContainerFields()
{
    ssr::enum_container container = GetContainer();
    unsigned int container_av = GetContainerAV();

    // show/hide fields
    //GroupVisible({m_label_container_av_not_shown, m_combobox_container_av_not_shown}, (container == ssr::enum_container::CONTAINER_OTHER));

    // show/hide warning
    //    m_label_container_warning->setVisible(GetContainerAVName() == "mp4");
    // mark uninstalled or unsupported codecs
    auto ssr_video_codecs = ssr->GetVideoCodecs();
    auto ssr_audio_codecs = ssr->GetAudioCodecs();
    auto ssr_containers = ssr->GetContainers();
    for(unsigned int i = 0; i < int(ssr::enum_video_codec::VIDEO_CODEC_OTHER); ++i) {
        QString name = ssr_video_codecs[i].name;
        if(!AVCodecIsInstalled(ssr_video_codecs[i].avname))
            name += " (" + tr("not installed") + ")";
        else if(container != ssr::enum_container::CONTAINER_OTHER && !ssr_containers[int(container)].supported_video_codecs.count((ssr::enum_video_codec) i))
            name += " (" + tr("not supported by container") + ")";
        m_comboBox_video_codec->setItemText(i, name);
    }
    for(unsigned int i = 0; i < int(ssr::enum_audio_codec::AUDIO_CODEC_OTHER); ++i) {
        QString name = ssr_audio_codecs[i].name;
        if(!AVCodecIsInstalled(ssr_audio_codecs[i].avname))
            name += " (" + tr("not installed") + ")";
        else if(container != ssr::enum_container::CONTAINER_OTHER && !ssr_containers[int(container)].supported_audio_codecs.count((ssr::enum_audio_codec) i))
            name += " (" + tr("not supported by container") + ")";
        m_comboBox_audio_codec->setItemText(i, name);
    }

    m_old_container = container;
    m_old_container_av = container_av;
}

void mypopup::OnIdentifyScreens()
{
    OnStopIdentifyScreens();
    std::vector<QRect> screen_geometries = GetScreenGeometries();
    for (size_t i = 0; i < screen_geometries.size(); ++i) {
        QRect &rect = screen_geometries[i];
        ScreenLabelWindow *label = new ScreenLabelWindow(this, tr("Screen %1", "This appears in the screen labels").arg(i + 1));
        label->move(rect.x(), rect.y());
        label->show();
         m_screen_labels.push_back(label);
    }
}


void mypopup::OnStopIdentifyScreens()
{
    for (unsigned int i = 0; i < m_screen_labels.size(); ++i) {
        delete m_screen_labels[i];
    }
    m_screen_labels.clear();
}
/*
void mypopup::OnBrowse()
{
    QString filters;
    auto ssr_containers = ssr->GetContainers();
    for (int i = 0; i < int(ssr::enum_container::CONTAINER_OTHER); ++i) {
        if (i != 0)
            filters += ssr_containers[i].filter;
    }

    auto ssr_containers_av = ssr->GetContainersAV();
    for (unsigned int i = 0; i < ssr_containers_av.size(); ++i) {
        if (!ssr_containers_av[i].filter.isEmpty())
            filters += ";;" + ssr_containers_av[i].filter;
    }

    ssr::enum_container container = GetContainer();
    unsigned int container_av = GetContainerAV();
    QString selected_filter = (container == ssr::enum_container::CONTAINER_OTHER) ?
                ssr->GetContainersAV()[container_av].filter : ssr->GetContainers()[int(container)].filter;
    QString selected_file = QFileDialog::getSaveFileName(this, tr("Save recording as"),
                                                         GetFile(), filters, &selected_filter, QFileDialog::DontConfirmOverwrite);
    if (selected_file.isNull()) {
        return;
    }

    m_lineedit_file_not_shown->clear();
    QFileInfo fi(selected_file);
    if(fi.suffix().isEmpty()) {
        QStringList suffixes = (container == ssr::enum_container::CONTAINER_OTHER)? ssr_containers_av[container_av].suffixes : ssr_containers[int(container)].suffixes;
        if(!suffixes.isEmpty())
            selected_file += "." + suffixes[0];
    } else {
        bool found = false;
        for(int i = 0; i < int(ssr::enum_container::CONTAINER_OTHER); ++i) {
            if(ssr_containers[i].suffixes.contains(fi.suffix(), Qt::CaseInsensitive)) {
                SetContainer((ssr::enum_container) i);
                found = true;
                break;
            }
        }
        if(!found) {
            for(unsigned int i = 0; i < ssr_containers_av.size(); ++i) {
                if(ssr_containers_av[i].suffixes.contains(fi.suffix(), Qt::CaseInsensitive)) {
                    SetContainer(ssr::enum_container::CONTAINER_OTHER);
                    SetContainerAV(i);
                    break;
                }
            }
        }
    }
    SetFile(selected_file);

    OnUpdateContainerFields();

}
*/
ssr::enum_container mypopup::GetContainer()
{
    return (ssr::enum_container) clamp(m_comboBox_container->currentIndex(), 0, int(ssr::enum_container::CONTAINER_COUNT) - 1);
}

#if 1
unsigned int mypopup::GetContainerAV()
{
    return clamp(m_combobox_container_av_not_shown->currentIndex(), 0, int(ssr->GetContainersAV().size()) - 1);
}
#endif

void mypopup::SetContainer(ssr::enum_container container)
{
    m_comboBox_container->setCurrentIndex(clamp((unsigned int) container, 0u, (unsigned int) ssr::enum_container::CONTAINER_COUNT - 1));
}

void mypopup::SetContainerAV(unsigned int container)
{
    m_combobox_container_av_not_shown->setCurrentIndex(clamp(container, 0u, (unsigned int) ssr->GetContainersAV().size() - 1));
}

ssr::enum_video_codec mypopup::GetVideoCodec()
{
     return (ssr::enum_video_codec) clamp(m_comboBox_video_codec->currentIndex(), 0, int(ssr::enum_video_codec::VIDEO_CODEC_COUNT) - 1);
}

void mypopup::SetVideoCodec(ssr::enum_video_codec video_codec)
{
    m_comboBox_video_codec->setCurrentIndex(clamp((unsigned int) video_codec,
                                                      0u, (unsigned int) ssr::enum_video_codec::VIDEO_CODEC_COUNT - 1));
}

ssr::enum_audio_codec mypopup::GetAudioCodec()
{
     return (ssr::enum_audio_codec) clamp(m_comboBox_audio_codec->currentIndex(), 0, int(ssr::enum_audio_codec::AUDIO_CODEC_COUNT) - 1);
}

//ssr::enum_video_codec mypopup::GetVideoCodecAV()
//{
//    return clamp(m_comboBox_video_codec_av->currentIndex(), 0, (int) m_video_codecs_av.size() - 1);
//}

void mypopup::SetAudioCodec(ssr::enum_audio_codec audio_codec)
{
    m_comboBox_audio_codec->setCurrentIndex(clamp((unsigned int) audio_codec, 0u, (unsigned int) ssr::enum_audio_codec::AUDIO_CODEC_COUNT - 1));
}

int mypopup::GetAudioKBitRate()
{
    auto ssr_audio_kbit_rates = ssr->GetAudioKBitRates();
    int len = sizeof(ssr_audio_kbit_rates)/sizeof(ssr_audio_kbit_rates[0]) - 1;
    return clamp(m_comboBox_audiorate->currentIndex(), 0,  len);
}

//void mypopup::SetVideoKBitRate(unsigned int kbit_rate)
//{
//    m_lineedit_video_kbit_rate->setCurrentIndex(clamp((unsigned int) audio_codec, 0u, (unsigned int) ssr::enum_audio_codec::AUDIO_CODEC_COUNT - 1));
//     m_lineedit_video_kbit_rate->setText(QString::number(kbit_rate));
//}

