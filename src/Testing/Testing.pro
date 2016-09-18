# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
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

CONFIG += qt dll
QT += xml gui
# Note: xml module is deprecated
greaterThan(QT_MAJOR_VERSION, 4) { QT += widgets printsupport testlib }
lessThan(QT_MAJOR_VERSION, 5) { CONFIG += qtestlib }

TARGET = QtilitiesTesting$${QTILITIES_LIB_POSTFIX}

TEMPLATE = lib

DEFINES += TESTING_LIBRARY
DESTDIR = $$QTILITIES_BIN

OBJECTS_DIR = $$QTILITIES_TEMP/Testing
MOC_DIR = $$QTILITIES_TEMP/Testing
RCC_DIR = $$QTILITIES_TEMP/Testing
UI_DIR = $$QTILITIES_TEMP/Testing

# --------------------------
# Files only included when Qtilities unit tests must be part of the library.
# To build them, add "DEFINES+=QTILITIES_TESTING" on the qmake command line.
# --------------------------
contains(DEFINES, QTILITIES_TESTING) {
    HEADERS += \
            source/BenchmarkTests.h \
            source/TestAbstractTreeItem.h \
            source/TestActivityPolicyFilter.h \
            source/TestExporting.h \
            source/TestingConstants.h \
            source/Testing_global.h \
            source/TestNamingPolicyFilter.h \
            source/TestObjectManager.h \
            source/TestObserver.h \
            source/TestObserverRelationalTable.h \
            source/TestSubjectIterator.h \
            source/TestSubjectTypeFilter.h \
            source/TestTask.h \
            source/TestTreeFileItem.h \
            source/TestTreeIterator.h \
            source/TestVersionNumber.h \

    SOURCES += \
            source/BenchmarkTests.cpp \
            source/TestAbstractTreeItem.cpp \
            source/TestActivityPolicyFilter.cpp \
            source/TestExporting.cpp \
            source/TestNamingPolicyFilter.cpp \
            source/TestObjectManager.cpp \
            source/TestObserver.cpp \
            source/TestObserverRelationalTable.cpp \
            source/TestSubjectIterator.cpp \
            source/TestSubjectTypeFilter.cpp \
            source/TestTask.cpp \
            source/TestTreeFileItem.cpp \
            source/TestTreeIterator.cpp \
            source/TestVersionNumber.cpp \
}

# --------------------------
# Extension Library Files
# --------------------------
HEADERS += \
        source/DebugWidget.h \
        source/DropableListWidget.h \
        source/FunctionCallAnalyzer.h \
        source/ITestable.h \
        source/TestFileSetInfo.h \
        source/TestFrontend.h \
        source/TestingConstants.h \
        source/Testing_global.h \

SOURCES += \
        source/DebugWidget.cpp \
        source/DropableListWidget.cpp \
        source/FunctionCallAnalyzer.cpp \
        source/TestFileSetInfo.cpp \
        source/TestFrontend.cpp \

FORMS += \
        source/TestFrontend.ui \
        source/DebugWidget.ui \
