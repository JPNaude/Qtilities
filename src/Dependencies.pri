# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
#
# See http://www.qtilities.org/licensing.html for licensing details.
# See the Conan section below for exceptions when enabling Conan Integration
# in the Qtilities Debug Plugin.
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
# build directory.  If you need to change this, you can add
# "-after QTILITIES_DEPENDENCIES=${somewhere_else}" to the qmake build command.
#****************************************************************************
QTILITIES_DEPENDENCIES  = $$PWD/../../

# --------------------------
# Qt Property Browser Solution
# --------------------------
# http://qt.gitorious.org/qt-solutions
# --------------------------
# Builds Qtilities with the following additional classes:
# ObjectPropertyBrowser
# ObjectDynamicPropertyBrowser
# GenericProperty
# GenericPropertyManager
# GenericPropertyBrowser
# They all depend on the Qt Property Browser Solution.
#
# When building Qtilities, you can add "DEFINES+=QTILITIES_PROPERTY_BROWSER" to
# qmake's command-line arguments to enable this integration.
# --------------------------
contains(DEFINES, QTILITIES_PROPERTY_BROWSER) {
    DEFINES += QT_QTPROPERTYBROWSER_IMPORT
    QT_SOLUTIONS_PATH = $$QTILITIES_DEPENDENCIES/qt-solutions
    PROPERTY_EDITOR_BASE = $$QT_SOLUTIONS_PATH/qtpropertybrowser
    DEPENDPATH += $$PROPERTY_EDITOR_BASE/src
    INCLUDEPATH += $$PROPERTY_EDITOR_BASE/src
    CONFIG(debug, debug|release) {
        # Note: Because the property browser uses $$qtLibraryTarget(QtSolutions_PropertyBrowser-head)
        # as its output file name, it ends up with a trailing 'd' on Windows, but not on Unix.
        win32::LIBS += -L$$PROPERTY_EDITOR_BASE/lib -lQtSolutions_PropertyBrowser-headd
        unix::LIBS += -L$$PROPERTY_EDITOR_BASE/lib -lQtSolutions_PropertyBrowser-head
    }
    else {
        LIBS += -L$$PROPERTY_EDITOR_BASE/lib -lQtSolutions_PropertyBrowser-head
    }
}

# --------------------------
# Conan
# --------------------------
# http://sourceforge.net/projects/conanforqt/
# Requires at least, and tested with Conan v1.0.2.
#
# Integrates the Conan Library into the debug widget:
# --------------------------
# Builds Qtilities with the Conan library integrated into the unit testing module.
# Note that this integration was only tested in Qt 4.7.x, since Conan v1.0.2 requires
# Qt 4.7.x and higher.
#
# When building Qtilities, you can add "DEFINES+=QTILITIES_CONAN" to qmake's
# command-line arguments to enable this integration.
#
# The Conan library is released under GPL v3.0. Therefore, when using the above
# DEFINE, the Testing library (which links against Conan) is not released under
# the normal Qtilities license found at http://www.qtilities.org/licensing.html
# It is released under the following license:
#
# Under this license the Testing Library is free software: you can
# redistribute it and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation, either
# version 3 of the License, or (at your option) any later version.
#
# The Testing Library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with the Testing Library. If not, see http://www.gnu.org/licenses/.
# --------------------------
contains(DEFINES, QTILITIES_CONAN) {
    CONAN_BASE = $$QTILITIES_DEPENDENCIES/conanforqt/Conan
    INCLUDEPATH += $$CONAN_BASE/include

    CONFIG(debug, debug|release) {
        LIBS += -L$$CONAN_BASE/lib -lConand
    }
    else {
        LIBS += -L$$CONAN_BASE/lib -lConan
    }
}

