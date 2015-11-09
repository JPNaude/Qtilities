# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Project Management Library
# ***************************************************************************
QTILITIES += coregui
include(../Qtilities.pri)
INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesProjectManagement

CONFIG += qt dll
QT += xml gui
# Note: xml module is deprecated

greaterThan(QT_MAJOR_VERSION, 4) { QT += widgets printsupport }

TARGET = QtilitiesProjectManagement$${QTILITIES_LIB_POSTFIX}

TEMPLATE = lib
DEFINES += PROJECT_MANAGEMENT_LIBRARY
DESTDIR = $$QTILITIES_BIN
OBJECTS_DIR = $$QTILITIES_TEMP/ProjectManagement
MOC_DIR = $$QTILITIES_TEMP/ProjectManagement
RCC_DIR = $$QTILITIES_TEMP/ProjectManagement
UI_DIR = $$QTILITIES_TEMP/ProjectManagement

# --------------------------
# Library Files
# --------------------------
HEADERS += source/ProjectManagement_global.h \
    source/ProjectManagementConstants.h \
    source/ProjectManager.h \
    source/IProject.h \
    source/IProjectItem.h \
    source/Project.h \
    source/ProjectManagementConfig.h \
    source/ObserverProjectItemWrapper.h \
    source/CodeEditorProjectItemWrapper.h \
    source/ProjectsBrowser.h
SOURCES += source/ProjectManager.cpp \
    source/Project.cpp \
    source/ProjectManagementConfig.cpp \
    source/ObserverProjectItemWrapper.cpp \
    source/CodeEditorProjectItemWrapper.cpp \
    source/ProjectsBrowser.cpp
FORMS += source/ProjectManagementConfig.ui \
    source/ProjectsBrowser.ui
