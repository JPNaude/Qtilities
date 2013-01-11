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
QT += network
QT += webkit

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport \
      help
}
lessThan(QT_MAJOR_VERSION, 5) {
CONFIG  += help
}

TARGET = MainWindowExample
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/MainWindowExample

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/MainWindowExample
MOC_DIR         = $$QTILITIES_TEMP/MainWindowExample
RCC_DIR         = $$QTILITIES_TEMP/MainWindowExample
UI_DIR          = $$QTILITIES_TEMP/MainWindowExample

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp \
        ExampleMode.cpp

HEADERS += ExampleMode.h

FORMS   += ExampleMode.ui

RC_FILE = rc_file.rc

