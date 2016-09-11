# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += coregui testing
include(../../Qtilities.pri)

QT += core \
      gui \
      xml \

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport
}

TARGET    = BuildingTreeStructures
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Examples/BuildingTreeStructures

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/BuildingTreeStructures
MOC_DIR         = $$QTILITIES_TEMP/BuildingTreeStructures
RCC_DIR         = $$QTILITIES_TEMP/BuildingTreeStructures
UI_DIR          = $$QTILITIES_TEMP/BuildingTreeStructures

# --------------------------
# Application Files
# --------------------------
SOURCES += \
    main.cpp \

RC_FILE = \
    rc_file.rc

OTHER_FILES += \
    rc_file.rc \
    qtilities_icon_white.ico \
