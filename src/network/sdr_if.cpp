/*
 * Nanosdr Client
 *
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#include <QDateTime>
#include <QString>
#include <QTcpSocket>
#include <QThread>

#include "nanosdr_protocol.h"
#include "sdr_if.h"

SdrIf::SdrIf()
{
    current_state = SDRIF_ST_IDLE;
    srv_host = "localhost";
    srv_port = 42000;
    srv_type = SDRIF_NANOSDR;

    tcp_client = new QTcpSocket(this);
    ping_timer = new QTimer(this);

#if 0
    this->moveToThread(&worker_thread);

    /* We must use Qt::QueuedConnection to connect signals and slots across
     * threads, however, the default Qt::AutoConnection should be able to figure
     * this out on its own.
     *
     * Queued connections require the parameter types to be registered as meta
     * objects.
     */
    qRegisterMetaType<sdrif_state_t>("sdrif_state_t");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
#endif
    connect(tcp_client, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this, SLOT(tcpStateChanged(QAbstractSocket::SocketState)));
    connect(tcp_client, SIGNAL(readyRead()), this, SLOT(readPacket()));
    // FIXME: also connect error signals

    connect(ping_timer, SIGNAL(timeout()), this, SLOT(pingTimeout()));

    //connect(worker_thread, SIGNAL(started()), this, SLOT(threadStarted()));
    //connect(&worker_thread, SIGNAL(finished()), tcp_client, SLOT(deleteLater()));
    //connect(&worker_thread, SIGNAL(finished()), ping_timer, SLOT(deleteLater()));
    //worker_thread.start();
}

SdrIf::~SdrIf()
{
    ping_timer->stop();
    delete ping_timer;

    if (interfaceIsBusy())
        tcp_client->abort();

    delete tcp_client;
    //worker_thread.quit();
    //worker_thread.wait();      // FIXME: Use finite timeout?
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
    tlast_ctl = QDateTime::currentMSecsSinceEpoch();
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

    if (new_state == current_state)
        return;

    current_state = new_state;
    emit sdrifStateChanged(new_state);

    if (new_state == SDRIF_ST_CONNECTED)
    {
        resetStats();
        if (!ping_timer->isActive())
            ping_timer->start(5000);
    }
    else
    {
        ping_timer->stop();
        if (new_state == SDRIF_ST_DISCONNECTED)
            printStats();
    }
}

void SdrIf::readPacket(void)
{
    qint64      bytes_read;

    // we need at least 2 bytes
    if (tcp_client->bytesAvailable() < 2)
        return;

    // packet length
    bytes_read = tcp_client->read((char *)pkt.raw, 2);
    if (bytes_read != 2)
        return;

    //pkt.length = (quint16)pkt.raw[0] + ((quint16)pkt.raw[1] << 8);
    pkt.length = bytes_to_u16(pkt.raw);
    if (pkt.length <=2)
        return;

    // read rest of the packet
    bytes_read += tcp_client->read((char *)&pkt.raw[2], pkt.length - 2);
    if (bytes_read != pkt.length)
    {
        stats.errors++;
        return;
    }
    stats.bytes_rx += bytes_read;

    pkt.type = pkt.raw[3];
    if (pkt.type == PKT_TYPE_PING)
    {
        qint64  tping = bytes_to_s64((quint8 *)&pkt.raw[4]);
        emit newLatency(QDateTime::currentMSecsSinceEpoch() - tping);
    }
}

/* Sends periodic keep-alive packets to the server in order keep the
 * connection active.
 */
void SdrIf::pingTimeout(void)
{
    char        ping_pkt[12] = {
        0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    qint64      bytes_written;
    qint64      tnow = QDateTime::currentMSecsSinceEpoch();

    if (tnow - tlast_ctl < 4000)
        return;

    s64_to_bytes(tnow, (quint8 *)&ping_pkt[4]);
    bytes_written = tcp_client->write(ping_pkt, 12);
    if (bytes_written == -1)
        qDebug() << "Error sending keep-alive packet";
    else
    {
        tlast_ctl = tnow;
        stats.bytes_tx += bytes_written;
    }
}


void SdrIf::resetStats(void)
{
    memset(&stats, 0, sizeof(stats));
}

void SdrIf::printStats(void)
{
    qDebug() << "Network statistics:"
             << "\n   CTLs sent:" << stats.ctl_tx
             << "\n   CTLs rcvd:" << stats.ctl_rx
             << "\n   FFT  rcvd:" << stats.fft_rx
             << "\n  Audio rcvd:" << stats.audio_rx
             << "\n  Bytes sent:" << stats.bytes_tx
             << "\n  Bytes rcvd:" << stats.bytes_rx
             << "\n      Errors:" << stats.errors;
}
