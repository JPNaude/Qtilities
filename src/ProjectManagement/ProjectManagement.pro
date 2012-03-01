# ***************************************************************************
# Copyright (c) 2009-2012, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Project Management Library
# ***************************************************************************
QTILITIES += coregui
include(../Qtilities.pri)
INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesProjectManagement

CONFIG += qt \
    dll \
    ordered

QT += core
QT += xml

CONFIG(debug, debug|release) {
    TARGET = QtilitiesProjectManagementd$${QTILITIES_FILE_VER}
} else {
    TARGET = QtilitiesProjectManagement$${QTILITIES_FILE_VER}
}

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
