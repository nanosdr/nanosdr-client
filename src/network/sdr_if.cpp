/*
 * Nanosdr Client
 *
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#include <QTcpSocket>
#include <QThread>

#include "sdr_if.h"

SdrIf::SdrIf()
{
    state = SDRIF_ST_IDLE;

    tcp_client = new QTcpSocket;
    this->moveToThread(&worker_thread);

    /* We must use Qt::QueuedConnection to connect signals and slots across
     * threads, however, the default Qt::AutoConnection should be able to figure
     * this out on its own.
     *
     * Queued connections requiore the parameter types to be registered as meta
     * objects.
     */
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    connect(tcp_client, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(tcpStateChanged(QAbstractSocket::SocketState)));
    //connect(worker_thread, SIGNAL(started()), this, SLOT(threadStarted()));
    connect(&worker_thread, SIGNAL(finished()), tcp_client, SLOT(deleteLater()));
    worker_thread.start();
}

SdrIf::~SdrIf()
{
    worker_thread.quit();
    worker_thread.wait();      // FIXME: Use finite timeout?
}

int SdrIf::setup(quint8 iftype, QString host, quint16 port)
{
    if (iftype != SDRIF_NANOSDR && iftype != SDRIF_RFSPACE)
        return SDRIF_EINVAL;
    if (host.isEmpty() || !port)
        return SDRIF_EINVAL;


    return SDRIF_OK;
}

void SdrIf::startInterface()
{

}

void SdrIf::stopInterface()
{

}

bool SdrIf::testInterface()
{
    tcp_client->connectToHost("localhost", 42000);
    if (tcp_client->waitForConnected(10000))
    {
        tcp_client->disconnectFromHost();
        return true;
    }

    return false;
}

void SdrIf::tcpStateChanged(QAbstractSocket::SocketState new_tcp_state)
{
    sdrif_state_t   new_state = state;

    switch (new_tcp_state)
    {
    case QAbstractSocket::UnconnectedState:
        new_state = SDRIF_ST_DISCONNECTED;
        break;
    case QAbstractSocket::HostLookupState:
        new_state = SDRIF_ST_CONNECTING;
        break;
    case QAbstractSocket::ConnectingState:
        new_state = SDRIF_ST_CONNECTING;
        break;
    case QAbstractSocket::ConnectedState:
        new_state = SDRIF_ST_CONNECTED;
        break;
    case QAbstractSocket::ClosingState:
        new_state = SDRIF_ST_DISCONNECTING;
        break;
    case QAbstractSocket::BoundState:
    case QAbstractSocket::ListeningState:
    default:
        qDebug() << "TCP client entered an unknown state:" << new_tcp_state;
        break;
    }

    if (new_state != state)
    {
        state = new_state;
        emit sdrifStateChanged(new_state);
    }
}
