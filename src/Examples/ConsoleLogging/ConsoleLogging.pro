# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += logging
include(../../Qtilities.pri)

QT       += core xml
QT       -= gui

TARGET = LoggingExample
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/ConsoleLogging

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/ConsoleLoggingExample
MOC_DIR         = $$QTILITIES_TEMP/ConsoleLoggingExample
RCC_DIR         = $$QTILITIES_TEMP/ConsoleLoggingExample
UI_DIR          = $$QTILITIES_TEMP/ConsoleLoggingExample

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp
