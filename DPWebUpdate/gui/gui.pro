QT       += core gui network xml

TARGET = DPWebUpdate
TEMPLATE = app
UI_DIR = .ui

CONFIG += ordered

INCLUDEPATH  += ../libs/qextserialport/src/ \
                .


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


 # set software version string

DEFINES += "WEBUPD_VERSION=0.01"



win32 {
    LIBS += -L../libs/qextserialport/src/build
}

macx {
    LIBS += -L$$(HOME)/lib
}




 # link to serialport lib, add icons

win32 {
    LIBS += -lqextserialport1
        # set icon
    # RC_FILE = DPWebUpdate.rc
}

macx {
    LIBS += -lqextserialport
        # set icon
    # ICON = DPWebUpdate.icns
}


include(core/core.pri)
include(avrdude/avrdude.pri)
include(themes/themes.pri)


# OSX-Specific Build Instructions

macx {


# QMAKE_INFO_PLIST = $$PWD/Info.plist
# QMAKE_POST_LINK += @echo "Setting PLIST as $$PWD/Info.plist" $$escape_expand(\\n\\t)

 CONFIG(release, debug|release) {

    QMAKE_POST_LINK += @echo "GRPRO: Deploying Mac app" $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/macdeployqt $$OUT_PWD/DPWebUpdate.app $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += @echo "GRPRO: Copying qextserialport" $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += cp -R $$(HOME)/lib/libqextserialport* $$OUT_PWD/DPWebUpdate.app/Contents/Frameworks/ $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += @echo "GRPRO: Correcting paths inside of libqextserialport" $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += install_name_tool -change $$[QT_INSTALL_LIBS]/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore $$OUT_PWD/DPWebUpdate.app/Contents/Frameworks/libqextserialport.1.2.0.dylib $$escape_expand(\\n\\t)
#    QMAKE_POST_LINK += @echo "GRPRO: Cleaning Ojbect and Source files" $$escape_expand(\\n\\t)
#    QMAKE_POST_LINK += rm -rf $$OUT_PWD/*.o $$escape_expand(\\n\\t)
#    QMAKE_POST_LINK += rm -rf $$OUT_PWD/*.cpp $$escape_expand(\\n\\t)
 }
}




