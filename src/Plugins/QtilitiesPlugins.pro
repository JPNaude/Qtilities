# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
#
# Qtilities Libraries Plugins
#
#****************************************************************************

TARGET      = Qtilities
TEMPLATE    = subdirs
CONFIG     += ordered

# Plugins:
#SUBDIRS    += PluginTemplate

SUBDIRS += \
    DebugPlugin \
    ProjectManagementPlugin \
    SessionLogPlugin

unix {
#SUBDIRS += HelpPlugin
}

win32-g++ {
# Qt WebEngine uses the Chromium browser. Unfortunately, Chromium only supports MSVC 2013, so on Windows the HelpPlugin is only supported with Qt for MSVC 2013.
}

win32-msvc* {
#SUBDIRS += HelpPlugin
}
