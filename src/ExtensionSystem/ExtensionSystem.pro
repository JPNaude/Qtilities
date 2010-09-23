# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Extension System Library
# ***************************************************************************
QTILITIES += coregui
include(../Qtilities.pri)
INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesExtensionSystem

CONFIG += qt \
    dll \
    ordered

QT += core
#QT -= gui

CONFIG(debug, debug|release) {
    TARGET = QtilitiesExtensionSystemd$${QTILITIES_FILE_VER}
} else {
    TARGET = QtilitiesExtensionSystem$${QTILITIES_FILE_VER}
}

TEMPLATE = lib

DEFINES += EXTENSION_SYSTEM_LIBRARY
DESTDIR = $$QTILITIES_BIN

OBJECTS_DIR = $$QTILITIES_TEMP/ExtensionSystem
MOC_DIR = $$QTILITIES_TEMP/ExtensionSystem
RCC_DIR = $$QTILITIES_TEMP/ExtensionSystem
UI_DIR = $$QTILITIES_TEMP/ExtensionSystem

# --------------------------
# Extension Library Files
# --------------------------
HEADERS += source/ExtensionSystemCore.h \
        source/ExtensionSystem_global.h \
        source/ExtensionSystemConstants.h \
        source/IPlugin.h \
        source/ExtensionSystemConfig.h \
    source/PluginInfoWidget.h
SOURCES += source/ExtensionSystemCore.cpp \
        source/ExtensionSystemConfig.cpp \
    source/PluginInfoWidget.cpp
FORMS   += source/ExtensionSystemConfig.ui \
    source/PluginInfoWidget.ui
