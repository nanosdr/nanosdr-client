/*
 * Nanosdr Client
 *
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#include <QString>
#include <QTcpSocket>
#include <QThread>


#include "sdr_if.h"

SdrIf::SdrIf()
{
    current_state = SDRIF_ST_IDLE;
    srv_host = "localhost";
    srv_port = 42000;
    srv_type = SDRIF_NANOSDR;

    tcp_client = new QTcpSocket;
    this->moveToThread(&worker_thread);

    /* We must use Qt::QueuedConnection to connect signals and slots across
     * threads, however, the default Qt::AutoConnection should be able to figure
     * this out on its own.
     *
     * Queued connections requiore the parameter types to be registered as meta
     * objects.
     */
    qRegisterMetaType<sdrif_state_t>("sdrif_state_t");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");

    connect(tcp_client, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(tcpStateChanged(QAbstractSocket::SocketState)));
    // FIXME: also connect error signals

    //connect(worker_thread, SIGNAL(started()), this, SLOT(threadStarted()));
    connect(&worker_thread, SIGNAL(finished()), tcp_client, SLOT(deleteLater()));
    worker_thread.start();
}

SdrIf::~SdrIf()
{
    if (interfaceIsBusy())
        tcp_client->abort();

    worker_thread.quit();
    worker_thread.wait();      // FIXME: Use finite timeout?
}

int SdrIf::setup(quint8 iftype, const QString host, quint16 port)
{
    if (iftype != SDRIF_NANOSDR && iftype != SDRIF_RFSPACE)
        return SDRIF_ETYPE;
    if (host.isEmpty())
        return SDRIF_EHOST;
    if (!port)
        return SDRIF_EPORT;

    srv_host = host;
    srv_port = port;
    srv_type = iftype;

    return SDRIF_OK;
}

void SdrIf::startInterface()
{
    if (current_state == SDRIF_ST_CONNECTED)
        tcp_client->close();
    else if (current_state == SDRIF_ST_CONNECTING ||
             current_state == SDRIF_ST_DISCONNECTING)
        tcp_client->abort();

    tcp_client->connectToHost(srv_host, srv_port);
}

void SdrIf::stopInterface()
{
    tcp_client->disconnectFromHost();
}

bool SdrIf::testInterface()
{
    if (interfaceIsBusy())
        return false;

    tcp_client->connectToHost(srv_host, srv_port);
    if (tcp_client->waitForConnected(10000))
    {
        tcp_client->disconnectFromHost();
        return true;
    }

    return false;
}

void SdrIf::tcpStateChanged(QAbstractSocket::SocketState new_tcp_state)
{
    sdrif_state_t   new_state = current_state;

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
        qDebug() << "TCP client entered an unexpected state:" << new_tcp_state;
        break;
    }

    if (new_state != current_state)
    {
        current_state = new_state;
        emit sdrifStateChanged(new_state);
    }
}
