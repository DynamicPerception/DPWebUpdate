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
include(update_index/update_index.pri)

defineTest(deployFiles) {
    files = $$1


    for(FILE, files) {
        FILE = $$FILE
        DEPFILECOPY += @echo "Copying $$FILE" $$escape_expand(\\n\\t)
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g


        DEPFILECOPY += $$QMAKE_COPY $$quote($$FILE) $$quote($$DEPDIR) $$escape_expand(\\n\\t)
    }
    export(DEPFILECOPY)
}


win32 {
    CONFIG(debug, debug|release) {
        DEPDIR = $$OUT_PWD/debug
    }
    CONFIG(release, debug|release) {
        DEPDIR = $$OUT_PWD/release
    }
    DEPDIR ~= s,/,\\,g



    # only copy dlls and destroy source files for release versions
 CONFIG(release, debug|release) {

 contains(QT_VERSION, "4.8.1") {
    DEP_FILES = \
       $$[QT_INSTALL_BINS]\\QtCore4.dll \
       $$[QT_INSTALL_BINS]\\QtGui4.dll \
       $$[QT_INSTALL_BINS]\\QtXml4.dll \
      $$[QT_INSTALL_BINS]\\QtNetwork4.dll \
      $$[QT_INSTALL_BINS]\\..\\..\\..\\..\\..\\mingw\\bin\\mingwm10.dll \
      $$[QT_INSTALL_BINS]\\..\\..\\..\\..\\..\\mingw\\bin\\libgcc_s_dw2-1.dll \
      $$OUT_PWD\\..\\libs\\qextserialport\\src\\build\\qextserialport1.dll
 }

 !contains(QT_VERSION, "4.8.1") {

    DEP_FILES = \
       $$[QT_INSTALL_BINS]\\QtCore4.dll \
      $$[QT_INSTALL_BINS]\\QtGui4.dll \
      $$[QT_INSTALL_BINS]\\QtXml4.dll \
      $$[QT_INSTALL_BINS]\\QtNetwork4.dll \
      $$[QT_INSTALL_BINS]\\mingwm10.dll \
      $$[QT_INSTALL_BINS]\\libgcc_s_dw2-1.dll \
      $$OUT_PWD\\..\\libs\\qextserialport\\src\\build\\qextserialport1.dll
  }


    deployFiles($$DEP_FILES)

    # need to deploy needed dll's, etc. more easily

    deploy_copy.target = windep
    deploy_copy.commands = @echo "GRPRO: Copying Required DLL files for Windows" $$escape_expand(\\n\\t)
    deploy_copy.commands += $$DEPFILECOPY

    QMAKE_EXTRA_TARGETS += deploy_copy
    POST_TARGETDEPS += windep
    QMAKE_POST_LINK += $$PWD\\clean_win.bat $$DEPDIR
 }

}

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




