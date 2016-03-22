/*
 * Nanosdr Client
 *
 * Copyright (c) 2013 Moe Wheatley
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#pragma once

#include <QMutex>
#include <QTcpSocket>

#include "../common/threadwrapper.h"

// FIXME: Move to sdr_if
enum eSdrStatus
{
	SDR_OFF,				//TCP not connected
	SDR_CONNECTING,
	SDR_CONNECTED,
	SDR_PWOK,
	SDR_RECEIVING,
	SDR_TRANSMITTING,
	SDR_DISCONNECT_BUSY,
	SDR_DISCONNECT_PWERROR,
	SDR_DISCONNECT_TIMEOUT
};


class TcpClient : public ThreadWrapper
{
	Q_OBJECT

public:
    TcpClient(QObject *parent = 0);
    ~TcpClient();

    QString     tcp_host;
    quint16     tcp_port;

signals:
    void        newSdrStatus(int status);

private slots:
    void        threadInit();	// called by new thread when started
    void        threadExit();	// called by thread before exiting

public slots:
    void        readTcpData();
    void        tcpStateChanged(QAbstractSocket::SocketState state);
    void        connectToServer();
    void        disconnectFromServer();
    void        sendSlot();

private:
    QMutex          tcp_mutex;
    QTcpSocket     *tcp_socket;
};

