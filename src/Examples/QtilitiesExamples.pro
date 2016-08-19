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
SUBDIRS    += ConsoleLogging
SUBDIRS    += ObjectManagement
SUBDIRS    += BuildingTreeStructures
SUBDIRS	   += ObserverWidgetExample
SUBDIRS	   += MainWindowExample
SUBDIRS	   += ClipboardExample
SUBDIRS	   += TheBasicsExample
SUBDIRS	   += ExportingExample
SUBDIRS	   += TasksExample
SUBDIRS	   += PropertiesExample

# The tcl scripting example depends on Tcl, thus its commented out by default.
#SUBDIRS    += TclScriptingExample
