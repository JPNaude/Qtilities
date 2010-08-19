# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
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
QTILITIES_BASE          = D:/Projects/Qt/Qtilities/trunk
QTILITIES_SOURCE        = $$QTILITIES_BASE/src
QTILITIES_INCLUDE       = $$QTILITIES_BASE/include
QTILITIES_TEMP          = $$QTILITIES_BASE/build_files/tmp
QTILITIES_BIN           = $$QTILITIES_BASE/bin
QTILITIES_DEPENDENCIES  = $$QTILITIES_BASE/dependencies
QTILITIES_FILE_VER      = -0.1

#****************************************************************************
# Include paths
#****************************************************************************
INCLUDEPATH         += $$QTILITIES_INCLUDE \
                    += $$QTILITIES_INCLUDE/QtilitiesCoreGui \
                    += $$QTILITIES_INCLUDE/QtilitiesProjectManagement \
                    += $$QTILITIES_INCLUDE/QtilitiesExtensionSystem \
CONFIG              += ordered qt

#****************************************************************************
# Build mode
#****************************************************************************
BUILD_MODE          = debug
CONFIG              += $$BUILD_MODE

#****************************************************************************
# Library includes
#****************************************************************************
contains( QTILITIES, logging ) {
    message( "Using Qtilities Logging module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging

    equals(BUILD_MODE, release) {
        LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
    } else {
        LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
    }
}

contains( QTILITIES, core ) {
    message( "Using Qtilities Core module..." )
    message( "Core Dependency: Using Qtilities Logging module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore

    equals(BUILD_MODE, release) {
        LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
    } else {
        LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
    }
}

contains( QTILITIES, coregui ) {
    message( "Using Qtilities Core Gui module..." )
    message( "CoreGui Dependency: Using Qtilities Logging module..." )
    message( "CoreGui Dependency: Using Qtilities Core module..." )
    message( "CoreGui Dependency: Make sure the Qt Property Editor Solution libraries are in your path..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCoreGui

    equals(BUILD_MODE, release) {
        LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
    } else {
        LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
    }
}

contains( QTILITIES, extension_system ) {
    message( "Using Qtilities Extension System module..." )
    message( "Extension System Dependency: Using Qtilities Logging module..." )
    message( "Extension System Dependency: Using Qtilities Core module..." )
    message( "Extension System Dependency: Using Qtilities CoreGui module..." )
    message( "CoreGui Dependency: Make sure the Qt Property Editor Solution libraries are in your path..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCoreGui
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesExtensionSystem

    equals(BUILD_MODE, release) {
        LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesExtensionSystem$${QTILITIES_FILE_VER}.a
    } else {
        LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesExtensionSystemd$${QTILITIES_FILE_VER}.a
    }
}

contains( QTILITIES, project_management ) {
    message( "Using Qtilities Project module..." )
    message( "Extension System Dependency: Using Qtilities Logging module..." )
    message( "Extension System Dependency: Using Qtilities Core module..." )
    message( "Extension System Dependency: Using Qtilities CoreGui module..." )
    message( "CoreGui Dependency: Make sure the Qt Property Editor Solution libraries are in your path..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCoreGui
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesProjectManagement

    equals(BUILD_MODE, release) {
        LIBS += $$QTILITIES_BIN/libQtilitiesLogging$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCore$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCoreGui$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesProjectManagement$${QTILITIES_FILE_VER}.a
    } else {
        LIBS += $$QTILITIES_BIN/libQtilitiesLoggingd$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCored$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesCoreGuid$${QTILITIES_FILE_VER}.a
        LIBS += $$QTILITIES_BIN/libQtilitiesProjectManagementd$${QTILITIES_FILE_VER}.a
    }
}

