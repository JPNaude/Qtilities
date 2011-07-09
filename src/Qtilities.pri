# ***************************************************************************
# Copyright (c) 2009-2011, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
#
# Qtilities - Qt Utility Libraries
# Config PRI File
#
# ***************************************************************************

#****************************************************************************
# Project paths
#****************************************************************************
QTILITIES_BASE          = $$PWD/../
QTILITIES_SOURCE        = $$QTILITIES_BASE/src
QTILITIES_INCLUDE       = $$QTILITIES_BASE/include
QTILITIES_TEMP          = $$QTILITIES_BASE/build_files/tmp
QTILITIES_BIN           = $$QTILITIES_BASE/bin
QTILITIES_DEPENDENCIES  = $$QTILITIES_BASE/dependencies
QTILITIES_FILE_VER      = -1.0

#****************************************************************************
# Build Options:
#   *************************************************************************
#   Dependencies:
#   *************************************************************************
#   See Dependencies.pri for dependency configuration.
include(Dependencies.pri)
#
#   *************************************************************************
#   General:
#   *************************************************************************
#   When uncommented, verbose action management messages are printed.
#   DEFINES += QTILITIES_VERBOSE_ACTION_DEBUGGING
#   When uncommented, the UnitTests library contains the Qtilities
#   unit tests in it.
   DEFINES += QTILITIES_UNIT_TESTS
#   When uncommented, basic benchmarking of potentially long operations in
#   Qtilities is done by logging execution times as warning messages.
#   DEFINES += QTILITIES_BENCHMARKING
#****************************************************************************

#****************************************************************************
# Include paths
#****************************************************************************
INCLUDEPATH         +=$$QTILITIES_INCLUDE
CONFIG              += ordered qt

#****************************************************************************
# Build mode:
#****************************************************************************
CONFIG              += debug

#****************************************************************************
# Library includes
#****************************************************************************
contains( QTILITIES, logging ) {
    message( "Using Qtilities Logging Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging

    CONFIG(debug, debug|release) {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
    } else {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLogging$${QTILITIES_FILE_VER}
    }
}

contains( QTILITIES, core ) {
    message( "Using Qtilities Core Module..." )
    message( "Core Dependency: Using Qtilities Logging Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore

    CONFIG(debug, debug|release) {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCored$${QTILITIES_FILE_VER}
    } else {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLogging$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCore$${QTILITIES_FILE_VER}
    }
}

contains( QTILITIES, coregui ) {
    message( "Using Qtilities Core Gui Module..." )
    message( "CoreGui Dependency: Using Qtilities Logging Module..." )
    message( "CoreGui Dependency: Using Qtilities Core Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCoreGui

    CONFIG(debug, debug|release) {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCored$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGuid$${QTILITIES_FILE_VER}
    } else {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLogging$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCore$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGui$${QTILITIES_FILE_VER}
    }
}

contains( QTILITIES, extension_system ) {
    message( "Using Qtilities Extension System Module..." )
    message( "Extension System Dependency: Using Qtilities Logging Module..." )
    message( "Extension System Dependency: Using Qtilities Core Module..." )
    message( "Extension System Dependency: Using Qtilities CoreGui Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCoreGui
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesExtensionSystem

    CONFIG(debug, debug|release) {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesExtensionSystemd$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCored$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGuid$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesExtensionSystemd$${QTILITIES_FILE_VER}
    } else {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesExtensionSystem$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLogging$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCore$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGui$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesExtensionSystem$${QTILITIES_FILE_VER}
    }
}

contains( QTILITIES, project_management ) {
    message( "Using Qtilities Project Management Module..." )
    message( "Project Management Dependency: Using Qtilities Logging Module..." )
    message( "Project Management Dependency: Using Qtilities Core Module..." )
    message( "Project Management Dependency: Using Qtilities CoreGui Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCoreGui
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesProjectManagement

    CONFIG(debug, debug|release) {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesProjectManagementd$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCored$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGuid$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesProjectManagementd$${QTILITIES_FILE_VER}
    } else {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesProjectManagement$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLogging$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCore$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGui$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesProjectManagement$${QTILITIES_FILE_VER}
    }
}

contains( QTILITIES, unit_tests ) {
    message( "Using Unit Testing Module..." )
    message( "Project Management Dependency: Using Qtilities Logging Module..." )
    message( "Project Management Dependency: Using Qtilities Core Module..." )
    message( "Project Management Dependency: Using Qtilities CoreGui Module..." )
    message( "Project Management Dependency: Using Qtilities Project Management Module..." )
    message( "Project Management Dependency: Using Qtilities Extension System Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCoreGui
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesProjectManagement
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesExtensionSystem
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesUnitTests

    CONFIG(debug, debug|release) {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesUnitTestsd$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesProjectManagementd$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesExtensionSystemd$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCored$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGuid$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesProjectManagementd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesUnitTestsd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesExtensionSystemd$${QTILITIES_FILE_VER}
    } else {
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesUnitTests$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesProjectManagement$${QTILITIES_FILE_VER}.a
        win32::LIBS += $$QTILITIES_BIN/libQtilitiesExtensionSystem$${QTILITIES_FILE_VER}.a
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLogging$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCore$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGui$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesProjectManagement$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesUnitTests$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesExtensionSystem$${QTILITIES_FILE_VER}
    }
}
