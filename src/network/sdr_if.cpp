/*
 * Nanosdr Client
 *
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */

#include "sdr_if.h"


int SdrIf::setup(quint8 iftype, QString host, quint16 port)
{
    if (iftype != SDR_IF_NANOSDR && iftype != SDR_IF_RFSPACE)
        return SDR_IF_EINVAL;
    if (host.isEmpty() || !port)
        return SDR_IF_EINVAL;


    return SDR_IF_OK;
}
