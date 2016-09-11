# ***************************************************************************
# Copyright (c) 2009-2010, Jaco Naude
#
# See http://www.qtilities.org/licensing.html for licensing details.
#
# ***************************************************************************
QTILITIES += coregui testing
DEFINES += QTILITIES_TESTING

include(../../Qtilities.pri)

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4) {
QT += widgets \
      printsupport \
      testlib
}
lessThan(QT_MAJOR_VERSION, 5) {
    CONFIG += qtestlib
}

TARGET    = QtilitiesModelTester
CONFIG   -= app_bundle

TEMPLATE = app
DESTDIR = $$QTILITIES_BIN/Tools/QtilitiesModelTester

# ------------------------------
# Temp Output Paths
# ------------------------------
OBJECTS_DIR     = $$QTILITIES_TEMP/QtilitiesModelTester
MOC_DIR         = $$QTILITIES_TEMP/QtilitiesModelTester
RCC_DIR         = $$QTILITIES_TEMP/QtilitiesModelTester
UI_DIR          = $$QTILITIES_TEMP/QtilitiesModelTester

# --------------------------
# Application Files
# --------------------------
SOURCES         += \
    dynamictreemodel.cpp \
    main.cpp \
    modeltest.cpp \

HEADERS         += \
    dynamictreemodel.h \
    modeltest.h \

OTHER_FILES     += \
    qtilities_icon_white.ico \
    rc_file.rc \

RC_FILE          = \
    rc_file.rc \
