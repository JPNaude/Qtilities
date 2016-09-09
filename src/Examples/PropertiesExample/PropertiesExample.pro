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

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport
}

TARGET = PropertiesExample
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/PropertiesExample

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/PropertiesExample
MOC_DIR         = $$QTILITIES_TEMP/PropertiesExample
RCC_DIR         = $$QTILITIES_TEMP/PropertiesExample
UI_DIR          = $$QTILITIES_TEMP/PropertiesExample

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp

RC_FILE = rc_file.rc

