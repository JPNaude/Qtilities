# ***************************************************************************
# Copyright (c) 2009-2012, Jaco Naude
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
QTILITIES_FILE_VER      = -1.2

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
#   When defined, verbose action management messages are printed.
#   DEFINES += QTILITIES_VERBOSE_ACTION_DEBUGGING
#
#   When defined, the Testing library contains the Qtilities
#   unit tests in it.
#   DEFINES += QTILITIES_TESTING
#
#   When defined, basic benchmarking of potentially long operations in
#   Qtilities is done by logging execution times as debug messages.
#   DEFINES += QTILITIES_BENCHMARKING
#
#   When defined, the CoreGui library does not contain the HELP_MANAGER,
#   removing the dependency on QtHelp. Also, the Help Plugin does not
#   contain anything when defined.
#   DEFINES += QTILITIES_NO_HELP
#****************************************************************************

#****************************************************************************
# Include paths
#****************************************************************************
INCLUDEPATH         +=$$QTILITIES_INCLUDE
CONFIG              += ordered qt

#****************************************************************************
# Build mode
#****************************************************************************
CONFIG              += debug

#****************************************************************************
# Library includes
#****************************************************************************
contains( QTILITIES, logging ) {
    message( "Using Qtilities Logging Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging

    CONFIG(debug, debug|release) {
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLoggingd$${QTILITIES_FILE_VER}.lib
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
    } else {
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLogging$${QTILITIES_FILE_VER}.lib
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLogging$${QTILITIES_FILE_VER}
    }
}

contains( QTILITIES, core ) {
    message( "Using Qtilities Core Module..." )
    message( "Core Dependency: Using Qtilities Logging Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore

    CONFIG(debug, debug|release) {
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLoggingd$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCored$${QTILITIES_FILE_VER}.lib
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCored$${QTILITIES_FILE_VER}
    } else {
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLogging$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCore$${QTILITIES_FILE_VER}.lib
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
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLoggingd$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCored$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCoreGuid$${QTILITIES_FILE_VER}.lib
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCored$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGuid$${QTILITIES_FILE_VER}
    } else {
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLogging$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCore$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCoreGui$${QTILITIES_FILE_VER}.lib
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
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLoggingd$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCored$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCoreGuid$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesExtensionSystemd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesExtensionSystemd$${QTILITIES_FILE_VER}.lib
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCored$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGuid$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesExtensionSystemd$${QTILITIES_FILE_VER}
    } else {
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLogging$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCore$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCoreGui$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesExtensionSystem$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesExtensionSystem$${QTILITIES_FILE_VER}.lib
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
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLoggingd$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCored$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCoreGuid$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesProjectManagementd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesProjectManagementd$${QTILITIES_FILE_VER}.lib
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCored$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGuid$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesProjectManagementd$${QTILITIES_FILE_VER}
    } else {
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLogging$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCore$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCoreGui$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesProjectManagement$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesProjectManagement$${QTILITIES_FILE_VER}.lib
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLogging$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCore$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGui$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesProjectManagement$${QTILITIES_FILE_VER}
    }
}

contains( QTILITIES, testing ) {
    message( "Using Testing Module..." )
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
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesTesting

    CONFIG(debug, debug|release) {
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLoggingd$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCored$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCoreGuid$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesProjectManagementd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesProjectManagementd$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesExtensionSystemd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesExtensionSystemd$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesTestingd$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesTestingd$${QTILITIES_FILE_VER}.lib
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLoggingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCored$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGuid$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesProjectManagementd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesTestingd$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesExtensionSystemd$${QTILITIES_FILE_VER}
    } else {
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesLogging$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCore$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesCoreGui$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesProjectManagement$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesProjectManagement$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesExtensionSystem$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesExtensionSystem$${QTILITIES_FILE_VER}.lib
        win32::win32-g++:LIBS += $$QTILITIES_BIN/libQtilitiesTesting$${QTILITIES_FILE_VER}.a
        win32::win32-msvc*:LIBS += $$QTILITIES_BIN/QtilitiesTesting$${QTILITIES_FILE_VER}.lib
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesLogging$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCore$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGui$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesProjectManagement$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesTesting$${QTILITIES_FILE_VER}
        unix::LIBS += -L$$QTILITIES_BIN -lQtilitiesExtensionSystem$${QTILITIES_FILE_VER}
    }
}