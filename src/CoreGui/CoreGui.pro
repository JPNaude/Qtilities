# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities - Qt Utility Libraries
# Core GUI Library
# ***************************************************************************
QTILITIES += core
include(../Qtilities.pri)
INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCoreGui

CONFIG += qt \
    dll \
    ordered
QT += xml

equals(BUILD_MODE, release) {
    TARGET = QtilitiesCoreGui$${QTILITIES_FILE_VER}
} else {
    TARGET = QtilitiesCoreGuid$${QTILITIES_FILE_VER}
}

TEMPLATE = lib

DEFINES += QTILITIES_CORE_GUI_LIBRARY
DESTDIR = $$QTILITIES_BIN

OBJECTS_DIR     = $$QTILITIES_TEMP/CoreGui
MOC_DIR         = $$QTILITIES_TEMP/CoreGui
RCC_DIR         = $$QTILITIES_TEMP/CoreGui
UI_DIR          = $$QTILITIES_TEMP/CoreGui

# --------------------------
# Propery Editor Stuff
# --------------------------
DEFINES += QT_QTPROPERTYBROWSER_IMPORT
LIBRARIES_PATH = $$QTILITIES_DEPENDENCIES
PROPERTY_EDITOR_BASE = $$LIBRARIES_PATH/qtpropertybrowser-2.5-opensource
DEPENDPATH += $$PROPERTY_EDITOR_BASE/src
INCLUDEPATH += $$PROPERTY_EDITOR_BASE/src
equals(BUILD_MODE, release) {
    LIBS += $$PROPERTY_EDITOR_BASE/lib/libQtSolutions_PropertyBrowser-2.5.a
} else {
    LIBS += $$PROPERTY_EDITOR_BASE/lib/libQtSolutions_PropertyBrowser-2.5d.a
}

# --------------------------
# Qtilities Core Gui Files
# --------------------------
RESOURCES += resources/resources.qrc

HEADERS += source/QtilitiesCoreGui_global.h \
    source/QtilitiesApplication.h \
    source/QtilitiesApplication_p.h \
    source/QtilitiesCoreGuiConstants.h \
    source/ObserverWidget.h \
    source/NamingPolicyInputDialog.h \
    source/NamingPolicyFilter.h \
    source/AbstractObserverItemModel.h \
    source/ObserverTableModel.h \
    source/ObserverTreeModel.h \
    source/ObjectScopeWidget.h \
    source/ObjectHierarchyNavigator.h \
    source/ObserverTreeItem.h \
    source/SearchBoxWidget.h \
    source/ObjectPropertyBrowser.h \
    source/ObjectInfoTreeWidget.h \
    source/CategorizedStringTreeWidget.h \
    source/ObserverTableModelCategoryFilter.h \
    source/IActionProvider.h \
    source/ActionProvider.h \
    source/LoggerConfigWidget.h \
    source/WidgetLoggerEngine.h \
    source/WidgetLoggerEngineFrontend.h \
    source/Command.h \
    source/ActionContainer.h \
    source/ActionManager.h \
    source/ClipboardManager.h \
    source/CommandEditor.h \
    source/CommandTableModel.h \
    source/IActionManager.h \
    source/IClipboard.h \
    source/LoggerGui.h \
    source/LoggerEnginesTableModel.h \
    source/ConfigurationWidget.h \
    source/IConfigPage.h \
    source/AboutWindow.h \
    source/DynamicSideWidgetViewer.h \
    source/IMode.h \
    source/ISideViewerWidget.h \
    source/ModeWidget.h \
    source/DynamicSideWidgetWrapper.h \
    source/TopToBottomList.h \
    source/QtilitiesMainWindow.h

SOURCES += source/QtilitiesApplication.cpp \
    source/QtilitiesApplication_p.cpp \
    source/ObserverWidget.cpp \
    source/NamingPolicyInputDialog.cpp \
    source/NamingPolicyFilter.cpp \
    source/AbstractObserverItemModel.cpp \
    source/ObserverTableModel.cpp \
    source/ObserverTreeModel.cpp \
    source/ObjectScopeWidget.cpp \
    source/ObjectHierarchyNavigator.cpp \
    source/ObserverTreeItem.cpp \
    source/SearchBoxWidget.cpp \
    source/ObjectPropertyBrowser.cpp \
    source/ObjectInfoTreeWidget.cpp \
    source/CategorizedStringTreeWidget.cpp \
    source/ObserverTableModelCategoryFilter.cpp \
    source/ActionProvider.cpp \
    source/LoggerConfigWidget.cpp \
    source/WidgetLoggerEngine.cpp \
    source/WidgetLoggerEngineFrontend.cpp \
    source/Command.cpp \
    source/ActionContainer.cpp \
    source/ActionManager.cpp \
    source/ClipboardManager.cpp \
    source/CommandEditor.cpp \
    source/CommandTableModel.cpp \
    source/LoggerEnginesTableModel.cpp \
    source/ConfigurationWidget.cpp \
    source/AboutWindow.cpp \
    source/DynamicSideWidgetViewer.cpp \
    source/ModeWidget.cpp \
    source/DynamicSideWidgetWrapper.cpp \
    source/TopToBottomList.cpp \
    source/QtilitiesMainWindow.cpp

FORMS += source/ObserverWidget.ui \
    source/NamingPolicyInputDialog.ui \
    source/ObjectScopeWidget.ui \
    source/ObjectHierarchyNavigator.ui \
    source/SearchBoxWidget.ui \
    source/LoggerConfigWidget.ui \
    source/CommandEditor.ui \
    source/ConfigurationWidget.ui \
    source/AboutWindow.ui \
    source/DynamicSideWidgetViewer.ui \
    source/DynamicSideWidgetWrapper.ui \
    source/ModeWidget.ui \
    source/QtilitiesMainWindow.ui
