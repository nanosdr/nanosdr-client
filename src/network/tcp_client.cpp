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

TcpClient::TcpClient(QObject * parent)
{
    Q_UNUSED(parent);
    tcp_socket = 0;
}

TcpClient::~TcpClient()
{
    cleanupThread();	// signals thread to call "ThreadExit() to clean up resources
}

/* Called by this worker thread to initialize itself */
void TcpClient::threadInit()
{
    tcp_socket = new QTcpSocket;

    connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(readTcpData()));
    connect(tcp_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(tcpStateChanged(QAbstractSocket::SocketState)));

    //connect(m_pParent, SIGNAL(ConnectToServerSig() ), this, SLOT( ConnectToServerSlot() ) );
    //connect(m_pParent, SIGNAL(DisconnectFromServerSig() ), this, SLOT( DisconnectFromServerSlot() ) );
    //connect(m_pParent, SIGNAL(SendSig() ), this, SLOT( SendSlot() ) );
    //connect(this, SIGNAL(NewSdrStatusSig(int)  ), m_pParent, SLOT( NewSdrStatusSlot(int) ) );
    emit newSdrStatus(SDR_OFF);
}

/* Called by this worker thread to cleanup after itself */
void TcpClient::threadExit()
{
    //DisconnectFromServerSlot();
    disconnect();
    if (tcp_socket)
        delete tcp_socket;
}

void TcpClient::connectToServer()
{
    emit newSdrStatus(SDR_CONNECTING);

    if ((QAbstractSocket::ConnectingState == tcp_socket->state()) ||
        (QAbstractSocket::HostLookupState == tcp_socket->state()))
        tcp_socket->abort();
    if (QAbstractSocket::ConnectedState == tcp_socket->state())
        tcp_socket->close();

    qDebug() << "Connecting to Server" << tcp_socket->state()
             << tcp_host << tcp_port;

    tcp_socket->connectToHost(tcp_host, tcp_port);
}

void TcpClient::disconnectFromServer()
{
    if( (QAbstractSocket::ConnectedState == tcp_socket->state() ) ||
        (QAbstractSocket::HostLookupState == tcp_socket->state() ) ||
        (QAbstractSocket::ConnectingState == tcp_socket->state() ) )
    {
        tcp_socket->abort();
        tcp_socket->close();
        qDebug() << "Disconnect from server";
    }
}

void TcpClient::readTcpData()
{
    quint8      pBuf[50000];
    qint64      n;

    if (QAbstractSocket::ConnectedState == tcp_socket->state())
    {
        do
        {
            n = tcp_socket->bytesAvailable();
            if ((n < 50000) && n > 0)
            {
                tcp_socket->read((char*)pBuf, 50000);
                //((CNetio*)m_pParent)->AssembleAscpMsg(pBuf, n);
            }
        } while (n > 0);
    }
}

void TcpClient::sendSlot()
{

    if (QAbstractSocket::ConnectedState == tcp_socket->state())
    {
        tcp_mutex.lock();
        // send data
        tcp_mutex.unlock();
    }
}

void TcpClient::tcpStateChanged(QAbstractSocket::SocketState state)
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
