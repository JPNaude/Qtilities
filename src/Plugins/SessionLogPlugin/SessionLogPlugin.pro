# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Session Log Plugin Library
# ***************************************************************************
QTILITIES += extension_system
include(../../Qtilities.pri)

CONFIG(debug, debug|release) {
    TARGET = SessionLogPlugind
} else {
    TARGET = SessionLogPlugin
}

DESTDIR += $$QTILITIES_BIN/plugins
TEMPLATE = lib
DEFINES += SESSION_LOG_PLUGIN_LIBRARY
CONFIG += plugin

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/SessionLogPlugin
MOC_DIR         = $$QTILITIES_TEMP/SessionLogPlugin
RCC_DIR         = $$QTILITIES_TEMP/SessionLogPlugin
UI_DIR          = $$QTILITIES_TEMP/SessionLogPlugin

# --------------------------
# Library Files
# --------------------------
HEADERS +=  source/SessionLogPlugin.h \
            source/SessionLogPlugin_global.h \
            source/SessionLogPluginConstants.h \
            source/SessionLogMode.h

SOURCES +=  source/SessionLogPlugin.cpp \
            source/SessionLogMode.cpp

RESOURCES += \
    resources/SessionLogPlugin.qrc
