# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
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
SUBDIRS     = Logging Core CoreGui ExtensionSystem ProjectManagement Testing

Core.depends              = Logging
CoreGui.depends           = Logging \
                            Core
ExtensionSystem.depends   = Logging \
                            Core \
                            CoreGui
ProjectManagement.depends = Logging \
                            Core \
                            CoreGui
Testing.depends           = Logging \
                            Core \
                            CoreGui \
                            ExtensionSystem \
                            ProjectManagement

TRANSLATIONS = qtilities.ts

OTHER_FILES += ../doc/sources/Action_Management.html \
    ../Readme.md \
    ../doc/sources/Whats_New_v1_0.html \
    ../doc/sources/Whats_New_v1_1.html \
    ../doc/sources/Whats_New_v1_2.html \
    ../doc/sources/Whats_New_v1_3.html \
    ../doc/sources/Whats_New_v1_4.html \
    ../doc/sources/Documentation_Overview.html \
    ../doc/sources/Examples_And_Plugins.html \
    ../doc/sources/Extension_System.html \
    ../doc/sources/Factories.html \
    ../doc/sources/Footer.html \
    ../doc/sources/Header.html \
    ../doc/sources/FooterWithTracking.html \
    ../doc/sources/Logging.html \
    ../doc/sources/Modules_Overview.html \
    ../doc/sources/Object_Management.html \
    ../doc/sources/Observer_Widgets.html \
    ../doc/sources/Observers.html \
    ../doc/sources/Project_Management.html \
    ../doc/sources/The_Basics.html \
    ../doc/sources/Tree_Structures.html \
    ../doc/sources/Widget_Set.html \
    ../doc/sources/Licensing.html \
    ../doc/sources/Contribute.html \
    ../doc/sources/Serializing_Overview.html \
    ../doc/sources/Serializing_Types_1_0.html \
    ../doc/sources/Serializing_Types_1_1.html \
    ../doc/sources/Serializing_Types_1_2.html \
    ../doc/sources/Tasking.html \
    ../doc/sources/Debugging_Qtilities_Applications.html \
    ../Changelog.txt \
    ../doc/doxyfile_packages \
    ../doc/doxyfile_website \
    ../doc/doxyfile_website_with_tracking
#OTHER_FILES = ../doc/win32_packages.bat
#OTHER_FILES = ../doc/win32_packages_gen_qch.bat
#OTHER_FILES = ../doc/win32_website.bat
#OTHER_FILES = ../doc/win32_website_with_tracking.bat
#OTHER_FILES = ../doc/Qtilities.qch

includes.path = /usr/local/include/Qtilities
includes.files = $$PWD/../include/*
libs.path = /usr/local/lib
libs.files = $$PWD/../bin/*
INSTALLS += includes libs
