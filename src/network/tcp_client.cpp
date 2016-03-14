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
#include <QDebug>
#include <QTcpSocket>

#include "tcp_client.h"

//#define TCP_CONNECT_TIMELIMIT 100	//time to wait for connect in 100mSec steps

Tcp::Tcp(QObject * parent)
{
    Q_UNUSED(parent);
    tcp_client = 0;
}

Tcp::~Tcp()
{
    cleanupThread();	// signals thread to call "ThreadExit() to clean up resources
}

/* Called by this worker thread to initialize itself */
void Tcp::threadInit()
{
    tcp_client = new QTcpSocket;

    connect(tcp_client, SIGNAL(readyRead()), this, SLOT(readTcpData()));
    connect(tcp_client, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(tcpStateChanged(QAbstractSocket::SocketState)));

    //connect(m_pParent, SIGNAL(ConnectToServerSig() ), this, SLOT( ConnectToServerSlot() ) );
    //connect(m_pParent, SIGNAL(DisconnectFromServerSig() ), this, SLOT( DisconnectFromServerSlot() ) );
    //connect(m_pParent, SIGNAL(SendSig() ), this, SLOT( SendSlot() ) );
    //connect(this, SIGNAL(NewSdrStatusSig(int)  ), m_pParent, SLOT( NewSdrStatusSlot(int) ) );
    emit newSdrStatus(SDR_OFF);
}

/* Called by this worker thread to cleanup after itself */
void Tcp::threadExit()
{
    //DisconnectFromServerSlot();
    disconnect();
    if (tcp_client)
        delete tcp_client;
}

void Tcp::connectToServer()
{
    emit newSdrStatus(SDR_CONNECTING);

    if ((QAbstractSocket::ConnectingState == tcp_client->state()) ||
        (QAbstractSocket::HostLookupState == tcp_client->state()))
        tcp_client->abort();
    if (QAbstractSocket::ConnectedState == tcp_client->state())
        tcp_client->close();

    qDebug() << "Connecting to Server" << tcp_client->state()
             << tcp_host << tcp_port;

    tcp_client->connectToHost(tcp_host, tcp_port);
}

void Tcp::disconnectFromServer()
{
    if( (QAbstractSocket::ConnectedState == tcp_client->state() ) ||
        (QAbstractSocket::HostLookupState == tcp_client->state() ) ||
        (QAbstractSocket::ConnectingState == tcp_client->state() ) )
    {
        tcp_client->abort();
        tcp_client->close();
        qDebug() << "Disconnect from server";
    }
}

void Tcp::readTcpData()
{
    quint8      pBuf[50000];
    qint64      n;

    if (QAbstractSocket::ConnectedState == tcp_client->state())
    {
        do
        {
            n = tcp_client->bytesAvailable();
            if ((n < 50000) && n > 0)
            {
                tcp_client->read((char*)pBuf, 50000);
                //((CNetio*)m_pParent)->AssembleAscpMsg(pBuf, n);
            }
        } while (n > 0);
    }
}

void Tcp::sendSlot()
{

    if (QAbstractSocket::ConnectedState == tcp_client->state())
    {
        tcp_mutex.lock();
        // send data
        tcp_mutex.unlock();
    }
}

void Tcp::tcpStateChanged(QAbstractSocket::SocketState state)
{
    qDebug() << state;

    switch (state)
    {
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::HostLookupState:
        break;
    case QAbstractSocket::ConnectedState:
        emit newSdrStatus(SDR_CONNECTED);
        qDebug() << "Connected to Server";
        break;
    case QAbstractSocket::ClosingState:
        break;
    case QAbstractSocket::UnconnectedState:
  /*      if (SDR_CONNECTING == ((CNetio*)m_pParent)->m_SdrStatus)
        {
            emit NewSdrStatusSig(SDR_DISCONNECT_TIMEOUT);
            qDebug()<<"Disconnected from Server";
        }*/
        break;
    default:
        break;
    }
}
