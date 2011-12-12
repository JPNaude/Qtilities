# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Help Plugin Library
# ***************************************************************************
QTILITIES += extension_system
include(../../Qtilities.pri)

TARGET   = HelpPlugin

CONFIG(debug, debug|release) {
    TARGET = HelpPlugind
} else {
    TARGET = HelpPlugin
}

DESTDIR += $$QTILITIES_BIN/plugins

TEMPLATE = lib
DEFINES += HELP_PLUGIN_LIBRARY
CONFIG  += plugin
CONFIG  += help

QT += webkit

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/HelpPlugin
MOC_DIR         = $$QTILITIES_TEMP/HelpPlugin
RCC_DIR         = $$QTILITIES_TEMP/HelpPlugin
UI_DIR          = $$QTILITIES_TEMP/HelpPlugin

# --------------------------
# Library Files
# --------------------------
HEADERS += source/HelpPlugin.h \
           source/HelpPlugin_global.h \
           source/HelpPluginConstants.h \
           source/HelpMode.h \
           source/ContentWidgetFactory.h \
           source/IndexWidgetFactory.h \
           source/SearchWidgetFactory.h \
           source/HelpBrowser.h

SOURCES += source/HelpPlugin.cpp \
           source/HelpMode.cpp \
           source/ContentWidgetFactory.cpp \
           source/IndexWidgetFactory.cpp \
           source/SearchWidgetFactory.cpp \
           source/HelpBrowser.cpp

RESOURCES += resources/resources.qrc

