# ***************************************************************************
# Copyright (c) 2009-2011, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
#
# Qtilities - Qt Utility Libraries
# Dependencies Config PRI File
#
# ***************************************************************************

#****************************************************************************
# Set up dependencies paths:
#
# By default Qtilities assumes that you have a folder where all your
# libraries are placed, including Qtilities. It assumes you have a folder
# for Qtilities with a trunk or specific version which you attempt to build.
# Thus all dependencies are expected two levels up from the Qtilities
# build directory.
#****************************************************************************
QTILITIES_DEPENDENCIES  = $$PWD/../../../

# --------------------------
# Qt Property Browser Solution
# --------------------------
# http://qt.gitorious.org/qt-solutions
# Tested with v2.6.
# --------------------------
# Builds Qtilities without the ObjectPropertyBrowser and
# ObjectDynamicPropertyBrowser classes, thus you
# don't need the Property Browser Solution.
# DEFINES += QTILITIES_NO_PROPERTY_BROWSER
# --------------------------
!contains(DEFINES, QTILITIES_NO_PROPERTY_BROWSER) {
    DEFINES += QT_QTPROPERTYBROWSER_IMPORT
    QT_SOLUTIONS_PATH = $$QTILITIES_DEPENDENCIES/qt-solutions
    PROPERTY_EDITOR_BASE = $$QT_SOLUTIONS_PATH/qtpropertybrowser
    DEPENDPATH += $$PROPERTY_EDITOR_BASE/src
    INCLUDEPATH += $$PROPERTY_EDITOR_BASE/src
    CONFIG(debug, debug|release) {
        win32:LIBS += $$PROPERTY_EDITOR_BASE/lib/libQtSolutions_PropertyBrowser-headd.a
        unix::LIBS += -L$$PROPERTY_EDITOR_BASE/lib \
            -lQtSolutions_PropertyBrowser-head
    }
    else {
        win32:LIBS += $$PROPERTY_EDITOR_BASE/lib/libQtSolutions_PropertyBrowser-head.a
        unix::LIBS += -L$$PROPERTY_EDITOR_BASE/lib \
            -lQtSolutions_PropertyBrowser-head
    }
}

# --------------------------
# Conan
# --------------------------
# https://sourceforge.net/projects/conanforqt/
# Requires at least, and tested with Conan v1.0.2.
#
# Integrates the Conan Library into the debug widget:
# --------------------------
# Builds Qtilities without Conan library integrated into the debug plugin.
# Note that this integration was only tested in Qt 4.7.x on Windows XP 32bit.
DEFINES += QTILITIES_NO_CONAN
# --------------------------

!contains(DEFINES, QTILITIES_NO_CONAN) {
    CONAN_BASE = $$QTILITIES_DEPENDENCIES/conanforqt/Conan
    INCLUDEPATH += $$CONAN_BASE/include

    CONFIG(debug, debug|release) {
        win32:LIBS += $$CONAN_BASE/lib/libConand1.a
        unix::LIBS += -L$$CONAN_BASE/lib -lConand
    }
    else {
        win32:LIBS += $$CONAN_BASE/lib/libConan1.a
        unix::LIBS += -L$$CONAN_BASE/lib -lConan
    }
}
