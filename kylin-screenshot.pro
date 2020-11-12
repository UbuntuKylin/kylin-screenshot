#-------------------------------------------------
#
# Project created by Dharkael 2017-04-21T00:42:49
#
#-------------------------------------------------

win32:LIBS += -luser32 -lshell32

LIBS += -lavutil -lavformat -lavcodec -lswscale -lX11 -lXext -lXfixes -lXinerama -lpulse

TAG_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)
isEmpty(TAG_VERSION){
    TAG_VERSION = v0.6.0
}
DEFINES += APP_VERSION=\\\"$$TAG_VERSION\\\"

QT  += core gui widgets network svg x11extras KWindowSystem

unix:!macx {
    QT  += dbus
}

CONFIG += c++11 link_pkgconfig 
PKGCONFIG += gsettings-qt
QMAKE_CXXFLAGS += -std=c++0x

#CONFIG += packaging   # Enables "make install" for packaging paths

TARGET = kylin-screenshot
TEMPLATE = app

win32:RC_ICONS += img/app/kylin_screenshot.ico

#release: DESTDIR = build/release
#debug:   DESTDIR = build/debug

#OBJECTS_DIR = $$DESTDIR/.obj
#MOC_DIR = $$DESTDIR/.moc
#RCC_DIR = $$DESTDIR/.qrc
#UI_DIR = $$DESTDIR/.ui

TRANSLATIONS = translations/Internationalization_es.ts \
    translations/Internationalization_ca.ts \
    translations/Internationalization_ru.ts \
    translations/Internationalization_zh_CN.ts \
    translations/Internationalization_zh_TW.ts \
    translations/Internationalization_tr.ts \
    translations/Internationalization_ka.ts \
    translations/Internationalization_fr.ts \
    translations/Internationalization_pl.ts \
    translations/Internationalization_ja.ts \
    translations/Internationalization_pt_br.ts \
    translations/Internationalization_sr.ts \
    translations/Internationalization_uk.ts \
    translations/Internationalization_de_DE.ts \
    translations/Internationalization_sk.ts \

# Generate translations in build
TRANSLATIONS_FILES =

qtPrepareTool(LRELEASE, lrelease)
for(tsfile, TRANSLATIONS) {
    qmfile = $$shadowed($$tsfile)
    qmfile ~= s,.ts$,.qm,
    qmdir = $$dirname(qmfile)
    !exists($$qmdir) {
        mkpath($$qmdir)|error("Aborting.")
    }
    command = $$LRELEASE -removeidentical $$tsfile -qm $$qmfile
    system($$command)|error("Failed to run: $$command")
    TRANSLATIONS_FILES += $$qmfile
}

DEFINES += QT_DEPRECATED_WARNINGS

include(src/third-party/singleapplication/singleapplication.pri)
include(src/third-party/Qt-Color-Widgets//color_widgets.pri)

DEFINES += QAPPLICATION_CLASS=QApplication

SOURCES += src/main.cpp \
    src/AV/AVWrapper.cpp \
    src/AV/FastResamper_FirFilter_Fallback.cpp \
    src/AV/FastResampler.cpp \
    src/AV/FastScaler.cpp \
    src/AV/FastScaler_Convert_Fallback.cpp \
    src/AV/FastScaler_Convert_SSSE3.cpp \
    src/AV/FastScaler_Scale_Fallback.cpp \
    src/AV/FastScaler_Scale_Generic.cpp \
    src/AV/FastScaler_Scale_SSSE3.cpp \
    src/AV/SourceSink.cpp \
    src/AV/input/PulseAudioInput.cpp \
    src/AV/input/X11Input.cpp \
    src/AV/input/input_widgets.cpp \
    src/AV/output/AudioEncoder.cpp \
    src/AV/output/BaseEncoder.cpp \
    src/AV/output/Muxer.cpp \
    src/AV/output/OutputManager.cpp \
    src/AV/output/SyncDiagram.cpp \
    src/AV/output/Synchronizer.cpp \
    src/AV/output/VideoEncoder.cpp \
    src/AV/output/X264Presets.cpp \
    src/common/CPUFeatures.cpp \
    src/common/CommandLineOptions.cpp \
    src/common/Logger.cpp \
    src/common/WidgetWrapper.cpp \
    src/common/utils.cpp \
    src/widgets/capture/buttonhandler.cpp \
    src/widgets/capture/font_options.cpp \
    src/widgets/capture/font_options2.cpp \
    src/widgets/infowindow.cpp \
    src/config/configwindow.cpp \
    src/widgets/capture/capturewidget.cpp \
    src/widgets/capture/colorpicker.cpp \
    src/config/buttonlistview.cpp \
    src/config/uicoloreditor.cpp \
    src/config/geneneralconf.cpp \
    src/core/controller.cpp \
    src/config/clickablelabel.cpp \
    src/config/filenameeditor.cpp \
    src/config/strftimechooserwidget.cpp \
    src/widgets/capture/capturebutton.cpp \
    src/tools/pencil/penciltool.cpp \
    src/tools/undo/undotool.cpp \
    src/tools/redo/redotool.cpp \
    src/tools/arrow/arrowtool.cpp \
    src/tools/circle/circletool.cpp \
    src/tools/copy/copytool.cpp \
    src/tools/exit/exittool.cpp \
    src/tools/imgur/imguruploadertool.cpp \
    src/tools/line/linetool.cpp \
    src/tools/marker/markertool.cpp \
    src/tools/move/movetool.cpp \
    src/tools/rectangle/rectangletool.cpp \
    src/tools/save/savetool.cpp \
    src/tools/selection/selectiontool.cpp \
    src/tools/sizeindicator/sizeindicatortool.cpp \
    src/tools/toolfactory.cpp \
    src/utils/filenamehandler.cpp \
    src/utils/screengrabber.cpp \
    src/utils/confighandler.cpp \
    src/utils/systemnotification.cpp \
    src/cli/commandlineparser.cpp \
    src/cli/commandoption.cpp \
    src/cli/commandargument.cpp \
    src/utils/screenshotsaver.cpp \
    src/tools/imgur/imguruploader.cpp \
    src/widgets/loadspinner.cpp \
    src/widgets/imagelabel.cpp \
    src/widgets/mypopup.cpp \
    src/widgets/notificationwidget.cpp \
    src/widgets/capture/notifierbox.cpp \
    src/utils/desktopinfo.cpp \
    src/tools/launcher/applauncherwidget.cpp \
    src/utils/desktopfileparse.cpp \
    src/tools/launcher/launcheritemdelegate.cpp \
    src/tools/blur/blurtool.cpp \
    src/tools/pin/pintool.cpp \
    src/tools/launcher/terminallauncher.cpp \
    src/config/visualseditor.cpp \
    src/config/extendedslider.cpp \
    src/tools/launcher/openwithprogram.cpp \
    src/tools/launcher/applaunchertool.cpp \
    src/utils/pathinfo.cpp \
    src/utils/colorutils.cpp \
    src/tools/capturecontext.cpp \
    src/widgets/capture/modificationcommand.cpp \
    src/tools/abstractpathtool.cpp \
    src/tools/abstracttwopointtool.cpp \
    src/tools/abstractactiontool.cpp \
    src/utils/globalvalues.cpp \
    src/widgets/panel/utilitypanel.cpp \
    src/widgets/capture/hovereventfilter.cpp \
    src/widgets/capture/selectionwidget.cpp \
    src/tools/pin/pinwidget.cpp \
    src/widgets/capturelauncher.cpp \
    src/tools/text/texttool.cpp \
    src/tools/text/textwidget.cpp \
    src/core/capturerequest.cpp \
    src/tools/text/textconfig.cpp \
    src/widgets/panel/sidepanelwidget.cpp \
    src/widgets/screenoption.cpp \
    src/widgets/screenrecorder.cpp \
    src/widgets/screentype.cpp \
    src/widgets/widget.cpp

HEADERS  += src/widgets/capture/buttonhandler.h \
    src/AV/AVWrapper.h \
    src/AV/FastResampler.h \
    src/AV/FastResampler_FirFilter.h \
    src/AV/FastScaler.h \
    src/AV/FastScaler_Convert.h \
    src/AV/FastScaler_Scale.h \
    src/AV/FastScaler_Scale_Generic.h \
    src/AV/SampleCast.h \
    src/AV/SourceSink.h \
    src/AV/input/PulseAudioInput.h \
    src/AV/input/X11Input.h \
    src/AV/input/input_widgets.h \
    src/AV/output/AudioEncoder.h \
    src/AV/output/BaseEncoder.h \
    src/AV/output/Muxer.h \
    src/AV/output/OutputManager.h \
    src/AV/output/OutputSettings.h \
    src/AV/output/SyncDiagram.h \
    src/AV/output/Synchronizer.h \
    src/AV/output/VideoEncoder.h \
    src/AV/output/X264Presets.h \
    src/common/CPUFeatures.h \
    src/common/CommandLineOptions.h \
    src/common/Enum.h \
    src/common/EnumStrings.h \
    src/common/Logger.h \
    src/common/MutexDataPair.h \
    src/common/QueueBuffer.h \
    src/common/SomeData.h \
    src/common/TempBuffer.h \
    src/common/WidgetWrapper.h \
    src/common/utils.h \
    src/core/Global.h \
    src/widgets/capture/font_options.h \
    src/widgets/capture/font_options2.h \
    src/widgets/infowindow.h \
    src/config/configwindow.h \
    src/widgets/capture/capturewidget.h \
    src/widgets/capture/colorpicker.h \
    src/config/buttonlistview.h \
    src/config/uicoloreditor.h \
    src/config/geneneralconf.h \
    src/config/clickablelabel.h \
    src/config/filenameeditor.h \
    src/utils/filenamehandler.h \
    src/config/strftimechooserwidget.h \
    src/utils/screengrabber.h \
    src/tools/capturetool.h \
    src/widgets/capture/capturebutton.h \
    src/tools/pencil/penciltool.h \
    src/tools/undo/undotool.h \
    src/tools/redo/redotool.h \
    src/tools/arrow/arrowtool.h \
    src/tools/circle/circletool.h \
    src/tools/copy/copytool.h \
    src/tools/exit/exittool.h \
    src/tools/imgur/imguruploadertool.h \
    src/tools/line/linetool.h \
    src/tools/marker/markertool.h \
    src/tools/move/movetool.h \
    src/tools/rectangle/rectangletool.h \
    src/tools/save/savetool.h \
    src/tools/selection/selectiontool.h \
    src/tools/sizeindicator/sizeindicatortool.h \
    src/tools/toolfactory.h \
    src/utils/confighandler.h \
    src/core/controller.h \
    src/utils/systemnotification.h \
    src/cli/commandlineparser.h \
    src/cli/commandoption.h \
    src/cli/commandargument.h \
    src/utils/screenshotsaver.h \
    src/tools/imgur/imguruploader.h \
    src/widgets/loadspinner.h \
    src/widgets/imagelabel.h \
    src/widgets/mypopup.h \
    src/widgets/notificationwidget.h \
    src/widgets/capture/notifierbox.h \
    src/utils/desktopinfo.h \
    src/tools/launcher/applauncherwidget.h \
    src/utils/desktopfileparse.h \
    src/tools/launcher/launcheritemdelegate.h \
    src/tools/blur/blurtool.h \
    src/tools/pin/pintool.h \
    src/tools/launcher/terminallauncher.h \
    src/config/visualseditor.h \
    src/config/extendedslider.h \
    src/tools/launcher/openwithprogram.h \
    src/utils/pathinfo.h \
    src/tools/capturecontext.h \
    src/tools/launcher/applaunchertool.h \
    src/utils/globalvalues.h \
    src/utils/colorutils.h \
    src/widgets/capture/modificationcommand.h \
    src/tools/abstractpathtool.h \
    src/tools/abstracttwopointtool.h \
    src/tools/abstractactiontool.h \
    src/widgets/panel/utilitypanel.h \
    src/widgets/capture/hovereventfilter.h \
    src/widgets/capture/selectionwidget.h \
    src/tools/pin/pinwidget.h \
    src/widgets/capturelauncher.h \
    src/tools/text/texttool.h \
    src/tools/text/textwidget.h \
    src/core/capturerequest.h \
    src/tools/text/textconfig.h \
    src/widgets/panel/sidepanelwidget.h \
    src/widgets/screenoption.h \
    src/widgets/screenrecorder.h \
    src/widgets/screentype.h \
    src/widgets/widget.h

unix:!macx {
    SOURCES += src/core/flameshotdbusadapter.cpp \
        src/utils/dbusutils.cpp

    HEADERS  += src/core/flameshotdbusadapter.h \
        src/utils/dbusutils.h
}

win32 {
    SOURCES += src/core/globalshortcutfilter.cpp

    HEADERS  += src/core/globalshortcutfilter.h
}

RESOURCES += \
    graphics.qrc

# installs
unix:!macx {
    isEmpty(PREFIX) {
      packaging {
        PREFIX = /usr
      } else {
        PREFIX = /usr/local
      }
    }

    DEFINES += APP_PREFIX=\\\"$$PREFIX\\\"

    target.path = $${PREFIX}/bin/

    qmfile.path = $${PREFIX}/share/kylin-screenshot/translations/
    qmfile.files = $${TRANSLATIONS_FILES}

    dbus.path = $${PREFIX}/share/dbus-1/interfaces/
    dbus.files = dbus/org.dharkael.kylinscreenshot.xml

    gsettings.path = $${PREFIX}/share/glib-2.0/schemas/
    gsettings.files =  gsettings/org.ukui.screenshot.gschema.xml

    icon.path = $${PREFIX}/share/icons/hicolor/
    icon.files = img/hicolor/*

    completion.path = $${PREFIX}/share/bash-completion/completions/
    completion.files = docs/bash-completion/kylin-screenshot

    appdata.path = $${PREFIX}/share/metainfo/
    appdata.files = docs/appdata/kylinscreenshot.appdata.xml

    desktopentry.path = $${PREFIX}/share/applications
    desktopentry.files = docs/desktopEntry/package/kylin-screenshot.desktop

    servicedbus.path = $${PREFIX}/share/dbus-1/services/

    packaging {
        servicedbus.files = dbus/package/org.dharkael.kylinscreenshot.service
    } else {
        servicedbus.files = dbus/make/org.dharkael.kylinscreenshot.service
    }

    INSTALLS += target \
        icon \
        desktopentry \
        qmfile \
	gsettings \
        servicedbus \
        dbus \
        completion \
        appdata
}

# Imgur API data
include(src/imgur.pri)
