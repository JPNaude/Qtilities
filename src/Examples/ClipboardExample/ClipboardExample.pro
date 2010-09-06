# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
# See http://www.qtilities.org/licensing.html for licensing details.
# ***************************************************************************
QTILITIES += coregui
include(../../Qtilities.pri)
QT += core
QT += gui
TARGET = ClipboardExample
CONFIG -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/ClipboardExample

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR = $$QTILITIES_TEMP/ClipboardExample
MOC_DIR = $$QTILITIES_TEMP/ClipboardExample
RCC_DIR = $$QTILITIES_TEMP/ClipboardExample
UI_DIR = $$QTILITIES_TEMP/ClipboardExample

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp \
    ObserverWidgetConfig.cpp \
    ExtendedObserverTreeModel.cpp \
    ExtendedObserverTableModel.cpp
RC_FILE = rc_file.rc
HEADERS += ObserverWidgetConfig.h \
    ExtendedObserverTreeModel.h \
    ExtendedObserverTableModel.h
FORMS += ObserverWidgetConfig.ui
