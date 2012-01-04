# ***************************************************************************
# Copyright (c) 2009-2012, Jaco Naude
# See http://www.qtilities.org/licensing.html for licensing details.
# ***************************************************************************
QTILITIES += coregui
include(../../Qtilities.pri)
QT += core
QT += gui
TARGET = TheBasicsExample
CONFIG -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/TheBasicsExample

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR = $$QTILITIES_TEMP/TheBasicsExample
MOC_DIR = $$QTILITIES_TEMP/TheBasicsExample
RCC_DIR = $$QTILITIES_TEMP/TheBasicsExample
UI_DIR = $$QTILITIES_TEMP/TheBasicsExample

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp
RC_FILE = rc_file.rc
