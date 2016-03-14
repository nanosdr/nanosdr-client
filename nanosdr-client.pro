#
# Nanosdr Client
#
# Copyright (c) 2016 Alexandru Csete OZ9AEC.
# All rights reserved.
#
# This software is licensed under the terms and conditions of the
# Simplified BSD License, see legal/license-bsd.txt for details.
#

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nanosdr-client
TEMPLATE = app


SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/network/sdr_if.cpp \
    src/network/tcp_client.cpp \
    src/server_dialog.cpp

HEADERS  += \
    src/common/threadwrapper.h \
    src/mainwindow.h \
    src/network/sdr_if.h \
    src/network/tcp_client.h \
    src/server_dialog.h

FORMS    += \
    src/mainwindow.ui \
    src/server_dialog.ui

OTHER_FILES += \
    legal/license-bsd.txt
