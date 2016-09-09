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
QT += core xml gui
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
HEADERS += \
    source/CodeEditorProjectItemWrapper.h \
    source/IProject.h \
    source/IProjectItem.h \
    source/ObserverProjectItemWrapper.h \
    source/Project.h \
    source/ProjectManagementConfig.h \
    source/ProjectManagementConstants.h \
    source/ProjectManagement_global.h \
    source/ProjectManager.h \
    source/ProjectsBrowser.h \

SOURCES += \
    source/CodeEditorProjectItemWrapper.cpp \
    source/ObserverProjectItemWrapper.cpp \
    source/Project.cpp \
    source/ProjectManagementConfig.cpp \
    source/ProjectManager.cpp \
    source/ProjectsBrowser.cpp \

FORMS += \
    source/ProjectManagementConfig.ui \
    source/ProjectsBrowser.ui \
