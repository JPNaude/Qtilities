# ***************************************************************************
# Copyright (c) 2009-2012, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Testing Library
# ***************************************************************************
QTILITIES += project_management
QTILITIES += extension_system
include(../Qtilities.pri)
INCLUDEPATH += $$QTILITIES_INCLUDE/Testing

CONFIG += qt \
    dll \
    ordered

QT += xml \
      printsupport \
      gui

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}

CONFIG += qtestlib

TARGET = QtilitiesTesting$${QTILITIES_LIB_POSTFIX}

TEMPLATE = lib

DEFINES += TESTING_LIBRARY
DESTDIR = $$QTILITIES_BIN

OBJECTS_DIR = $$QTILITIES_TEMP/Testing
MOC_DIR = $$QTILITIES_TEMP/Testing
RCC_DIR = $$QTILITIES_TEMP/Testing
UI_DIR = $$QTILITIES_TEMP/Testing

# --------------------------
# Files only included when Qtilities unit tests must be part of the library
# --------------------------
contains(DEFINES, QTILITIES_TESTING) {
HEADERS += source/TestingConstants.h \
        source/Testing_global.h \
        source/TestObserver.h \
        source/TestObserverRelationalTable.h \
        source/TestVersionNumber.h \
        source/TestExporting.h \
        source/TestSubjectIterator.h \
        source/TestTreeIterator.h \
        source/BenchmarkTests.h \
        source/TestNamingPolicyFilter.h \
        source/TestActivityPolicyFilter.h \
        source/TestSubjectTypeFilter.h \
        source/TestTreeFileItem.h \
        source/TestAbstractTreeItem.h \
        source/TestObjectManager.h \
        source/TestTask.h

SOURCES += source/TestObserver.cpp \
        source/TestObserverRelationalTable.cpp \
        source/TestVersionNumber.cpp \
        source/TestExporting.cpp \
        source/TestSubjectIterator.cpp \
        source/TestTreeIterator.cpp \
        source/BenchmarkTests.cpp \
        source/TestNamingPolicyFilter.cpp \
        source/TestActivityPolicyFilter.cpp \
        source/TestSubjectTypeFilter.cpp \
        source/TestTreeFileItem.cpp \
        source/TestAbstractTreeItem.cpp \
        source/TestObjectManager.cpp \
        source/TestTask.cpp
}

# --------------------------
# Extension Library Files
# --------------------------
HEADERS += source/TestingConstants.h \
        source/Testing_global.h \
        source/ITestable.h \
        source/TestFrontend.h \
        source/DebugWidget.h \
        source/DropableListWidget.h \
        source/FunctionCallAnalyzer.h \
    source/TestFileSetInfo.h

SOURCES += source/TestFrontend.cpp \
        source/DebugWidget.cpp \
        source/DropableListWidget.cpp \
        source/FunctionCallAnalyzer.cpp \
    source/TestFileSetInfo.cpp

FORMS += source/TestFrontend.ui \
        source/DebugWidget.ui

