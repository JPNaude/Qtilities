# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Extension System Library
# ***************************************************************************
QTILITIES += coregui
include(../Qtilities.pri)
INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesExtensionSystem

CONFIG += qt dll
QT += core xml gui
# Note: xml module is deprecated

greaterThan(QT_MAJOR_VERSION, 4) { QT += widgets printsupport }

TARGET = QtilitiesExtensionSystem$${QTILITIES_LIB_POSTFIX}

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
HEADERS += \
        source/ExtensionSystemConfig.h \
        source/ExtensionSystemConstants.h \
        source/ExtensionSystemCore.h \
        source/ExtensionSystem_global.h \
        source/IPlugin.h \
        source/PluginInfoWidget.h \
        source/PluginTreeModel.h \

SOURCES += \
        source/ExtensionSystemConfig.cpp \
        source/ExtensionSystemCore.cpp \
        source/PluginInfoWidget.cpp \
        source/PluginTreeModel.cpp \

FORMS   += \
        source/ExtensionSystemConfig.ui \
        source/PluginInfoWidget.ui \
