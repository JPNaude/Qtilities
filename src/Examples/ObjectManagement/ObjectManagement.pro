# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += extension_system project_management
include(../../Qtilities.pri)

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport
}

TARGET = ObjectManagementExample
TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/ObjectManagement

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/ObjectManagementExample
MOC_DIR         = $$QTILITIES_TEMP/ObjectManagementExample
RCC_DIR         = $$QTILITIES_TEMP/ObjectManagementExample
UI_DIR          = $$QTILITIES_TEMP/ObjectManagementExample

# --------------------------
# Application Files
# --------------------------
HEADERS += \
    ObjectManagementModeWidget.h \
    ObjectManagementMode.h \

SOURCES += \
    main.cpp \
    ObjectManagementModeWidget.cpp \
    ObjectManagementMode.cpp \

FORMS += \
    ObjectManagementModeWidget.ui \

RESOURCES += \
    ObjectManagementExample.qrc \

OTHER_FILES += \
    rc_file.rc \
    qtilities_icon_white.ico \

RC_FILE = \
    rc_file.rc \
