# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += testing
DEFINES += QTILITIES_TESTING
include(../../Qtilities.pri)

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport \
      testlib
}
lessThan(QT_MAJOR_VERSION, 5) {
    CONFIG += qtestlib
}

TARGET    = QtilitiesTester
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Tools/QtilitiesTester

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

