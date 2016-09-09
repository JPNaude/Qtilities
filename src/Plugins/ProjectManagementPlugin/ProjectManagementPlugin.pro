# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Project Management Plugin Library
# ***************************************************************************
QTILITIES += extension_system
QTILITIES += project_management
include(../../Qtilities.pri)

CONFIG(debug, debug|release) {
    TARGET = ProjectManagementPlugind
} else {
    TARGET = ProjectManagementPlugin
}

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets
}

DESTDIR += $$QTILITIES_BIN/plugins
TEMPLATE = lib
DEFINES += PROJECT_MANAGEMENT_PLUGIN_LIBRARY
CONFIG += plugin dll

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/ProjectManagementPlugin
MOC_DIR         = $$QTILITIES_TEMP/ProjectManagementPlugin
RCC_DIR         = $$QTILITIES_TEMP/ProjectManagementPlugin
UI_DIR          = $$QTILITIES_TEMP/ProjectManagementPlugin

# --------------------------
# Library Files
# --------------------------
HEADERS +=  \
            source/ProjectManagementPluginConstants.h \
            source/ProjectManagementPlugin_global.h \
            source/ProjectManagementPlugin.h \

SOURCES +=  \
            source/ProjectManagementPlugin.cpp \
