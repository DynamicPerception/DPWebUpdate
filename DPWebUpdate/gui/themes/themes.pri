# copy theme files into correct build/deploy locations

CONFIG(release, debug|release) {
    # un-comment to disable all debugging output from libraries

    DEFINES *= QT_NO_DEBUG_OUTPUT
}

win32 {

    CONFIG(debug, debug|release) {
        DDIR = $$OUT_PWD/debug/themes
    }
    CONFIG(release, debug|release) {
        DDIR = $$OUT_PWD/release/themes
    }
    DDIR ~= s,/,\\,g
}

macx {

    DDIR = $$OUT_PWD/DPWebUpdate.app/Contents/MacOS/themes
}

defineTest(copyThemeFiles) {
    files = $$1

    macx {
        files += README.txt
    }

    for(FILE, files) {
        ORIG_FILE = $$FILE
        FILE = $$PWD/$$FILE
        THEMEFILECOPY += @echo "Copying $$FILE" $$escape_expand(\\n\\t)
        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g


        macx {
            THEMEFILECOPY += $$QMAKE_COPY -R $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
        }

        win32 {
            THEMEFILECOPY += xcopy /Y /E /I $$quote($$FILE) $$quote($$DDIR\\$$ORIG_FILE) $$escape_expand(\\n\\t)
        }

    }

    win32 {
        NEWFILE = $$PWD/README.txt
        NEWFILE ~= s,/,\\,g
        THEMEFILECOPY += $$QMAKE_COPY /Y $$quote($$NEWFILE) $$quote($$DIR\\$$ORIG_FILE) $$escape_expand(\\n\\t)
    }

    export(THEMEFILECOPY)
}

# list all theme directories which should be included as part of the base build
THEME_FILES = \
        DP-Blue


OTHER_FILES += \
     themes/DP-Blue \
     themes/DP-Blue/about.qss \
     themes/DP-Blue/confirm.qss \
     themes/DP-Blue/error.qss \
     themes/DP-Blue/img/drop-down-sm-blue.png \
     themes/DP-Blue/img/small-blue-but-normal.png \
     themes/DP-Blue/img/small-gray-but-normal.png \
     themes/DP-Blue/info.qss \
     themes/DP-Blue/main.qss \
     themes/DP-Blue/theme.ini \
     themes/DP-Blue/update.qss \
     themes/README.txt \
     themes/themes.pri


copyThemeFiles($$THEME_FILES)

theme_copy.target = themes
theme_copy.commands = @echo "Copying Theme files to $$DDIR"  $$escape_expand(\\n\\t)

win32 {

  theme_copy.commands += $$THEMEFILECOPY
}

macx {

 !exists($$DDIR) {
    theme_copy.commands += @echo "Creating Theme Directory: $$DDIR" $$escape_expand(\\n\\t)
    theme_copy.commands += mkdir -p $$DDIR $$escape_expand(\\n\\t)
  }

  theme_copy.commands += $$THEMEFILECOPY

}

QMAKE_EXTRA_TARGETS += theme_copy
POST_TARGETDEPS += themes
