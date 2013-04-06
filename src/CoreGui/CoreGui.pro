# ***************************************************************************
# Copyright (c) 2009-2013, Floware Computing (Pty) Ltd
# See http://www.qtilities.org/licensing.html for licensing details.
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

QT += xml \
      gui

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport
}

TARGET = QtilitiesCoreGui$${QTILITIES_LIB_POSTFIX}

TEMPLATE = lib
DEFINES += QTILITIES_CORE_GUI_LIBRARY
DESTDIR = $$QTILITIES_BIN
OBJECTS_DIR = $$QTILITIES_TEMP/CoreGui
MOC_DIR = $$QTILITIES_TEMP/CoreGui
RCC_DIR = $$QTILITIES_TEMP/CoreGui
UI_DIR = $$QTILITIES_TEMP/CoreGui

# --------------------------
# Files only included when Qt Property Browser solution is present
# --------------------------
contains(DEFINES, QTILITIES_PROPERTY_BROWSER) {
    HEADERS += source/ObjectPropertyBrowser.h \
               source/ObjectDynamicPropertyBrowser.h
    SOURCES += source/ObjectPropertyBrowser.cpp \
               source/ObjectDynamicPropertyBrowser.cpp
}

# --------------------------
# Qtilities Core Gui Files
# --------------------------
RESOURCES += \
    resources/CoreGui.qrc
HEADERS += source/QtilitiesCoreGui_global.h \
    source/QtilitiesApplication.h \
    source/QtilitiesApplication_p.h \
    source/QtilitiesCoreGuiConstants.h \
    source/ObserverWidget.h \
    source/NamingPolicyInputDialog.h \
    source/NamingPolicyFilter.h \
    source/AbstractObserverItemModel.h \
    source/ObjectScopeWidget.h \
    source/ObjectHierarchyNavigator.h \
    source/ObserverTreeItem.h \
    source/SearchBoxWidget.h \
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
    source/DynamicSideWidgetWrapper.h \
    source/QtilitiesMainWindow.h \
    source/CodeEditor.h \
    source/CodeEditorWidget.h \
    source/CodeEditorWidgetConfig.h \
    source/TreeNode.h \
    source/AbstractTreeItem.h \
    source/TreeItem.h \
    source/ObserverTreeModelProxyFilter.h \
    source/TreeFileItem.h \
    source/TreeItemBase.h \
    source/SideWidgetFileSystem.h \
    source/ModeManager.h \
    source/SideViewerWidgetFactory.h \
    source/ModeListWidget.h \
    source/ObserverTableModel.h \
    source/ObserverTreeModel.h \
    source/StringListWidget.h \
    source/CommandTreeModel.h \
    source/INamingPolicyDialog.h \
    source/ObserverTreeModelBuilder.h \
    source/TaskManagerGui.h \
    source/SingleTaskWidget.h \
    source/TaskSummaryWidget.h \
    source/ObserverTableModelProxyFilter.h \
    source/GroupedConfigPage.h \
    source/IGroupedConfigPageInfoProvider.h \
    source/AddDynamicPropertyWizard.h

SOURCES += source/QtilitiesApplication.cpp \
    source/QtilitiesApplication_p.cpp \
    source/ObserverWidget.cpp \
    source/NamingPolicyInputDialog.cpp \
    source/NamingPolicyFilter.cpp \
    source/AbstractObserverItemModel.cpp \
    source/ObjectScopeWidget.cpp \
    source/ObjectHierarchyNavigator.cpp \
    source/ObserverTreeItem.cpp \
    source/SearchBoxWidget.cpp \
    source/ActionProvider.cpp \
    source/LoggerConfigWidget.cpp \
    source/WidgetLoggerEngine.cpp \
    source/WidgetLoggerEngineFrontend.cpp \
    source/Command.cpp \
    source/ActionContainer.cpp \
    source/ActionManager.cpp \
    source/ClipboardManager.cpp \
    source/CommandEditor.cpp \
    source/LoggerEnginesTableModel.cpp \
    source/ConfigurationWidget.cpp \
    source/AboutWindow.cpp \
    source/DynamicSideWidgetViewer.cpp \
    source/DynamicSideWidgetWrapper.cpp \
    source/QtilitiesMainWindow.cpp \
    source/CodeEditor.cpp \
    source/CodeEditorWidget.cpp \
    source/CodeEditorWidgetConfig.cpp \
    source/TreeNode.cpp \
    source/AbstractTreeItem.cpp \
    source/TreeItem.cpp \
    source/ObserverTreeModelProxyFilter.cpp \
    source/TreeFileItem.cpp \
    source/TreeItemBase.cpp \
    source/SideWidgetFileSystem.cpp \
    source/ModeManager.cpp \
    source/SideViewerWidgetFactory.cpp \
    source/ModeListWidget.cpp \
    source/ObserverTableModel.cpp \
    source/ObserverTreeModel.cpp \
    source/StringListWidget.cpp \
    source/CommandTreeModel.cpp \
    source/ObserverTreeModelBuilder.cpp \
    source/TaskManagerGui.cpp \
    source/SingleTaskWidget.cpp \
    source/TaskSummaryWidget.cpp \
    source/ObserverTableModelProxyFilter.cpp \
    source/GroupedConfigPage.cpp \
    source/AddDynamicPropertyWizard.cpp

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
    source/QtilitiesMainWindow.ui \
    source/CodeEditorWidget.ui \
    source/CodeEditorWidgetConfig.ui \
    source/SideWidgetFileSystem.ui \
    source/StringListWidget.ui \
    source/SingleTaskWidget.ui \
    source/TaskSummaryWidget.ui \
    source/GroupedConfigPage.ui

# --------------------------
# Files only included when Qtilities help is defined to be present.
# --------------------------
contains(DEFINES, QTILITIES_NO_HELP) {
    # Do nothing here for now.
} else {
    greaterThan(QT_MAJOR_VERSION, 4) {
        QT += help
    }
    lessThan(QT_MAJOR_VERSION, 5) {
        CONFIG  += help
    }
    HEADERS += source/HelpManager.h
    SOURCES += source/HelpManager.cpp
}
