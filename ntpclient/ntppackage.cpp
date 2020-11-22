/* ============================================================
 *
 * Date        : 2020-11-20
 * Description : Network Time Protocal Qt client
 *
 * Copyright (C) 2019-2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "ntppackage.h"

// Qt includes

#include <QDateTime>
#include <QTimer>

#define MS_JAN_1970                 0x20251FE2400L
#define CURRENT_NTP_MILLION_SECOND  (MS_JAN_1970 + QDateTime::currentMSecsSinceEpoch())

namespace QtSampleCodes
{

quint32 s_byteToUInt32(const QByteArray& bt)
{
    quint32 value = 0;

    for (int i = 0 ; i < 4 ; ++i)
    {
        value = (value << 8) + (bt[i] & 0xff);
    }

    return value;
}

qint64 s_byte64ToMillionSecond(const QByteArray& bt)
{
    quint32 second        = s_byteToUInt32(bt.left(4));
    quint32 millionSecond = s_byteToUInt32(bt.mid(4, 4));

    return (second * 1000L + millionSecond * 1000L / (1L << 32));
}

QByteArray s_millionSecondToByte64(qint64 ms)
{
    qint64 second = qint64(ms / 1000);
    qint64 nseond = (ms % 1000 * 1000L) * (1L << 32) / 1000000L;

    QByteArray result;
    result[0]     = char(second >> 24 & 0xff);
    result[1]     = char(second >> 16 & 0xff);
    result[2]     = char(second >> 8  & 0xff);
    result[3]     = char(second       & 0xff);
    result[4]     = char(nseond >> 24 & 0xff);
    result[5]     = char(nseond >> 16 & 0xff);
    result[6]     = char(nseond >> 8  & 0xff);
    result[7]     = char(nseond       & 0xff);

    return result;
}

// ---------------------------------------------------------------------

NTPPackage::NTPPackage()
{
    m_li                      = 0;
    m_vn                      = 3;
    m_mode                    = 3;
    m_stratum                 = 0;
    m_poll                    = 4;
    m_precision               = -6;
    m_rootdelay               = 0;
    m_rootdelay               = 0;
    m_rootDispersion          = 0;
    m_rootDispersion          = 0;
    m_referenceIdentifier     = 0;
    m_referenceTimestamp      = 0;
    m_originTimestamp         = 0;
    m_receiveTimestamp        = 0;
    m_translateTimestamp      = 0;

    m_requestLocalTimestampByte.clear();

    m_currentLocalTimestamp   = 0;
}

NTPPackage::~NTPPackage()
{
}

QByteArray NTPPackage::toByteArray()
{
    QByteArray result(48, 0);

    result[0]       = char((m_li << 6) | (m_vn << 3) | (m_mode));
    result[1]       = char(m_stratum   & 0xff);
    result[2]       = char(m_poll      & 0xff);
    result[3]       = char(m_precision & 0xff);

/*
    // Protocol

    result[4]       = char(m_rootdelay           >> 24 & 0xff);
    result[5]       = char(m_rootdelay           >> 16 & 0xff);
    result[6]       = char(m_rootdelay           >> 8  & 0xff);
    result[7]       = char(m_rootdelay                 & 0xff);
    result[8]       = char(m_rootDispersion      >> 24 & 0xff);
    result[9]       = char(m_rootDispersion      >> 16 & 0xff);
    result[10]      = char(m_rootDispersion      >> 8  & 0xff);
    result[11]      = char(m_rootDispersion            & 0xff);
    result[12]      = char(m_referenceIdentifier >> 24 & 0xff);
    result[13]      = char(m_referenceIdentifier >> 12 & 0xff);
    result[14]      = char(m_referenceIdentifier >> 8  & 0xff);
    result[15]      = char(m_referenceIdentifier       & 0xff);

    QByteArray rfTs = s_millionSecondToByte64(m_referenceTimestamp);
    result.replace(16, 8, rfTs.data());

    QByteArray oTs  = s_millionSecondToByte64(m_originTimestamp);
    result.replace(24, 8, oTs.data());

    QByteArray rTs  = s_millionSecondToByte64(m_receiveTimestamp);
    result.replace(32, 8, rTs.data());

    QByteArray tTs  = s_millionSecondToByte64(m_translateTimestamp);
    result.replace(40, 8, tTs.data());
*/

    m_currentLocalTimestamp     = CURRENT_NTP_MILLION_SECOND;

    QByteArray tTs              = s_millionSecondToByte64(m_currentLocalTimestamp);
    result.replace(40, 8, tTs.data());

    m_requestLocalTimestampByte = tTs;

    return result;
}

void NTPPackage::parseByByteArray(const QByteArray& bytes)
{
    m_li                        = quint8(bytes[0] >> 6 & 0x3);
    m_vn                        = quint8(bytes[0] >> 3 & 0x7);
    m_mode                      = quint8(bytes[0]      & 0x7);
    m_stratum                   = quint8(bytes[1]           );
    m_poll                      = quint8(bytes[2]           );
    m_precision                 = qint8 (bytes[3]           );
    m_rootdelay                 = qint32(s_byteToUInt32(bytes.mid(4, 4)));
    m_rootDispersion            = qint32(s_byteToUInt32(bytes.mid(8, 4)));
    m_referenceIdentifier       = s_byteToUInt32(bytes.mid(12, 4));
    m_referenceTimestamp        = s_byte64ToMillionSecond(bytes.mid(16, 8));
    m_originTimestamp           = s_byte64ToMillionSecond(bytes.mid(24, 8));
    m_receiveTimestamp          = s_byte64ToMillionSecond(bytes.mid(32, 8));
    m_translateTimestamp        = s_byte64ToMillionSecond(bytes.mid(40, 8));
    m_requestLocalTimestampByte = bytes.mid(24, 8);
    m_currentLocalTimestamp     = CURRENT_NTP_MILLION_SECOND;
}

qint64 NTPPackage::calcOffset() const
{
    // t0 - client send time    : originTimestamp
    // t1 - server received time: receiveTimestamp
    // t2 - server send time    : translateTimestamp
    // t3 - client received time: currentLocalTimestamp

    // t1     = t0 + offset + delay
    // t3     = t2 - offset + delay
    // offset = ((t1 - t3) - (t0 - t2)) / 2
    //        = (t1 - t0 + t2 - t3) / 2

    return qint64((m_receiveTimestamp - m_originTimestamp + m_translateTimestamp - m_currentLocalTimestamp) / 2);
}

bool NTPPackage::checkByOriginTimestamp(const QByteArray& ots) const
{
    return (m_requestLocalTimestampByte == ots);
}

} // namespace QtSampleCodes
