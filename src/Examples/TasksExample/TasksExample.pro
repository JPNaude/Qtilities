# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += extension_system
QTILITIES += project_management
include(../../Qtilities.pri)
include(../../Dependencies.pri)

QT       += core
QT       += gui

TARGET = TasksExample
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/TasksExample

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/TasksExample
MOC_DIR         = $$QTILITIES_TEMP/TasksExample
RCC_DIR         = $$QTILITIES_TEMP/TasksExample
UI_DIR          = $$QTILITIES_TEMP/TasksExample

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp \
        ExampleMode.cpp

HEADERS += ExampleMode.h

FORMS   += ExampleMode.ui

RC_FILE = rc_file.rc

