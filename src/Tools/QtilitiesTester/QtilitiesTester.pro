# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += coregui
QTILITIES += testing
include(../../Qtilities.pri)

QT       += core
QT       += gui
QT       += xml
CONFIG += qtestlib

TARGET    = QtilitiesTester
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/QtilitiesTester

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/QtilitiesTester
MOC_DIR         = $$QTILITIES_TEMP/QtilitiesTester
RCC_DIR         = $$QTILITIES_TEMP/QtilitiesTester
UI_DIR          = $$QTILITIES_TEMP/QtilitiesTester

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp
RC_FILE = rc_file.rc

