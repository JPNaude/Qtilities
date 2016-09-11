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
SUBDIRS += \
    DebugPlugin \
    PluginTemplate \
    ProjectManagementPlugin \
    SessionLogPlugin

contains(DEFINES, QTILITIES_NO_HELP) {
    # Do nothing here for now.
} else {
    greaterThan(QT_MAJOR_VERSION, 4) {
        #QT += help
    }
    lessThan(QT_MAJOR_VERSION, 5) {
        #CONFIG  += help
    }

    unix {
        #SUBDIRS += HelpPlugin
    }

    win32-g++ {
        # Qt WebEngine uses the Chromium browser. Unfortunately, Chromium only
        # supports MSVC 2013, so on Windows the HelpPlugin is only supported
        # with Qt for MSVC 2013.
    }

    win32-msvc* {
        #SUBDIRS += HelpPlugin
    }
}

