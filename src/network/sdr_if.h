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
#define SDR_IF_OK       0
#define SDR_IF_ERROR   -1
#define SDR_IF_EBUSY   -2
#define SDR_IF_EINVAL  -3

#define SDR_IF_NANOSDR  0
#define SDR_IF_RFSPACE  1

/** SDR interface states. */
typedef enum {
    SDR_IF_ST_UNKNOWN,
    SDR_IF_ST_IDLE,
    SDR_IF_ST_ERROR,
    SDR_IF_ST_CONNECTED,
    SDR_IF_ST_DISCONNECTED
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
     * @retval SDR_IF_OK     The interface has been configured without errors.
     * @retval SDR_IF_EINVAL Invalid parameter.
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

private:
    QTcpSocket *tcp_client;
    QThread    *worker_thread;

    sdrif_state_t   state;
};
