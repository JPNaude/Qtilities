# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
# See http://www.qtilities.org/licensing.html for licensing details.
# ***************************************************************************
# Qtilities - Qt Utility Libraries
# Core GUI Library
# ***************************************************************************
QTILITIES += core
include(../Qtilities.pri)

INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCoreGui

CONFIG += qt dll
QT += core xml gui
# Note: xml module is deprecated

greaterThan(QT_MAJOR_VERSION, 4) { QT += widgets printsupport }

TARGET = QtilitiesCoreGui$${QTILITIES_LIB_POSTFIX}

TEMPLATE = lib
DEFINES += QTILITIES_CORE_GUI_LIBRARY
DESTDIR = $$QTILITIES_BIN
OBJECTS_DIR = $$QTILITIES_TEMP/CoreGui
MOC_DIR = $$QTILITIES_TEMP/CoreGui
RCC_DIR = $$QTILITIES_TEMP/CoreGui
UI_DIR = $$QTILITIES_TEMP/CoreGui

# --------------------------
# Files only included when Qt Property Browser solution is present (see
# Dependencies.pri for more details)
# --------------------------
contains(DEFINES, QTILITIES_PROPERTY_BROWSER) {
    HEADERS += \
        source/ObjectPropertyBrowser.h \
        source/ObjectDynamicPropertyBrowser.h \

    SOURCES += \
        source/ObjectPropertyBrowser.cpp \
        source/ObjectDynamicPropertyBrowser.cpp \
}

# --------------------------
# Qtilities Core Gui Files
# --------------------------
RESOURCES += \
    resources/CoreGui.qrc \

HEADERS += \
    source/AboutWindow.h \
    source/AbstractObserverItemModel.h \
    source/AbstractTreeItem.h \
    source/ActionContainer.h \
    source/ActionManager.h \
    source/ActionProvider.h \
    source/AddDynamicPropertyWizard.h \
    source/ClipboardManager.h \
    source/CodeEditor.h \
    source/CodeEditorWidgetConfig.h \
    source/CodeEditorWidget.h \
    source/CommandEditor.h \
    source/Command.h \
    source/CommandTreeModel.h \
    source/ConfigurationWidget.h \
    source/DynamicSideWidgetViewer.h \
    source/DynamicSideWidgetWrapper.h \
    source/GenericPropertyBrowser.h \
    source/GenericPropertyPathEditor.h \
    source/GenericPropertyPathEditorListWrapper.h \
    source/GenericPropertyTypeManagers.h \
    source/GroupedConfigPage.h \
    source/IActionManager.h \
    source/IActionProvider.h \
    source/IClipboard.h \
    source/IConfigPage.h \
    source/IGroupedConfigPageInfoProvider.h \
    source/IMode.h \
    source/INamingPolicyDialog.h \
    source/ISideViewerWidget.h \
    source/LoggerConfigWidget.h \
    source/LoggerEnginesTableModel.h \
    source/LoggerGui.h \
    source/ModeListWidget.h \
    source/ModeManager.h \
    source/NamingPolicyFilter.h \
    source/NamingPolicyInputDialog.h \
    source/ObjectHierarchyNavigator.h \
    source/ObjectScopeWidget.h \
    source/ObserverTableModel.h \
    source/ObserverTableModelProxyFilter.h \
    source/ObserverTreeItem.h \
    source/ObserverTreeModelBuilder.h \
    source/ObserverTreeModel.h \
    source/ObserverTreeModelProxyFilter.h \
    source/ObserverWidget.h \
    source/QtilitiesApplication.h \
    source/QtilitiesApplication_p.h \
    source/QtilitiesCoreGuiConstants.h \
    source/QtilitiesCoreGui_global.h \
    source/QtilitiesMainWindow.h \
    source/SearchBoxWidget.h \
    source/SideViewerWidgetFactory.h \
    source/SideWidgetFileSystem.h \
    source/SingleTaskWidget.h \
    source/StringListWidget.h \
    source/TaskManagerGui.h \
    source/TaskSummaryWidget.h \
    source/TreeFileItem.h \
    source/TreeItemBase.h \
    source/TreeItem.h \
    source/TreeNode.h \
    source/WidgetLoggerEngineFrontend.h \
    source/WidgetLoggerEngine.h \


SOURCES += \
    source/AboutWindow.cpp \
    source/AbstractObserverItemModel.cpp \
    source/AbstractTreeItem.cpp \
    source/ActionContainer.cpp \
    source/ActionManager.cpp \
    source/ActionProvider.cpp \
    source/AddDynamicPropertyWizard.cpp \
    source/ClipboardManager.cpp \
    source/CodeEditor.cpp \
    source/CodeEditorWidgetConfig.cpp \
    source/CodeEditorWidget.cpp \
    source/Command.cpp \
    source/CommandEditor.cpp \
    source/CommandTreeModel.cpp \
    source/ConfigurationWidget.cpp \
    source/DynamicSideWidgetViewer.cpp \
    source/DynamicSideWidgetWrapper.cpp \
    source/GenericPropertyBrowser.cpp \
    source/GenericPropertyPathEditor.cpp \
    source/GenericPropertyPathEditorListWrapper.cpp \
    source/GenericPropertyTypeManagers.cpp \
    source/GroupedConfigPage.cpp \
    source/LoggerConfigWidget.cpp \
    source/LoggerEnginesTableModel.cpp \
    source/ModeListWidget.cpp \
    source/ModeManager.cpp \
    source/NamingPolicyFilter.cpp \
    source/NamingPolicyInputDialog.cpp \
    source/ObjectHierarchyNavigator.cpp \
    source/ObjectScopeWidget.cpp \
    source/ObserverTableModel.cpp \
    source/ObserverTableModelProxyFilter.cpp \
    source/ObserverTreeItem.cpp \
    source/ObserverTreeModelBuilder.cpp \
    source/ObserverTreeModel.cpp \
    source/ObserverTreeModelProxyFilter.cpp \
    source/ObserverWidget.cpp \
    source/QtilitiesApplication.cpp \
    source/QtilitiesApplication_p.cpp \
    source/QtilitiesMainWindow.cpp \
    source/SearchBoxWidget.cpp \
    source/SideViewerWidgetFactory.cpp \
    source/SideWidgetFileSystem.cpp \
    source/SingleTaskWidget.cpp \
    source/StringListWidget.cpp \
    source/TaskManagerGui.cpp \
    source/TaskSummaryWidget.cpp \
    source/TreeFileItem.cpp \
    source/TreeItemBase.cpp \
    source/TreeItem.cpp \
    source/TreeNode.cpp \
    source/WidgetLoggerEngine.cpp \
    source/WidgetLoggerEngineFrontend.cpp \


FORMS += \
    source/AboutWindow.ui \
    source/CodeEditorWidgetConfig.ui \
    source/CodeEditorWidget.ui \
    source/CommandEditor.ui \
    source/ConfigurationWidget.ui \
    source/DynamicSideWidgetViewer.ui \
    source/DynamicSideWidgetWrapper.ui \
    source/GenericPropertyPathEditorListWrapper.ui \
    source/GenericPropertyPathEditor.ui \
    source/GroupedConfigPage.ui \
    source/LoggerConfigWidget.ui \
    source/NamingPolicyInputDialog.ui \
    source/ObjectHierarchyNavigator.ui \
    source/ObjectScopeWidget.ui \
    source/ObserverWidget.ui \
    source/QtilitiesMainWindow.ui \
    source/SearchBoxWidget.ui \
    source/SideWidgetFileSystem.ui \
    source/SingleTaskWidget.ui \
    source/StringListWidget.ui \
    source/TaskSummaryWidget.ui \


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

    HEADERS += \
        source/HelpManager.h \

    SOURCES += \
        source/HelpManager.cpp \
}
