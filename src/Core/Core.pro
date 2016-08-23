# ***************************************************************************
# Copyright (c) 2009-2013, Jaco Naudé
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
# Qtilities - Qt Utility Libraries
# Qtilities Core Library
# ***************************************************************************
QTILITIES += logging
include(../Qtilities.pri)
INCLUDEPATH += $$QTILITIES_INCLUDE/QtilitiesCore

CONFIG += qt dll
QT += core xml
# Note: xml module is deprecated
QT -= gui

TARGET = QtilitiesCore$${QTILITIES_LIB_POSTFIX}

TEMPLATE = lib
DEFINES += QTILITIESCORE_LIBRARY
DESTDIR = $$QTILITIES_BIN

OBJECTS_DIR = $$QTILITIES_TEMP/Core
MOC_DIR = $$QTILITIES_TEMP/Core
RCC_DIR = $$QTILITIES_TEMP/Core
UI_DIR = $$QTILITIES_TEMP/Core

# --------------------------
# Qtilities Core Files
# --------------------------
HEADERS += \
    ../Common/Qtilities.h \
    source/AbstractSubjectFilter.h \
    source/ActivityPolicyFilter.h \
    source/ContextManager.h \
    source/Factory.h \
    source/FileLocker.h \
    source/FileSetInfo.h \
    source/FileUtils.h \
    source/GenericProperty.h \
    source/GenericPropertyManager.h \
    source/IAvailablePropertyProvider.h \
    source/IContext.h \
    source/IContextManager.h \
    source/IExportableFormatting.h \
    source/IExportable.h \
    source/IExportableObserver.h \
    source/IFactoryProvider.h \
    source/IIterator.h \
    source/IModificationNotifier.h \
    source/InstanceFactoryInfo.h \
    source/IObjectBase.h \
    source/IObjectManager.h \
    source/ITaskContainer.h \
    source/ITask.h \
    source/ObjectManager.h \
    source/ObserverData.h \
    source/ObserverDotWriter.h \
    source/Observer.h \
    source/ObserverHints.h \
    source/ObserverMimeData.h \
    source/ObserverRelationalTable.h \
    source/PointerList.h \
    source/QtilitiesCategory.h \
    source/QtilitiesCoreApplication.h \
    source/QtilitiesCoreApplication_p.h \
    source/QtilitiesCoreConstants.h \
    source/QtilitiesCore_global.h \
    source/QtilitiesFileInfo.h \
    source/QtilitiesProcess.h \
    source/QtilitiesPropertyChangeEvent.h \
    source/QtilitiesProperty.h \
    source/SubjectFilterTemplate.h \
    source/SubjectIterator.h \
    source/SubjectTypeFilter.h \
    source/Task.h \
    source/TaskManager.h \
    source/TreeIterator.h \
    source/VersionInformation.h \
    source/Zipper.h \


SOURCES += \
    source/ActivityPolicyFilter.cpp \
    source/ContextManager.cpp \
    source/FileLocker.cpp \
    source/FileSetInfo.cpp \
    source/FileUtils.cpp \
    source/GenericProperty.cpp \
    source/GenericPropertyManager.cpp \
    source/IExportable.cpp \
    source/InstanceFactoryInfo.cpp \
    source/ITaskContainer.cpp \
    source/ObjectManager.cpp \
    source/Observer.cpp \
    source/ObserverData.cpp \
    source/ObserverDotWriter.cpp \
    source/ObserverHints.cpp \
    source/ObserverRelationalTable.cpp \
    source/PointerList.cpp \
    source/QtilitiesCategory.cpp \
    source/QtilitiesCoreApplication.cpp \
    source/QtilitiesCoreApplication_p.cpp \
    source/QtilitiesFileInfo.cpp \
    source/QtilitiesProcess.cpp \
    source/QtilitiesPropertyChangeEvent.cpp \
    source/QtilitiesProperty.cpp \
    source/SubjectFilterTemplate.cpp \
    source/SubjectTypeFilter.cpp \
    source/Task.cpp \
    source/TaskManager.cpp \
    source/VersionInformation.cpp \
    source/Zipper.cpp \

