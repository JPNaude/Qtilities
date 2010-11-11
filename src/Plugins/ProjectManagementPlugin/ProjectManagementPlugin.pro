# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
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

DESTDIR += $$QTILITIES_BIN/plugins
TEMPLATE = lib
DEFINES += PROJECT_MANAGEMENT_PLUGIN_LIBRARY
CONFIG += plugin

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
HEADERS +=  source/ProjectManagementPlugin.h \
            source/ProjectManagementPlugin_global.h \
            source/ProjectManagementPluginConstants.h

SOURCES +=  source/ProjectManagementPlugin.cpp

RESOURCES += \
    resources/ProjectManagementPlugin.qrc
