# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities Unit Tests Library
# ***************************************************************************
QTILITIES += coregui
include(../Qtilities.pri)
INCLUDEPATH += $$QTILITIES_INCLUDE/UnitTests

CONFIG += qt \
    dll \
    ordered

QT += core
CONFIG += qtestlib

CONFIG(debug, debug|release) {
    TARGET = QtilitiesUnitTestsd$${QTILITIES_FILE_VER}
} else {
    TARGET = QtilitiesUnitTests$${QTILITIES_FILE_VER}
}

TEMPLATE = lib

DEFINES += UNIT_TESTS_LIBRARY
DESTDIR = $$QTILITIES_BIN

OBJECTS_DIR = $$QTILITIES_TEMP/UnitTests
MOC_DIR = $$QTILITIES_TEMP/UnitTests
RCC_DIR = $$QTILITIES_TEMP/UnitTests
UI_DIR = $$QTILITIES_TEMP/UnitTests

# --------------------------
# Extension Library Files
# --------------------------
HEADERS += source/UnitTestsConstants.h \
        source/UnitTests_global.h \
        source/TestObserver.h
SOURCES += source/TestObserver.cpp

