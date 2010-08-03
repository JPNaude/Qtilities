# ***************************************************************************
# Copyright 2010, Jaco Naude
# ***************************************************************************
# Qtilities Configuration Library
# ***************************************************************************
include(../Qtilities.pri)

CONFIG += qt \
    dll \
    ordered

QT += core
QT -= gui

TARGET = QtilitiesConfig
TEMPLATE = lib
DEFINES += CONFIG_LIBRARY
DESTDIR = $$QTILITIES_BIN

OBJECTS_DIR = $$QTILITIES_BASE/libraries/tmp/Config
MOC_DIR = $$QTILITIES_BASE/libraries/tmp/Config
RCC_DIR = $$QTILITIES_BASE/libraries/tmp/Config
UI_DIR = $$QTILITIES_BASE/libraries/tmp/Config

# ------------------------------
# Qtilities Library Dependancies
# ------------------------------
LIBS += $$QTILITIES_BIN/libQtilitiesLogging.a

# --------------------------
# Library Files
# --------------------------
HEADERS += source/Config_global.h \
    source/ConfigConstants.h \
    source/EditorTypesConfigWrapper.h \
    source/EnvironmentOptionsConfigWrapper.h \
    source/ScriptBackendTypesConfigWrapper.h \
    source/ScriptingOptionsConfigWrapper.h \
    source/ShortcutsConfigWrapper.h

SOURCES += source/EditorTypesConfigWrapper.cpp \
    source/EnvironmentOptionsConfigWrapper.cpp \
    source/ScriptBackendTypesConfigWrapper.cpp \
    source/ScriptingOptionsConfigWrapper.cpp \
    source/ShortcutsConfigWrapper.cpp
