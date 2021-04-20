/* Copyright(c) 2017-2018 Alejandro Sirgo Rica & Contributors
*               2020 KylinSoft Co., Ltd.
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

#include "src/core/controller.h"
#include "singleapplication.h"
#include "src/utils/filenamehandler.h"
#include "src/utils/confighandler.h"
#include "src/cli/commandlineparser.h"
#include "src/utils/systemnotification.h"
#include "src/utils/pathinfo.h"
#include "src/core/capturerequest.h"
#include "my_qt.h"
#include <sys/types.h>
#include <signal.h>
#include "src/common/CommandLineOptions.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
#include "src/core/flameshotdbusadapter.h"
#include "src/utils/dbusutils.h"
#include <QDBusMessage>
#include <QDBusConnection>
#endif

#include "Logger.h"
#include "my_x.h"

int getScreenWidth()
{
    Display *disp = XOpenDisplay(NULL);
    Screen *scrn = DefaultScreenOfDisplay(disp);
    if (NULL == scrn) {
        return 0;
    }
    int width = scrn->width;

    if (NULL != disp) {
        XCloseDisplay(disp);
    }
    return width;
}

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // �~J| �~T~A
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug    ");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning    ");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical    ");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal    ");
        break;
    case QtInfoMsg:
        strMsg = QString("Info    ");
        break;
    }

    // 设置�~S�~G�信�~A��| ��~O
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    //QString strMessage = QString("[Message]: %1 [File]: %2  [Line]: %3  [Function]: %4  [DateTime]: %5")
    //        .arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(strDateTime);
    QString strMessage = QString("[DateTime]: %1  [Message]: %2  [Line]: %3  [Function]: %4")
            .arg(strDateTime).arg(localMsg.constData()).arg(context.line).arg(context.function);

    // �~S�~G�信�~A��~G��~V~G件中�~H读�~F~Y�~@~A追�~J| 形�~O�~I
    //QString url_filepath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation)+"/.config/kylin-screenshot_output.log";
    QString url_filepath =  "/tmp/kylin-screenshot_output.log";
    QFile file(url_filepath);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMsg << strMessage << "\r\n";
    file.flush();
    file.close();

    // 解�~T~A
    mutex.unlock();
}


int main(int argc, char *argv[]) {
    //�~G��~Z�~I�~W��~W
    qInstallMessageHandler(myMessageOutput);
    // required for the button serialization
    // TODO: change to QVector in v1.0
    if (getScreenWidth() > 2560) {
            #if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
                    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
                    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
            #endif
    }
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
    qApp->setApplicationVersion(static_cast<QString>(APP_VERSION));
    if (qgetenv("QT_QPA_PLATFORMTHEME") != "ukui")
            qputenv("QT_QPA_PLATFORMTHEME", "ukui");
    // no arguments, just launch kylin-screenshot
    if (argc == 1) {
        SingleApplication app(argc, argv);
        CommandLineOptions command_line_options;
        Logger logger;
        QTranslator translator, qtTranslator;
        QStringList trPaths = PathInfo::translationsPaths();

        for (const QString &path: trPaths) {
            bool match = translator.load(QLocale(),
                                    QStringLiteral("Internationalization"), QStringLiteral("_"),
                                    path);
            if (match) {
                break;
            }
        }

        qtTranslator.load(QLocale::system(), "qt", "_",
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));

        app.installTranslator(&translator);
        app.installTranslator(&qtTranslator);
        app.setAttribute(Qt::AA_DontCreateNativeWidgetSiblings, true);
        app.setApplicationName(QObject::tr("kylin-screenshot"));
        app.setOrganizationName(QStringLiteral("Dharkael"));
        app.setProperty("useFileDialog",false);
        auto c = Controller::getInstance();
#if defined(Q_OS_LINUX) || defined(Q_OS_UNIX)
        new FlameshotDBusAdapter(c);
        QDBusConnection dbus = QDBusConnection::sessionBus();
        if (!dbus.isConnected()) {
            SystemNotification().sendMessage(
                        QObject::tr("Unable to connect via DBus"));
        }
        dbus.registerObject(QStringLiteral("/"), c);
        dbus.registerService(QStringLiteral("org.dharkael.kylinscreenshot"));
#endif
        // Exporting captures must be connected after the dbus interface
        // or the dbus signal gets blocked until we end the exports.
        c->enableExports();
        return app.exec();
    }

#ifndef Q_OS_WIN
    /*--------------|
     * CLI parsing  |
     * ------------*/
    QCoreApplication app(argc, argv);
    app.setApplicationName(QObject::tr("Kylin-Screenshot"));
    app.setOrganizationName(QStringLiteral("Dharkael"));
    app.setApplicationVersion(qApp->applicationVersion());
    CommandLineParser parser;
    // Add description
    parser.setDescription(
                QStringLiteral("Powerful yet simple to use screenshot software."));
    parser.setGeneralErrorMessage(QStringLiteral("See 'kylin-screenshot --help'."));
    // Arguments
    CommandArgument fullArgument(QStringLiteral("full"), QStringLiteral("Capture the entire desktop."));
    CommandArgument launcherArgument(QStringLiteral("launcher"), QStringLiteral("Open the capture launcher."));
    CommandArgument guiArgument(QStringLiteral("gui"), QStringLiteral("Start a manual capture in GUI mode."));
    CommandArgument configArgument(QStringLiteral("config"), QStringLiteral("Configure Kylin-Screenshot."));
    CommandArgument screenArgument(QStringLiteral("screen"), QStringLiteral("Capture a single screen."));

    // Options
    CommandOption pathOption(
                {"p", "path"},
                QStringLiteral("Path where the capture will be saved"),
                QStringLiteral("path"));
    CommandOption clipboardOption(
                {"c", "clipboard"},
                QStringLiteral("Save the capture to the clipboard"));
    CommandOption delayOption(
                {"d", "delay"},
                QStringLiteral("Delay time in milliseconds"),
                QStringLiteral("milliseconds"));
    CommandOption filenameOption(
                {"f", "filename"},
                QStringLiteral("Set the filename pattern"),
                QStringLiteral("pattern"));
    CommandOption trayOption(
                {"t", "trayicon"},
                QStringLiteral("Enable or disable the trayicon"),
                QStringLiteral("bool"));
    CommandOption autostartOption(
                {"a", "autostart"},
                QStringLiteral("Enable or disable run at startup"),
                QStringLiteral("bool"));
    CommandOption showHelpOption(
                {"s", "showhelp"},
                QStringLiteral("Show the help message in the capture mode"),
                QStringLiteral("bool"));
    CommandOption mainColorOption(
                {"m", "maincolor"},
                QStringLiteral("Define the main UI color"),
                QStringLiteral("color-code"));
    CommandOption contrastColorOption(
                {"k", "contrastcolor"},
                QStringLiteral("Define the contrast UI color"),
                QStringLiteral("color-code"));
    CommandOption rawImageOption(
                {"r", "raw"},
                QStringLiteral("Print raw PNG capture"));
    CommandOption screenNumberOption(
                {"n", "number"},
                QStringLiteral("Define the screen to capture,\ndefault: screen containing the cursor"),
                QStringLiteral("Screen number"), QStringLiteral("-1"));

    // Add checkers
    auto colorChecker = [](const QString &colorCode) -> bool {
        QColor parsedColor(colorCode);
        return parsedColor.isValid() && parsedColor.alphaF() == 1.0;
    };
    QString colorErr = "Invalid color, "
                       "this flag supports the following formats:\n"
                       "- #RGB (each of R, G, and B is a single hex digit)\n"
                       "- #RRGGBB\n- #RRRGGGBBB\n"
                       "- #RRRRGGGGBBBB\n"
                       "- Named colors like 'blue' or 'red'\n"
                       "You may need to escape the '#' sign as in '\\#FFF'";

    const QString delayErr = QStringLiteral("Invalid delay, it must be higher than 0");
    const QString numberErr = QStringLiteral("Invalid screen number, it must be non negative");
    auto numericChecker = [](const QString &delayValue) -> bool {
        int value = delayValue.toInt();
        return value >= 0;
    };

    const QString pathErr = QStringLiteral("Invalid path, it must be a real path in the system");
    auto pathChecker = [pathErr](const QString &pathValue) -> bool {
        bool res = QDir(pathValue).exists();
        if (!res) {
            SystemNotification().sendMessage(QObject::tr(pathErr.toLatin1().data()));
        }
        return res;
    };

    const QString booleanErr = QStringLiteral("Invalid value, it must be defined as 'true' or 'false'");
    auto booleanChecker = [](const QString &value) -> bool {
        return value == QLatin1String("true") || value == QLatin1String("false");
    };

    contrastColorOption.addChecker(colorChecker, colorErr);
    mainColorOption.addChecker(colorChecker, colorErr);
    delayOption.addChecker(numericChecker, delayErr);
    pathOption.addChecker(pathChecker, pathErr);
    trayOption.addChecker(booleanChecker, booleanErr);
    autostartOption.addChecker(booleanChecker, booleanErr);
    showHelpOption.addChecker(booleanChecker, booleanErr);
    screenNumberOption.addChecker(numericChecker, numberErr);

    // Relationships
    parser.AddArgument(guiArgument);
    parser.AddArgument(screenArgument);
    parser.AddArgument(fullArgument);
    parser.AddArgument(launcherArgument);
    parser.AddArgument(configArgument);
    auto helpOption = parser.addHelpOption();
    auto versionOption = parser.addVersionOption();
    parser.AddOptions({ pathOption, delayOption, rawImageOption }, guiArgument);
    parser.AddOptions({ screenNumberOption, clipboardOption, pathOption,
                        delayOption, rawImageOption },
                      screenArgument);
    parser.AddOptions({ pathOption, clipboardOption, delayOption, rawImageOption },
                      fullArgument);
    parser.AddOptions({ autostartOption, filenameOption, trayOption,
                        showHelpOption, mainColorOption, contrastColorOption },
                      configArgument);
    // Parse
    if (!parser.parse(app.arguments())) {
        goto finish;
    }

    // PROCESS DATA
    //--------------
    if (parser.isSet(helpOption) || parser.isSet(versionOption)) {
    }
    else if (parser.isSet(launcherArgument)) { // LAUNCHER
        QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.dharkael.kylinscreenshot"),
                                           QStringLiteral("/"), QLatin1String(""), QStringLiteral("openLauncher"));
        QDBusConnection sessionBus = QDBusConnection::sessionBus();
        if (!sessionBus.isConnected()) {
            SystemNotification().sendMessage(
                        QObject::tr("Unable to connect via DBus"));
        }
        sessionBus.call(m);
    }
    else if (parser.isSet(guiArgument)) { // GUI
        QString pathValue = parser.value(pathOption);
        int delay = parser.value(delayOption).toInt();
        bool isRaw = parser.isSet(rawImageOption);
        DBusUtils dbusUtils;
        CaptureRequest req(CaptureRequest::GRAPHICAL_MODE, delay, pathValue);
        uint id = req.id();

        // Send message
        QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.dharkael.kylinscreenshot"),
                                           QStringLiteral("/"), QLatin1String(""), QStringLiteral("graphicCapture"));
        m << pathValue << delay << id;
        QDBusConnection sessionBus = QDBusConnection::sessionBus();
        dbusUtils.checkDBusConnection(sessionBus);
        sessionBus.call(m);

        if (isRaw) {
            dbusUtils.connectPrintCapture(sessionBus, id);
            QTimer t;
            t.setInterval(delay + 1000 * 60 * 15); // 15 minutes timeout
            QObject::connect(&t, &QTimer::timeout, qApp,
                             &QCoreApplication::quit);
            t.start();
            // wait
            return app.exec();
        }
    }
    else if (parser.isSet(fullArgument)) { // FULL
        QString pathValue = parser.value(pathOption);
        int delay = parser.value(delayOption).toInt();
        bool toClipboard = parser.isSet(clipboardOption);
        bool isRaw = parser.isSet(rawImageOption);
        // Not a valid command
        if(pathValue.isEmpty())
        {
            QStringList a = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
            pathValue= a.at(0);
        }
        if (!isRaw && !toClipboard && pathValue.isEmpty()) {
            QTextStream out(stdout);
            out << "Invalid format, set where to save the content with one of "
                << "the following flags:\n "
                << pathOption.dashedNames().join(QStringLiteral(", ")) << "\n "
                << rawImageOption.dashedNames().join(QStringLiteral(", ")) << "\n "
                << clipboardOption.dashedNames().join(QStringLiteral(", ")) << "\n\n";
            parser.parse(QStringList() << argv[0] << QStringLiteral("full") << QStringLiteral("-h"));
            goto finish;
        }

        CaptureRequest req(CaptureRequest::FULLSCREEN_MODE, delay, pathValue);
        if (toClipboard) {
            req.addTask(CaptureRequest::CLIPBOARD_SAVE_TASK);
        }
        if (!pathValue.isEmpty()) {
            req.addTask(CaptureRequest::FILESYSTEM_SAVE_TASK);
        }
        uint id = req.id();
        DBusUtils dbusUtils;

        // Send message
        QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.dharkael.kylinscreenshot"),
                                               QStringLiteral("/"), QLatin1String(""), QStringLiteral("fullScreen"));
        m << pathValue << toClipboard << delay << id;
        QDBusConnection sessionBus = QDBusConnection::sessionBus();
        dbusUtils.checkDBusConnection(sessionBus);
        sessionBus.call(m);

        if (isRaw) {
            dbusUtils.connectPrintCapture(sessionBus, id);
            // timeout just in case
            QTimer t;
            t.setInterval(delay + 2000);
            QObject::connect(&t, &QTimer::timeout, qApp,
                             &QCoreApplication::quit);
            t.start();
            // wait
            return app.exec();
        }
    }
    else if (parser.isSet(screenArgument)) { // SCREEN
        QString numberStr = parser.value(screenNumberOption);
        int number = numberStr.startsWith(QLatin1String("-")) ? -1 : numberStr.toInt();
        QString pathValue = parser.value(pathOption);
        int delay = parser.value(delayOption).toInt();
        bool toClipboard = parser.isSet(clipboardOption);
        bool isRaw = parser.isSet(rawImageOption);
        // Not a valid command
        if(pathValue.isEmpty())
        {
            QStringList a = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
            pathValue= a.at(0);
        }
        if (!isRaw && !toClipboard && pathValue.isEmpty()) {
            QTextStream out(stdout);
            out << "Invalid format, set where to save the content with one of "
                << "the following flags:\n "
                << pathOption.dashedNames().join(QStringLiteral(", ")) << "\n "
                << rawImageOption.dashedNames().join(QStringLiteral(", ")) << "\n "
                << clipboardOption.dashedNames().join(QStringLiteral(", ")) << "\n\n";
            parser.parse(QStringList() << argv[0] << QStringLiteral("screen") << QStringLiteral("-h"));
            goto finish;
        }

        CaptureRequest req(CaptureRequest::SCREEN_MODE,
                           delay, pathValue, number);
        if (toClipboard) {
            req.addTask(CaptureRequest::CLIPBOARD_SAVE_TASK);
        }
        if (!pathValue.isEmpty()) {
            req.addTask(CaptureRequest::FILESYSTEM_SAVE_TASK);
        }
        uint id = req.id();
        DBusUtils dbusUtils;

        // Send message
        QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.dharkael.kylinscreenshot"),
                                               QStringLiteral("/"), QLatin1String(""), QStringLiteral("captureScreen"));
        m << number << pathValue << toClipboard << delay << id;
        QDBusConnection sessionBus = QDBusConnection::sessionBus();
        dbusUtils.checkDBusConnection(sessionBus);
        sessionBus.call(m);

        if (isRaw) {
            dbusUtils.connectPrintCapture(sessionBus, id);
            // timeout just in case
            QTimer t;
            t.setInterval(delay + 2000);
            QObject::connect(&t, &QTimer::timeout, qApp,
                             &QCoreApplication::quit);
            t.start();
            // wait
            return app.exec();
        }
    }
    else if (parser.isSet(configArgument)) { // CONFIG
        bool autostart = parser.isSet(autostartOption);
        bool filename = parser.isSet(filenameOption);
        bool tray = parser.isSet(trayOption);
        bool help = parser.isSet(showHelpOption);
        bool mainColor = parser.isSet(mainColorOption);
        bool contrastColor = parser.isSet(contrastColorOption);
        bool someFlagSet = (filename || tray || help ||
                            mainColor || contrastColor);
        ConfigHandler config;
        if (autostart) {
            QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.dharkael.kylinscreenshot"),
                                               QStringLiteral("/"), QLatin1String(""), QStringLiteral("autostartEnabled"));
            if (parser.value(autostartOption) == QLatin1String("false")) {
                m << false;
            } else if (parser.value(autostartOption) == QLatin1String("true")) {
                m << true;
            }
            QDBusConnection sessionBus = QDBusConnection::sessionBus();
            if (!sessionBus.isConnected()) {
                SystemNotification().sendMessage(
                            QObject::tr("Unable to connect via DBus"));
            }
            sessionBus.call(m);
        }
        if (filename) {
            QString newFilename(parser.value(filenameOption));
            config.setFilenamePattern(newFilename);
            FileNameHandler fh;
            QTextStream(stdout)
                    << QStringLiteral("The new pattern is '%1'\n"
                                      "Parsed pattern example: %2\n").arg(newFilename)
                       .arg(fh.parsedPattern());
        }
        if (tray) {
            QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.dharkael.kylinscreenshot"),
                                               QStringLiteral("/"), QLatin1String(""), QStringLiteral("trayIconEnabled"));
            if (parser.value(trayOption) == QLatin1String("false")) {
                m << false;
            } else if (parser.value(trayOption) == QLatin1String("true")) {
                m << true;
            }
            QDBusConnection sessionBus = QDBusConnection::sessionBus();
            if (!sessionBus.isConnected()) {
                SystemNotification().sendMessage(
                            QObject::tr("Unable to connect via DBus"));
            }
            sessionBus.call(m);
        }
        if (help) {
            if (parser.value(showHelpOption) == QLatin1String("false")) {
                config.setShowHelp(false);
            } else if (parser.value(showHelpOption) == QLatin1String("true")) {
                config.setShowHelp(true);
            }
        }
        if (mainColor) {
            QString colorCode = parser.value(mainColorOption);
            QColor parsedColor(colorCode);
            config.setUIMainColor(parsedColor);
        }
        if (contrastColor) {
            QString colorCode = parser.value(contrastColorOption);
            QColor parsedColor(colorCode);
            config.setUIContrastColor(parsedColor);
        }

        // Open gui when no options
        if (!someFlagSet) {
            QDBusMessage m = QDBusMessage::createMethodCall(QStringLiteral("org.dharkael.kylinscreenshot"),
                                               QStringLiteral("/"), QLatin1String(""), QStringLiteral("openConfig"));
            QDBusConnection sessionBus = QDBusConnection::sessionBus();
            if (!sessionBus.isConnected()) {
                SystemNotification().sendMessage(
                            QObject::tr("Unable to connect via DBus"));
            }
            sessionBus.call(m);
        }
    }
finish:

#endif
    return 0;
}
