# ***************************************************************************
# Copyright (c) 2009-2011, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
#
# Qtilities Libraries
#
#****************************************************************************

TARGET      = Qtilities
TEMPLATE    = subdirs
CONFIG     += ordered
SUBDIRS    += Logging
SUBDIRS    += Core
SUBDIRS    += CoreGui
SUBDIRS    += ExtensionSystem
SUBDIRS    += ProjectManagement
SUBDIRS    += UnitTests

OTHER_FILES += ../doc/sources/Action_Management.html \
    ../doc/sources/Serializing_Types_1_0.html
OTHER_FILES += ../doc/sources/Documentation_Overview.html
OTHER_FILES += ../doc/sources/Examples_And_Plugins.html
OTHER_FILES += ../doc/sources/Extension_System.html
OTHER_FILES += ../doc/sources/Factories.html
OTHER_FILES += ../doc/sources/Footer.html
OTHER_FILES += ../doc/sources/FooterWithTracking.html
OTHER_FILES += ../doc/sources/Getting_Started.html
OTHER_FILES += ../doc/sources/Logging.html
OTHER_FILES += ../doc/sources/Modules_Overview.html
OTHER_FILES += ../doc/sources/Object_Management.html
OTHER_FILES += ../doc/sources/Observer_Widgets.html
OTHER_FILES += ../doc/sources/Observers.html
OTHER_FILES += ../doc/sources/Project_Management.html
OTHER_FILES += ../doc/sources/The_Basics.html
OTHER_FILES += ../doc/sources/Tree_Structures.html
OTHER_FILES += ../doc/sources/Widget_Set.html
OTHER_FILES += ../doc/sources/Serializing_Overview.html
OTHER_FILES +=
OTHER_FILES += ../doc/sources/Whats_New.html
OTHER_FILES += ../doc/sources/Debugging_Qtilities_Applications.html
OTHER_FILES += ../Read Me First.txt
OTHER_FILES += ../Changelog.txt
OTHER_FILES += ../Building On Linux.txt
OTHER_FILES += ../Building On Windows.txt
OTHER_FILES += ../doc/doxyfile_packages.txt
OTHER_FILES += ../doc/doxyfile_website.txt
