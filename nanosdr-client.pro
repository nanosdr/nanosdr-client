#
# Nanosdr Client
#
# Copyright (c) 2016 Alexandru Csete OZ9AEC.
# All rights reserved.
#
# This software is licensed under the terms and conditions of the
# Simplified BSD License, see legal/license-bsd.txt for details.
#

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nanosdr-client
TEMPLATE = app


SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp

HEADERS  += \
    src/mainwindow.h

FORMS    += \
    src/mainwindow.ui

OTHER_FILES += \
    legal/license-bsd.txt
