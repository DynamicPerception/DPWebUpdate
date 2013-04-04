
win32 {

    CONFIG(debug, debug|release) {
        DDIR = $$OUT_PWD/debug/avrdude
    }
    CONFIG(release, debug|release) {
        DDIR = $$OUT_PWD/release/avrdude
    }
    DDIR ~= s,/,\\,g
}

macx {
    OTHER_FILES += \
        avrdude/avrdude \
        avrdude/avrdude.conf

    AVR_FILES = \
        avrdude \
        avrdude.conf


    DDIR = $$OUT_PWD/DPWebUpdate.app/Contents/MacOS/avrdude
}


defineTest(copyAvrDudeFiles) {
    files = $$1

    for(FILE, files) {
        ORIG_FILE = $$FILE
        FILE = $$PWD/$$FILE
        AVRFILECOPY += @echo "Copying $$FILE" $$escape_expand(\\n\\t)
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g


        macx {
            AVRFILECOPY += $$QMAKE_COPY -R $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
        }

        win32 {
            AVRFILECOPY += xcopy /Y /E /I $$quote($$FILE) $$quote($$DDIR\\$$ORIG_FILE) $$escape_expand(\\n\\t)
        }

    }


    export(AVRFILECOPY)
}


copyAvrDudeFiles($$AVR_FILES)

avr_copy.target = avr
avr_copy.commands = @echo "Copying AVR files to $$DDIR"  $$escape_expand(\\n\\t)

win32 {

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
