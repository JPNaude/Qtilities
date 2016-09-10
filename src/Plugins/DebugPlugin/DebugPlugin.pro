# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
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

QT += xml
greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport \
      testlib
}
lessThan(QT_MAJOR_VERSION, 5) {
    CONFIG += qtestlib
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
HEADERS += \
    source/DebugPluginConstants.h \
    source/DebugPlugin_global.h \
    source/DebugPlugin.h \

SOURCES += \
    source/DebugPlugin.cpp \

#RESOURCES += \
#    resources/DebugPlugin.qrc \

