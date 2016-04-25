/*
 * Nanosdr Client
 *
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#pragma once

#include <QString>
#include <QTcpSocket>
#include <QThread>
#include <QTimer>

#include "nanosdr_protocol.h"

// Error codes
#define SDRIF_OK        0
#define SDRIF_ERROR    -1
#define SDRIF_EBUSY    -2
#define SDRIF_EINVAL   -3
#define SDRIF_ETYPE    -4       // Invalid server type enumerator
#define SDRIF_EHOST    -5       // Invalid host name or IP address
#define SDRIF_EPORT    -6       // Invalid port number (e.g. 0)

#define SDRIF_NANOSDR   0
#define SDRIF_RFSPACE   1

/** SDR interface states. */
typedef enum {
    SDRIF_ST_IDLE,
    SDRIF_ST_ERROR,
    SDRIF_ST_CONNECTING,
    SDRIF_ST_CONNECTED,
    SDRIF_ST_DISCONNECTING,
    SDRIF_ST_DISCONNECTED
} sdrif_state_t;

/**
 * @brief Main interface class for handling connections to SDR servers.
 *
 * This object creates and runs in its own thread to prevent blocking of the
 * GUI while waiting for potentially I/O operation to finish. The thread is
 * created automatically by the constructor and deleted by the destructor.
 */
class SdrIf : public QObject
{
    Q_OBJECT

public:

    explicit SdrIf();
    virtual ~SdrIf();

    /**
     * @brief Setup SDR interface.
     * @param  iftype The server type.
     * @param  host   IP address or host name of the server.
     * @param  port   The port number where the server is listening.
     * @retval SDRIF_OK     The interface has been configured without errors.
     * @retval SDRIF_ETYPE  Invalid server type.
     * @retval SDRIF_EHOST  Invalid host name or IP address.
     * @retval SDRIF_EPORT  Invalid port number (e.g. 0)
     * @retval SDRIF_EINVAL Other invalid parameter.
     */
    int         setup(quint8 iftype, const QString host, quint16 port);

    /**
     * @brief Start the interface by connecting to the server.
     *
     * If server type, IP/hostname or port number have not been configured
     * using the @ref setup function, default values will be used, namely
     * nanosdr server type on localhost:42000.
     *
     * If the interface state is different from IDLE or DISCONNECTED, i.e. there
     * is already an active or pending connection, the existing connection will
     * be terminated and the new connection initiated.
     *
     * Note that this API call will return immediately and not wait for the
     * connection to be established. Success or failure must be monitored by the
     * caller through the @ref sdrifStateChanged signal.
     */
    void        startInterface();

    /** Stop the interface by disconnecting from the server. */
    void        stopInterface();

    /** Test interface. */
    bool        testInterface();

    sdrif_state_t   state() const { return current_state; }

signals:
    void        sdrifStateChanged(sdrif_state_t new_state);
    void        newLatency(qint64 msec);

private slots:
    /**
     * The state of the TCP client socket has changed.
     * This slot will run in the TCP worker thread.
     */
    void        tcpStateChanged(QAbstractSocket::SocketState new_tcp_state);

    /** Read packet from the TCP socket */
    void        readPacket(void);

    /** Ping timeout, i.e. time to send a keep-aliver packet. */
    void        pingTimeout(void);

private:
    QTimer     *ping_timer;
    QTcpSocket *tcp_client;
    //QThread     worker_thread;      // TCP worker thread

    sdrif_state_t   current_state;       // the interface state

    // server settings
    QString     srv_host;
    quint16     srv_port;
    quint8      srv_type;

    pkt_t       pkt;            // packet buffer
    qint64      tlast_ctl;      // Time of last CTL sent to server

    bool        interfaceIsBusy() const
    {
        return (current_state == SDRIF_ST_CONNECTING ||
                current_state == SDRIF_ST_CONNECTED ||
                current_state == SDRIF_ST_DISCONNECTING);
    }

    void        processCtlPacket(void);
    void        processAudioPacket(void);
    void        processFftPacket(void);

    // Statistics
    struct {
        quint64     ctl_tx;         // Control packets sent
        quint64     ctl_rx;         // Control packets received
        quint64     fft_rx;         // FFT packets received
        quint64     audio_rx;       // Audio packets received
        quint64     bytes_tx;
        quint64     bytes_rx;
        quint64     errors;
    } stats;
    void resetStats(void);
    void printStats(void);
};
