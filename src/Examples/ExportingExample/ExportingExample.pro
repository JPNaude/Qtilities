# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
# See http://www.qtilities.org/licensing.html for licensing details.
# ***************************************************************************
QTILITIES += coregui
include(../../Qtilities.pri)

QT += core \
      gui \
      xml

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport
}

TARGET = ExportingExample
CONFIG -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/ExportingExample

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR = $$QTILITIES_TEMP/ExportingExample
MOC_DIR = $$QTILITIES_TEMP/ExportingExample
RCC_DIR = $$QTILITIES_TEMP/ExportingExample
UI_DIR = $$QTILITIES_TEMP/ExportingExample

# --------------------------
# Application Files
# --------------------------
SOURCES += \
    main.cpp \
    VersionDetails.cpp \

HEADERS += \
    VersionDetails.h \

RC_FILE = \
    rc_file.rc \

OTHER_FILES += \
  rc_file.rc \
  qtilities_icon_white.ico \
