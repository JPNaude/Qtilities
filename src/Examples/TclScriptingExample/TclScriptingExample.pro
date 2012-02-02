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

TARGET    = TclScriptingExample
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/TclScriptingExample

# ------------------------------
# Tcl Stuff
# ------------------------------
INCLUDEPATH += . include "D:/Tools/Tcl/include"
LIBS += -L"D:/Tools/Tcl/lib" -ltcl85

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/TclScriptingExample
MOC_DIR         = $$QTILITIES_TEMP/TclScriptingExample
RCC_DIR         = $$QTILITIES_TEMP/TclScriptingExample
UI_DIR          = $$QTILITIES_TEMP/TclScriptingExample

# --------------------------
# Application Files
# --------------------------
SOURCES += main.cpp \
    TclScriptingMode.cpp \
    tclnotify.cpp \
    qtclconsole.cpp \
    qconsole.cpp \
    commandsManager.cpp \
    commands.cpp
RC_FILE = rc_file.rc

HEADERS += \
    TclScriptingMode.h \
    qtclconsole.h \
    qconsole.h \
    commandsManager.h \
    commands.h \
    tclnotify.h

FORMS += \
    TclScriptingMode.ui
