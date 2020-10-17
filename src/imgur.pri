# Use default Imgur client_id if user did not pass
# this variable to qmake
isEmpty(IMGUR_CLIENT_ID) {
    IMGUR_CLIENT_ID = "313baf0c7b4d3ff"
}

DEFINES += IMGUR_CLIENT_ID=\\\"$${IMGUR_CLIENT_ID}\\\"

HEADERS += \
    $$PWD/tools/CaptureToolCap.h \
    $$PWD/tools/cut/cut.h \
    $$PWD/tools/luping/luping.h \
    $$PWD/tools/options/options.h \
    $$PWD/tools/save/saveastool.h \
    $$PWD/tools/screencap/screencap.h \
    $$PWD/tools/screencapexit/screencapexit.h \
    $$PWD/tools/screencapoption/screencapoption.h \
    $$PWD/tools/screencapstart/screencapstart.h \
    $$PWD/tools/screencut/screencut.h \
    $$PWD/tools/screenmouse/screenmouse.h \
    $$PWD/tools/screenmousecursor/screenmousecursor.h \
    $$PWD/tools/screenvoice/screenvoice.h \
    $$PWD/tools/toolfactorycap.h \
    $$PWD/widgets/capture/buttonhandercap.h \
    $$PWD/widgets/capture/capturebuttoncap.h \
    $$PWD/widgets/capture/fontsize_color_chose.h \
    $$PWD/widgets/capture/fontsize_color_chose2.h \
    $$PWD/widgets/capture/save_location.h \
    $$PWD/widgets/capture/save_location2.h

SOURCES += \
    $$PWD/tools/cut/cut.cpp \
    $$PWD/tools/luping/luping.cpp \
    $$PWD/tools/options/options.cpp \
    $$PWD/tools/save/saveastool.cpp \
    $$PWD/tools/screencap/screencap.cpp \
    $$PWD/tools/screencapexit/screencapexit.cpp \
    $$PWD/tools/screencapoption/screencapoption.cpp \
    $$PWD/tools/screencapstart/screencapstart.cpp \
    $$PWD/tools/screencut/screencut.cpp \
    $$PWD/tools/screenmouse/screenmouse.cpp \
    $$PWD/tools/screenmousecursor/screenmousecursor.cpp \
    $$PWD/tools/screenvoice/screenvoice.cpp \
    $$PWD/tools/toolfactorycap.cpp \
    $$PWD/widgets/capture/buttonhandercap.cpp \
    $$PWD/widgets/capture/capturebuttoncap.cpp \
    $$PWD/widgets/capture/fontsize_color_chose.cpp \
    $$PWD/widgets/capture/fontsize_color_chose2.cpp \
    $$PWD/widgets/capture/save_location.cpp \
    $$PWD/widgets/capture/save_location2.cpp
