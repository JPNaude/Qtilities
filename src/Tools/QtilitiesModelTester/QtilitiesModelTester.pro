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

TARGET    = QtilitiesModelTester
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Tools/QtilitiesModelTester

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/QtilitiesModelTester
MOC_DIR         = $$QTILITIES_TEMP/QtilitiesModelTester
RCC_DIR         = $$QTILITIES_TEMP/QtilitiesModelTester
UI_DIR          = $$QTILITIES_TEMP/QtilitiesModelTester

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp
RC_FILE = rc_file.rc

SOURCES         += modeltest.cpp dynamictreemodel.cpp
HEADERS         += modeltest.h dynamictreemodel.h

