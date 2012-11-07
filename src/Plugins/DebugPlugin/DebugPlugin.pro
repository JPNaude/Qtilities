# ***************************************************************************
# Copyright (c) 2009-2012, Jaco Naude
#
# See Dependencies.pri for the licensing requirements of the Debug plugin.
#
# ***************************************************************************
# Qtilities Debug Plugin Library
# ***************************************************************************
QTILITIES += extension_system
QTILITIES += project_management
QTILITIES += testing
include(../../Qtilities.pri)

CONFIG(debug, debug|release) {
    TARGET = DebugPlugind
} else {
    TARGET = DebugPlugin
}

DESTDIR += $$QTILITIES_BIN/plugins
TEMPLATE = lib
DEFINES += DEBUG_PLUGIN_LIBRARY
CONFIG += plugin dll
CONFIG += qtestlib

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
           source/DebugPluginConstants.h

SOURCES += source/DebugPlugin.cpp

RESOURCES += \
    resources/DebugPlugin.qrc

