# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
# See http://www.qtilities.org/licensing.html for licensing details.
# ***************************************************************************
QTILITIES += testing
include(../../Qtilities.pri)

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport
      testlib
}
lessThan(QT_MAJOR_VERSION, 5) {
    CONFIG += qtestlib
}

TARGET = ClipboardExample
CONFIG -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/ClipboardExample

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR = $$QTILITIES_TEMP/ClipboardExample
MOC_DIR = $$QTILITIES_TEMP/ClipboardExample
RCC_DIR = $$QTILITIES_TEMP/ClipboardExample
UI_DIR = $$QTILITIES_TEMP/ClipboardExample

# --------------------------
# Application Files
# --------------------------
SOURCES += \
    main.cpp \
    ObserverWidgetConfig.cpp \
    ExtendedObserverTreeModel.cpp \
    ExtendedObserverTableModel.cpp \

HEADERS += \
    ObserverWidgetConfig.h \
    ExtendedObserverTreeModel.h \
    ExtendedObserverTableModel.h \

RC_FILE = \
    rc_file.rc

FORMS += \
    ObserverWidgetConfig.ui \

OTHER_FILES += \
    rc_file.rc \
    qtilities_icon_white.ico \
