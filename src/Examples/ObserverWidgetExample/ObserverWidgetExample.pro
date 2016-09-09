# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += coregui
include(../../Qtilities.pri)
include(../../Dependencies.pri)

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport
}

TARGET = ObserverWidgetExample
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/ObserverWidgetExample

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/ObserverWidgetExample
MOC_DIR         = $$QTILITIES_TEMP/ObserverWidgetExample
RCC_DIR         = $$QTILITIES_TEMP/ObserverWidgetExample
UI_DIR          = $$QTILITIES_TEMP/ObserverWidgetExample

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp \
    AvailablePropertyProvider.cpp \
    ExampleObject.cpp
RC_FILE = rc_file.rc

OTHER_FILES +=

HEADERS += \
    AvailablePropertyProvider.h \
    ExampleObject.h
