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
    HelpPlugin \
    PluginTemplate \
    ProjectManagementPlugin \
    SessionLogPlugin \

