# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Plugin Template Library
# ***************************************************************************
QTILITIES += extension_system
include(../../Qtilities.pri)

CONFIG(debug, debug|release) {
    TARGET = PluginTemplated
} else {
    TARGET = PluginTemplate
}

QT += gui

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

DESTDIR += $$QTILITIES_BIN/plugins
TEMPLATE = lib
DEFINES += PLUGIN_TEMPLATE_LIBRARY
CONFIG += plugin dll

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/PluginTemplate
MOC_DIR         = $$QTILITIES_TEMP/PluginTemplate
RCC_DIR         = $$QTILITIES_TEMP/PluginTemplate
UI_DIR          = $$QTILITIES_TEMP/PluginTemplate

# --------------------------
# Library Files
# --------------------------
HEADERS += source/PluginTemplate.h \
           source/PluginTemplate_global.h \
           source/PluginTemplateConstants.h

SOURCES += source/PluginTemplate.cpp

RESOURCES += \
    resources/PluginTemplate.qrc
