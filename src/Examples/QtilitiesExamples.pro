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

# Example plugins:
SUBDIRS    += PluginTemplate
SUBDIRS    += SessionLogPlugin
SUBDIRS    += ProjectManagementPlugin

# Example applications:
SUBDIRS    += ConsoleLogging
SUBDIRS    += ObjectManagement
SUBDIRS    += BuildingTreeStructures
SUBDIRS	   += ObserverWidgetExample
SUBDIRS	   += MainWindowExample
