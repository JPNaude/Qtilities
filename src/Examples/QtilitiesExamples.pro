# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
#
# Qtilities Libraries Examples
#
#****************************************************************************

TARGET      = Qtilities
TEMPLATE    = subdirs
CONFIG     += ordered
DEFINES += QTILITIES_TESTING

# Example Applications:
SUBDIRS += \
    BuildingTreeStructures \
    ClipboardExample \
    ConsoleLogging \
    ExportingExample \
    MainWindowExample \
    ObjectManagement \
    ObserverWidgetExample \
    PropertiesExample \
    TasksExample \
    TheBasicsExample \

# The tcl scripting example depends on Tcl, thus its commented out by default.
#SUBDIRS    += TclScriptingExample
