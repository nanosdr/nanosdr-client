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

#include <QObject>

// Error codes
#define SDR_IF_OK       0
#define SDR_IF_ERROR   -1
#define SDR_IF_EBUSY   -2
#define SDR_IF_EINVAL  -3

#define SDR_IF_NANOSDR  0
#define SDR_IF_RFSPACE  1


/**
 * @brief Main interface class for handling connections to SDR servers.
 */
class SdrIf : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Setup SDR interface.
     * @param  iftype The server type.
     * @param  host   IP address or host name of the server.
     * @param  port   The port number where the server is listening.
     * @retval SDR_IF_OK     The interface has been configured without errors.
     * @retval SDR_IF_EINVAL Invalid parameter.
     */
    int     setup(quint8 iftype, QString host, quint16 port);

    /**
     * @brief Connect to server and start streaming data.
     * @return SDR_IF_OK.
     */
    int     start();

    /**
     * @brief Stop streaming and disconnect from server.
     * @return SDR_IF_OK.
     */
    int     stop();

};
