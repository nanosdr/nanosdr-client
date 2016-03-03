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


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
