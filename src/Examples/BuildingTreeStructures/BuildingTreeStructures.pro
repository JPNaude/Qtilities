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

TARGET    = BuildingTreeStructures
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/BuildingTreeStructures

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
