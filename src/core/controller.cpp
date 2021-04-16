/* Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
*
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

#include "controller.h"
#include "src/widgets/capture/capturewidget.h"
#include "src/utils/confighandler.h"
#include "src/widgets/infowindow.h"
#include "src/config/configwindow.h"
#include "src/widgets/capture/capturebutton.h"
#include "src/utils/systemnotification.h"
#include "src/utils/screengrabber.h"
#include "my_qt.h"
#include "src/utils/systemnotification.h"
//#include "Logger.h"
#ifdef Q_OS_WIN
#include "src/core/globalshortcutfilter.h"
#endif

// Controller is the core component of kylin-screenshot, creates the trayIcon and
// launches the capture widget

Controller::Controller() : m_captureWindow(nullptr) {
    qApp->setQuitOnLastWindowClosed(false);

    // init tray icon
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    if (!ConfigHandler().disabledTrayIconValue()) {
        enableTrayIcon();
    }
#elif defined(Q_OS_WIN)
    enableTrayIcon();

    GlobalShortcutFilter *nativeFilter = new GlobalShortcutFilter(this);
    qApp->installNativeEventFilter(nativeFilter);
    connect(nativeFilter, &GlobalShortcutFilter::printPressed,
            this, [this](){
        this->requestCapture(CaptureRequest(CaptureRequest::GRAPHICAL_MODE));
    });
#endif

    QString StyleSheet = CaptureButton::globalStyleSheet();
    qApp->setStyleSheet(StyleSheet);
}

Controller *Controller::getInstance() {
    static Controller c;
    return &c;
}

void Controller::enableExports() {
    connect(this, &Controller::captureTaken,
            this, &Controller::handleCaptureTaken);
    connect(this, &Controller::captureFailed,
            this, &Controller::handleCaptureFailed);
}

void Controller::requestCapture(const CaptureRequest &request) {
    uint id = request.id();
    m_requestMap.insert(id, request);

    QGSettings *screen= new QGSettings("org.ukui.screenshot");
    QString screenshot = screen->get("screenshot").toString();
    switch (request.captureMode()) {
    case CaptureRequest::FULLSCREEN_MODE:
        if (screenshot.compare("true")==0){
            doLater(request.delay(), this, [this, id](){
                this->startFullscreenCapture(id);
            });
        }
        else
        {
            disableScreenCut();
        }
        break;
    case CaptureRequest::SCREEN_MODE: {
        if (screenshot.compare("true")==0){
            int &&number = request.data().toInt();
            doLater(request.delay(), this, [this, id, number](){
            this->startScreenGrab(id, number);
            });
        }
        else
        {
            disableScreenCut();
        }
        break;
    } case CaptureRequest::GRAPHICAL_MODE: {
        if (screenshot.compare("true")==0){
            QString &&path = request.path();
            doLater(request.delay(), this, [this, id, path](){
                this->startVisualCapture(id, path);
            });
        }
        else
        {
            disableScreenCut();
        }
        break;
    } default:
        emit captureFailed(id);
        break;
    }
}

// creation of a new capture in GUI mode
void Controller::startVisualCapture(const uint id, const QString &forcedSavePath) {
#ifdef SUPPORT_UKUI
    QGSettings *screen= new QGSettings("org.ukui.screenshot");
//    if (!screen) {
//        Logger::LogInfo("[Controller::startVisualCapture] " + Logger::tr("not support org.ukui.screenshot..."));
//        exit(1);
//    }
    QString screenshot = screen->get("screenshot").toString();
#endif
    if (!m_captureWindow) {
#ifdef SUPPORT_UKUI
        if (screenshot.compare("true")==0){
#endif
            QWidget *modalWidget = nullptr;
            do {
                modalWidget = qApp->activeModalWidget();
                if (modalWidget) {
                    modalWidget->close();
                    modalWidget->deleteLater();
                }
            } while (modalWidget);

            m_captureWindow = new CaptureWidget(id, forcedSavePath);
        //m_captureWindow = new CaptureWidget(id, forcedSavePath, false); // debug
            connect(m_captureWindow, &CaptureWidget::captureFailed,
                this, &Controller::captureFailed);
            connect(m_captureWindow, &CaptureWidget::captureTaken,
                this, &Controller::captureTaken);
#ifdef SUPPORT_UKUI
        }
        else
        {
            disconnect(m_captureWindow, &CaptureWidget::captureFailed,
                this, &Controller::captureFailed);
            disconnect(m_captureWindow, &CaptureWidget::captureTaken,
                this, &Controller::captureTaken);
            disableScreenCut();
        }
#endif
#ifdef Q_OS_WIN
        m_captureWindow->show();
#else
#ifdef SUPPORT_UKUI
        if (screenshot.compare("true")==0){
#endif
            m_captureWindow->showFullScreen();
#ifdef SUPPORT_UKUI
        }
#endif
        //m_captureWindow->show(); // Debug
#endif
    } else {
        emit captureFailed(id);
    }
}

void Controller::startScreenGrab(const uint id, const int screenNumber) {
    bool ok = true;
    int n = screenNumber;

    if (n < 0) {
        QPoint globalCursorPos = QCursor::pos();
        n = qApp->desktop()->screenNumber(globalCursorPos);
    }
    QPixmap p(ScreenGrabber().grabScreen(n, ok));
    if (ok) {
        emit captureTaken(id, p);
    } else {
        emit captureFailed(id);
    }
}

// creation of the configuration window
void Controller::openConfigWindow() {
    if (!m_configWindow) {
        m_configWindow = new ConfigWindow();
        m_configWindow->move(((qApp->desktop()->screenGeometry().width()-m_configWindow->width())/2),
                             ((qApp->desktop()->screenGeometry().height()-m_configWindow->height())/2));
        m_configWindow->show();
    }
}

// creation of the window of information
void Controller::openInfoWindow() {
    if (!m_infoWindow) {
        m_infoWindow = new InfoWindow();
    }
}

void Controller::openLauncherWindow() {
    QGSettings *screen= new QGSettings("org.ukui.screenshot");
    QString screenshot = screen->get("screenshot").toString();
    if (m_launcherWindow)
    {   
        delete m_launcherWindow;
    }
    if (!m_launcherWindow) {
        m_launcherWindow = new CaptureLauncher();
        m_launcherWindow->move(((qApp->desktop()->screenGeometry().width()-m_launcherWindow->width())/2),
            ((qApp->desktop()->screenGeometry().height()-m_launcherWindow->height())/2));
        if (screenshot.compare("true")==0){
            m_launcherWindow->show();
        }
	else
            disableScreenCut();
        }
}

void Controller::enableTrayIcon() {
    if (m_trayIcon) {
        return;
    }
    ConfigHandler().setDisabledTrayIcon(false);
    //Start the screenshot from the start menu and add a delay by zhanghanhuan
    //doLater(400, this, [this](){ this->startVisualCapture(); });
    QAction *captureAction = new QAction(tr("&Take Screenshot"), this);
    connect(captureAction, &QAction::triggered, this, [this](){
        // Wait 400 ms to hide the QMenu
        doLater(400, this, [this](){ this->startVisualCapture(); });
    });
    QAction *launcherAction = new QAction(tr("&Open Screenshot Option"), this);
    connect(launcherAction, &QAction::triggered, this,
            &Controller::openLauncherWindow);
    QAction *configAction = new QAction(tr("&Configuration"), this);
    connect(configAction, &QAction::triggered, this,
            &Controller::openConfigWindow);
    QAction *infoAction = new QAction(tr("&Information"), this);
    connect(infoAction, &QAction::triggered, this,
            &Controller::openInfoWindow);
    QAction *quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, this,
            &Controller::deletePidfile);
    connect(quitAction, &QAction::triggered, qApp,
            &QCoreApplication::quit);

    QMenu *trayIconMenu = new QMenu();
    trayIconMenu->addAction(captureAction);
    trayIconMenu->addAction(launcherAction);
    trayIconMenu->addSeparator();
    //trayIconMenu->addAction(configAction);
    trayIconMenu->addAction(infoAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    m_trayIcon = new QSystemTrayIcon();
    m_trayIcon->setToolTip(tr("Kylin-Screenshot"));
    m_trayIcon->setContextMenu(trayIconMenu);
    QIcon trayicon = QIcon::fromTheme("kylin-screenshot");
    //QIcon trayicon = QIcon("/usr/share/icons/ukui-icon-theme-default/128x128/apps/kylin-screenshot.png");
    m_trayIcon->setIcon(trayicon);

    auto trayIconActivated = [this](QSystemTrayIcon::ActivationReason r){
        if (r == QSystemTrayIcon::Trigger) {
            startVisualCapture();
        }
    };
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, trayIconActivated);
    m_trayIcon->show();
}

void Controller::disableTrayIcon() {
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
    if (m_trayIcon) {
        m_trayIcon->deleteLater();
    }
    ConfigHandler().setDisabledTrayIcon(true);
#endif
}

void Controller::sendTrayNotification(
        const QString &text,
        const QString &title,
        const int timeout)
{
    if (m_trayIcon) {
        m_trayIcon->showMessage(title, text, QSystemTrayIcon::Information, timeout);
    }
}

void Controller::updateConfigComponents() {
    if (m_configWindow) {
        m_configWindow->updateChildren();
    }
}

void Controller::deletePidfile()
{
    QFile file("/tmp/screenshot.pid");
    if (file.exists())
    {
        file.remove();
    }
}

void Controller::startFullscreenCapture(const uint id) {
    bool ok = true;
    QPixmap p(ScreenGrabber().grabEntireDesktop(ok));
    if (ok) {
        emit captureTaken(id, p);
    } else {
        emit captureFailed(id);
    }
}

void Controller::handleCaptureTaken(uint id, QPixmap p) {
    auto it = m_requestMap.find(id);
    if (it != m_requestMap.end()) {
        it.value().exportCapture(p);
        m_requestMap.erase(it);
    }
    if (ConfigHandler().closeAfterScreenshotValue()) {
        QApplication::quit();
    }
}

void Controller::handleCaptureFailed(uint id) {
    m_requestMap.remove(id);

    if (ConfigHandler().closeAfterScreenshotValue()) {
        QApplication::quit();
    }
}

void Controller::doLater(int msec, QObject *receiver, lambda func)  {
    QTimer *timer = new QTimer(receiver);
    QObject::connect(timer, &QTimer::timeout, receiver,
                     [timer, func](){ func(); timer->deleteLater(); });
    timer->setInterval(msec);
    timer->start();
}

void Controller::disableScreenCut()
{
    SystemNotification().sendMessage(tr("Unable to use kylin-screenshot"));
}
