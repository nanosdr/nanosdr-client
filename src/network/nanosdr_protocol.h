/*
 * Nanosdr network protocol specific defines and functions.
 *
 * Copyright (c) 2016 Alexandru Csete OZ9AEC.
 * All rights reserved.
 *
 * This software is licensed under the terms and conditions of the
 * Simplified BSD License, see legal/license-bsd.txt for details.
 */
#pragma once

#include <QtGlobal>


// Packet types
#define PKT_TYPE_PING           0x00
#define PKT_TYPE_SET_CI         0x01
#define PKT_TYPE_GET_CI         0x02
#define PKT_TYPE_GET_CI_RNG     0x03
#define PKT_TYPE_AUDIO          0x04
#define PKT_TYPE_FFT            0x05

typedef struct {
    quint16     length;
    quint8      type;
    quint8      raw[65536];
} pkt_t;


static inline quint16 bytes_to_u16(quint8 * bytes)
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    return *(quint16 *)bytes;
#else
    return (quint16)bytes[0] | ((quint16)bytes[1] << 8);
#endif
}

static inline qint64 bytes_to_s64(quint8 * bytes)
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    return *(qint64 *)bytes;
#else
    return (qint64)bytes[0] |
           ((qint64)bytes[1] << 8) |
           ((qint64)bytes[2] << 16) |
           ((qint64)bytes[3] << 24) |
           ((qint64)bytes[4] << 32) |
           ((qint64)bytes[5] << 40) |
           ((qint64)bytes[6] << 48) |
           ((qint64)bytes[7] << 56);
#endif
}

static inline void s64_to_bytes(qint64 val, quint8 * bytes)
{
    bytes[0] = val & 0xFF;
    bytes[1] = (val >> 8) & 0xFF;
    bytes[2] = (val >> 16) & 0xFF;
    bytes[3] = (val >> 24) & 0xFF;
    bytes[4] = (val >> 32) & 0xFF;
    bytes[5] = (val >> 40) & 0xFF;
    bytes[6] = (val >> 48) & 0xFF;
    bytes[7] = (val >> 56) & 0xFF;
}
