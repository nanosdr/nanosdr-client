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

#include <QTcpSocket>
#include <QThread>

// Error codes
#define SDRIF_OK        0
#define SDRIF_ERROR    -1
#define SDRIF_EBUSY    -2
#define SDRIF_EINVAL   -3

#define SDRIF_NANOSDR   0
#define SDRIF_RFSPACE   1

/** SDR interface states. */
typedef enum {
    SDRIF_ST_UNKNOWN,
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

    SdrIf();
    virtual ~SdrIf();

    /**
     * @brief Setup SDR interface.
     * @param  iftype The server type.
     * @param  host   IP address or host name of the server.
     * @param  port   The port number where the server is listening.
     * @retval SDRIF_OK     The interface has been configured without errors.
     * @retval SDRIF_EINVAL Invalid parameter.
     */
    int         setup(quint8 iftype, QString host, quint16 port);

    /**
     * @brief Connect to server and start streaming data.
     *
     * If server IP/hostname and port number have not been set up using the
     * @ref setup function, connection will be attempted to localhost:42000.
     *
     * Note that this API call will return immediately and not wait for the
     * connection to be established. Success or failure must be monitored by the
     * caller through the @ref sdrifStateChanged signal.
     */
    void        startInterface();

    /**
     * @brief Stop streaming and disconnect from server.
     * @note Servers will not allow idle connections, which is why we have to
     *       disconnect when the streaming is stopped.
     */
    void        stopInterface();

    /** Test interface. */
    bool        testInterface();

signals:
    void        sdrifStateChanged(sdrif_state_t new_state);

private slots:
    /**
     * The state of the TCP client socket has changed.
     * This slot will run in the TCP worker thread.
     */
    void        tcpStateChanged(QAbstractSocket::SocketState new_tcp_state);

private:
    QTcpSocket *tcp_client;
    QThread     worker_thread;      // TCP worker thread

    sdrif_state_t   state;
};
