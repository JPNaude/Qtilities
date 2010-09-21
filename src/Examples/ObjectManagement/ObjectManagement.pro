# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += extension_system
QTILITIES += project_management
include(../../Qtilities.pri)

TARGET = ObjectManagementExample
TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/ObjectManagement

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/ObjectManagementExample
MOC_DIR         = $$QTILITIES_TEMP/ObjectManagementExample
RCC_DIR         = $$QTILITIES_TEMP/ObjectManagementExample
UI_DIR          = $$QTILITIES_TEMP/ObjectManagementExample

# --------------------------
# Application Files
# --------------------------
HEADERS += ObjectManagementModeWidget.h \
    ObjectManagementMode.h

SOURCES += main.cpp \
    ObjectManagementModeWidget.cpp \
    ObjectManagementMode.cpp

FORMS += ObjectManagementModeWidget.ui

RESOURCES += resources.qrc
RC_FILE = rc_file.rc

