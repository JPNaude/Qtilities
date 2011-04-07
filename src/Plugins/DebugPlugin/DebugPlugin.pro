# ***************************************************************************
# Copyright (c) 2009-2011, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Debug Plugin Library
# ***************************************************************************
QTILITIES += extension_system
QTILITIES += project_management
include(../../Qtilities.pri)

CONFIG(debug, debug|release) {
    TARGET = DebugPlugind
} else {
    TARGET = DebugPlugin
}

DESTDIR += $$QTILITIES_BIN/plugins
TEMPLATE = lib
DEFINES += DEBUG_PLUGIN_LIBRARY
CONFIG += plugin

# --------------------------
# Conan Stuff
# Integrates the Conan Library into the debug widget:
# https://sourceforge.net/projects/conanforqt/
#
# Requires at least, and tested with Conan v1.0.2.
# --------------------------
!contains(DEFINES, QTILITIES_NO_CONAN) {
    CONAN_BASE = $$QTILITIES_DEPENDENCIES/conanforqt/Conan
    INCLUDEPATH += $$CONAN_BASE/include

    CONFIG(debug, debug|release) {
        win32:LIBS += $$CONAN_BASE/lib/libConand1.a
        unix::LIBS += -L$$CONAN_BASE/lib -lConand
    }
    else {
        win32:LIBS += $$CONAN_BASE/lib/libConan1.a
        unix::LIBS += -L$$CONAN_BASE/lib -lConan
    }
}

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/DebugPlugin
MOC_DIR         = $$QTILITIES_TEMP/DebugPlugin
RCC_DIR         = $$QTILITIES_TEMP/DebugPlugin
UI_DIR          = $$QTILITIES_TEMP/DebugPlugin

# --------------------------
# Library Files
# --------------------------
HEADERS += source/DebugPlugin.h \
           source/DebugPlugin_global.h \
           source/DebugPluginConstants.h \
           source/DebugWidget.h \
           source/DropableListWidget.h

SOURCES += source/DebugPlugin.cpp \
           source/DebugWidget.cpp \
           source/DropableListWidget.cpp

RESOURCES += \
    resources/DebugPlugin.qrc

FORMS += \
    source/DebugWidget.ui
