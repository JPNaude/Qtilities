# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += coregui
include(../../Qtilities.pri)

QT       += core
QT       += gui

TARGET = BuildingTreeStructures$${name}
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/BuildingTreeStructures

# ------------------------------
# Qtilities Library Dependancies
# ------------------------------
equals(BUILD_MODE, release) {
    LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
    LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
} else {
    LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
    LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
}

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/BuildingTreeStructures
MOC_DIR         = $$QTILITIES_TEMP/BuildingTreeStructures
RCC_DIR         = $$QTILITIES_TEMP/BuildingTreeStructures
UI_DIR          = $$QTILITIES_TEMP/BuildingTreeStructures

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp
RC_FILE = rc_file.rc
