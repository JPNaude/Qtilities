# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
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
CONFIG += plugin dll

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport
}

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
HEADERS +=  \
            source/SessionLogMode.h \
            source/SessionLogPluginConstants.h \
            source/SessionLogPlugin_global.h \
            source/SessionLogPlugin.h \

SOURCES +=  \
            source/SessionLogMode.cpp \
            source/SessionLogPlugin.cpp \

RESOURCES += \
            resources/SessionLogPlugin.qrc \
