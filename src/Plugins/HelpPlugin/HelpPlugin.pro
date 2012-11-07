# ***************************************************************************
# Copyright (c) 2009-2012, Jaco Naude
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
CONFIG  += plugin dll
CONFIG  += help

QT += network
QT += webkit

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

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
contains(DEFINES, QTILITIES_NO_HELP) {
    # Do nothing here for now.
} else {
HEADERS += source/HelpPlugin.h \
           source/HelpPlugin_global.h \
           source/HelpPluginConstants.h \
           source/HelpMode.h \
           source/ContentWidgetFactory.h \
           source/IndexWidgetFactory.h \
           source/SearchWidgetFactory.h \
           source/HelpPluginConfig.h \
           source/Browser.h

SOURCES += source/HelpPlugin.cpp \
           source/HelpMode.cpp \
           source/ContentWidgetFactory.cpp \
           source/IndexWidgetFactory.cpp \
           source/SearchWidgetFactory.cpp \
           source/HelpPluginConfig.cpp \
           source/Browser.cpp

FORMS   += source/HelpPluginConfig.ui

RESOURCES += resources/resources.qrc
}


