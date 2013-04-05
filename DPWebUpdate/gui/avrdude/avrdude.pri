
win32 {

    OTHER_FILES += \
        avrdude/win/avrdude.exe \
        avrdude/win/avrdude.conf \
        avrdude/win/libusb0.dll

    AVR_FILES = \
        win/avrdude.exe \
        win/avrdude.conf \
        win/libusb0.dll

    CONFIG(debug, debug|release) {
        DDIR = $$OUT_PWD/debug/avrdude
    }
    CONFIG(release, debug|release) {
        DDIR = $$OUT_PWD/release/avrdude
    }

    AVR_FILES ~= s,/,\\,g
    DDIR ~= s,/,\\,g
}

macx {
    OTHER_FILES += \
        avrdude/osx/avrdude \
        avrdude/osx/avrdude.conf

    AVR_FILES = \
        osx/avrdude \
        osx/avrdude.conf


    DDIR = $$OUT_PWD/DPWebUpdate.app/Contents/MacOS/avrdude
}


defineTest(copyAvrDudeFiles) {
    files = $$1

    for(FILE, files) {
        ORIG_FILE = $$FILE
        win32:ORIG_FILE ~= s,\\win,,g
        macx:ORIG_FILE ~= s,/osx,,g
        FILE = $$PWD/$$FILE
        AVRFILECOPY += @echo "Copying $$FILE" $$escape_expand(\\n\\t)
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g


        macx {
            AVRFILECOPY += $$QMAKE_COPY -R $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
        }

        win32 {
            AVRFILECOPY += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR\\$$ORIG_FILE) $$escape_expand(\\n\\t)
        }

    }


    export(AVRFILECOPY)
}


copyAvrDudeFiles($$AVR_FILES)

avr_copy.target = avr
avr_copy.commands = @echo "Copying AVR files to $$DDIR"  $$escape_expand(\\n\\t)

win32 {

  avr_copy.commands += mkdir $$DDIR $$escape_expand(\\n\\t)
  avr_copy.commands += $$AVRFILECOPY
}

macx {

 !exists($$DDIR) {
    avr_copy.commands += @echo "Creating AVR Directory: $$DDIR" $$escape_expand(\\n\\t)
    avr_copy.commands += mkdir -p $$DDIR $$escape_expand(\\n\\t)
  }

  avr_copy.commands += $$AVRFILECOPY

}

QMAKE_EXTRA_TARGETS += avr_copy
POST_TARGETDEPS += avr
