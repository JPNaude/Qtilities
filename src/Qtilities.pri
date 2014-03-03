# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
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

#****************************************************************************
# Version Definitions
#****************************************************************************
QTILITIES_VERSION_MAJOR    = 1
QTILITIES_VERSION_MINOR    = 5
QTILITIES_VERSION_REVISION = 0
DEFINES                += "QTILITIES_VERSION_MAJOR=$$QTILITIES_VERSION_MAJOR"
DEFINES                += "QTILITIES_VERSION_MINOR=$$QTILITIES_VERSION_MINOR"
DEFINES                += "QTILITIES_VERSION_REVISION=$$QTILITIES_VERSION_REVISION"
QTILITIES_FILE_VER      = -$${QTILITIES_VERSION_MAJOR}.$${QTILITIES_VERSION_MINOR}

#****************************************************************************
# Library Postfix
#****************************************************************************
win32 {
    CONFIG(debug, debug|release) {
        QTILITIES_LIB_POSTFIX = d$${QTILITIES_FILE_VER}
    } else {
        QTILITIES_LIB_POSTFIX = $${QTILITIES_FILE_VER}
    }
}
unix {
    QTILITIES_LIB_POSTFIX = $${QTILITIES_FILE_VER}
}

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
DEFINES += QTILITIES_NO_HELP
#****************************************************************************

#****************************************************************************
# Include paths
#****************************************************************************
INCLUDEPATH         +=$$QTILITIES_INCLUDE
CONFIG              += ordered qt
CONFIG              += exceptions rtti

#****************************************************************************
# Compiler directives
#****************************************************************************
# Turns on the parallellised build switch for msvc:
# See https://github.com/JPNaude/Qtilities/issues/91 for more details.
win32-msvc* {
    CONFIG += msvc_mp

    # The following makes sure .pdb files are generated in release mode in
    # order to debug stack traces in release mode. Comment this if not desired.
    QMAKE_LFLAGS_RELEASE += /MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE += /debug /opt:ref
}

#****************************************************************************
# Library includes
#****************************************************************************

contains( QTILITIES, testing ) {
    #message( "Using Testing Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesTesting
    LIBS += -L$$QTILITIES_BIN -lQtilitiesTesting$${QTILITIES_LIB_POSTFIX}
    
    QTILITIES += logging
    QTILITIES += core
    QTILITIES += coregui
    QTILITIES += extension_system
    QTILITIES += project_management
}

contains( QTILITIES, project_management ) {
    #message( "Using Qtilities Project Management Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesProjectManagement
    LIBS += -L$$QTILITIES_BIN -lQtilitiesProjectManagement$${QTILITIES_LIB_POSTFIX}

    QTILITIES += logging
    QTILITIES += core
    QTILITIES += coregui
}

contains( QTILITIES, extension_system ) {
    #message( "Using Qtilities Extension System Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesExtensionSystem
    LIBS += -L$$QTILITIES_BIN -lQtilitiesExtensionSystem$${QTILITIES_LIB_POSTFIX}

    QTILITIES += logging
    QTILITIES += core
    QTILITIES += coregui
}

contains( QTILITIES, coregui ) {
    #message( "Using Qtilities Core Gui Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCoreGui
    LIBS += -L$$QTILITIES_BIN -lQtilitiesCoreGui$${QTILITIES_LIB_POSTFIX}

    QTILITIES += logging
    QTILITIES += core
}

contains( QTILITIES, core ) {
    #message( "Using Qtilities Core Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore
    LIBS += -L$$QTILITIES_BIN -lQtilitiesCore$${QTILITIES_LIB_POSTFIX}

    QTILITIES += logging
}

contains( QTILITIES, logging ) {
    #message( "Using Qtilities Logging Module..." )
    INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesLogging
    LIBS += -L$$QTILITIES_BIN -lQtilitiesLogging$${QTILITIES_LIB_POSTFIX}

}
