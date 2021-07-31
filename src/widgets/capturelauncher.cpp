/* Copyright(c) 2017-2018 Alejandro Sirgo Rica & Contributors
* This file is part of Kylin-Screenshot.

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "capturelauncher.h"
#include "src/core/controller.h"
#include "src/widgets/imagelabel.h"
#include "src/widgets/notificationwidget.h"
#include "src/utils/screengrabber.h"
#include "src/utils/screenshotsaver.h"
#include "src/widgets/imagelabel.h"
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QMimeData>
#include <QDrag>
#include <QFormLayout>
#include <QStandardPaths>
// https://github.com/KDE/spectacle/blob/941c1a517be82bed25d1254ebd735c29b0d2951c/src/Gui/KSWidget.cpp
// https://github.com/KDE/spectacle/blob/941c1a517be82bed25d1254ebd735c29b0d2951c/src/Gui/KSMainWindow.cpp

CaptureLauncher::CaptureLauncher(QWidget *parent) :
    QWidget(parent),
    m_id(0)
{
    setWindowIcon(QIcon::fromTheme("kylin-screenshot"));
    setWindowTitle(tr("Screenshot"));
    setAttribute(Qt::WA_DeleteOnClose);
    connect(Controller::getInstance(), &Controller::captureTaken,
            this, &CaptureLauncher::captureTaken);
    connect(Controller::getInstance(), &Controller::captureFailed,
            this, &CaptureLauncher::captureFailed);

    m_imageLabel = new ImageLabel(this);
    bool ok;
    m_imageLabel->setScreenshot(ScreenGrabber().grabEntireDesktop(ok));
    if (!ok) {
    }
    m_imageLabel->setFixedSize(320, 180);
    m_imageLabel->move(24, 16);

    connect(m_imageLabel, &ImageLabel::dragInitiated,
            this, &CaptureLauncher::startDrag);

    font.setFamily("Noto Sans CJK SC Medium");
    font.setPixelSize(16);
    m_CaptureModeLabel = new QLabel(tr("Capture Mode"), this);
    // m_CaptureModeLabel->setFixedSize(64, 24);
    m_CaptureModeLabel->move(376, 16);

    font.setFamily("Noto Sans CJK SC Regular");
    font.setPixelSize(14);
    m_captureType = new QComboBox(this);
    m_captureType->setFixedSize(192, 36);
    m_captureType->move(414, 56);
    // TODO remember number
    m_captureType->insertItem(1, tr("Rectangular Region"), CaptureRequest::GRAPHICAL_MODE);
    m_captureType->insertItem(2, tr("Full Screen (All Monitors)"), CaptureRequest::FULLSCREEN_MODE);
    m_captureType->setFont(font);
    m_delaySpinBox = new QSpinBox(this);
    m_delaySpinBox->setFont(font);
    m_delaySpinBox->setSingleStep(1.0);
    m_delaySpinBox->setFixedSize(192, 36);
    m_delaySpinBox->move(414, 100);
    m_delaySpinBox->setMinimumWidth(160);
    connect(m_delaySpinBox,
            static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, [this](int val)
    {
        QString sufix = val == 1 ? tr(" second") : tr(" seconds");
        this->m_delaySpinBox->setSuffix(sufix);
    });

    m_launchButton = new QPushButton(tr("Take shot"), this);
    m_launchButton->setFont(font);
    m_launchButton->setFixedSize(96, 36);
    m_launchButton->move(510, 160);
    connect(m_launchButton, &QPushButton::pressed,
            this, &CaptureLauncher::startCapture);
    m_launchButton->setFocus();
// m_launchButton->setProperty(
    m_launchButton->setStyleSheet(
        ".QPushButton{background-color:#3790FA;border-radius:4px;color:rgb(255,255,255);}");
    QLabel *m_areaLabel = new  QLabel(this);
    m_areaLabel->setText(tr("Area:"));
    m_areaLabel->setFont(font);
    // m_areaLabel->setFixedSize(28, 24);
    m_areaLabel->move(376, 60);
    QLabel *m_delayLabel = new  QLabel(this);
    m_delayLabel->setText(tr("Delay:"));
    m_delayLabel->setFont(font);
    // m_delayLabel->setFixedSize(28, 24);
    m_delayLabel->move(376, 106);
    setFixedSize(630, 220);
}

// HACK: https://github.com/KDE/spectacle/blob/fa1e780b8bf3df3ac36c410b9ece4ace041f401b/src/Gui/KSMainWindow.cpp#L70
void CaptureLauncher::startCapture()
{
    hide();
    auto mode = static_cast<CaptureRequest::CaptureMode>(
        m_captureType->currentData().toInt());
    CaptureRequest req(mode, 600 + m_delaySpinBox->value() * 1000);
    m_id = req.id();
    Controller::getInstance()->requestCapture(req);
}

void CaptureLauncher::startDrag()
{
    QDrag *dragHandler = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setImageData(m_imageLabel->pixmap());
    dragHandler->setMimeData(mimeData);

    dragHandler->setPixmap(m_imageLabel->pixmap()
                           ->scaled(256, 256, Qt::KeepAspectRatioByExpanding,
                                    Qt::SmoothTransformation));
    dragHandler->exec();
}

void CaptureLauncher::captureTaken(uint id, QPixmap p)
{
    if (id == m_id) {
        m_id = 0;
        m_imageLabel->setScreenshot(p);
        auto mode = static_cast<CaptureRequest::CaptureMode>(
            m_captureType->currentData().toInt());
        QStringList a = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        if (mode == CaptureRequest::FULLSCREEN_MODE) {
            ScreenshotSaver().saveToFilesystem(p, a.at(0));
        }
        show();
    }
}

void CaptureLauncher::captureFailed(uint id)
{
    if (id == m_id) {
        m_id = 0;
        show();
    }
}
