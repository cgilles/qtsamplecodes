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

#include "ntptimestamp.h"

// Qt includes

#include <QMutex>
#include <QDateTime>

namespace QtSampleCodes
{

NTPTimeStamp* NTPTimeStamp::instance()
{
    static NTPTimeStamp* s_instance = nullptr;

    if (!s_instance)
    {
        s_instance = new NTPTimeStamp();
        s_instance->init();
    }

    return s_instance;
}

qint64 NTPTimeStamp::currentMSTimestamp()
{
    // Synchronization is not completed, directly return the current timestamp.

    return (QDateTime::currentMSecsSinceEpoch() + (m_syncDone ? m_offsetTS : 0));
}

NTPTimeStamp::NTPTimeStamp()
    : QObject (nullptr),
      m_daemonThread(nullptr),
      m_syncDone(false),
      m_offsetTS(0)
{
}

NTPTimeStamp::~NTPTimeStamp()
{
    if (m_daemonThread)
    {
        m_daemonThread->requestInterruption();
        m_daemonThread->quit();
        m_daemonThread->wait();
        m_daemonThread->deleteLater();
        m_daemonThread = nullptr;
    }

    qDeleteAll(m_ntpClients.keys());
    m_ntpClients.clear();
}

void NTPTimeStamp::init()
{
    // Ntp server

    m_ntpServers.clear();

    // Standard Ntp time servers at IRFM

    m_ntpServers.append("time.cea.fr");
    m_ntpServers.append("time.intra.cea.fr");

    // Ntp client

    qDeleteAll(m_ntpClients.keys());
    m_ntpClients.clear();

    foreach (const QString& host, m_ntpServers)
    {
        NTPClient* const client = new NTPClient(host);

        connect(client, SIGNAL(signalNtpFinished()),
                this, SLOT(slotNTPFinished()));

        m_ntpClients[client] = false;
    }

    // Start daemon thread.

    m_daemonThread = new NTPNotifier(this);

    connect(m_daemonThread, SIGNAL(signalLocaltimeChanged()),
            this, SLOT(slotLocaltimeChanged()));

    m_daemonThread->start();
}

QStringList NTPTimeStamp::ntpServers() const
{
    return m_ntpServers;
}

void NTPTimeStamp::slotLocaltimeChanged()
{
    m_syncDone = false;
    m_offsetTS = 0;

    syncTimestamp();
}

void NTPTimeStamp::syncTimestamp()
{
    // qDebug() << "m_syncTimestamp :" << QDateTime::currentMSecsSinceEpoch() << ", " << QThread::currentThreadId();

    for (QMap<NTPClient*, bool>::iterator it = m_ntpClients.begin() ; it != m_ntpClients.end() ; ++it)
    {
        it.value() = false;
        it.key()->start();
    }
}

void NTPTimeStamp::slotNTPFinished()
{
    NTPClient* const ntpSender = qobject_cast<NTPClient*>(sender());

    if (!ntpSender)
    {
        return;
    }

    if (!m_ntpClients.contains(ntpSender))
    {
        qDebug()<<"onNTPFinished no this client";

        return;
    }

    m_ntpClients[ntpSender] = true;

/*
    // Traverse all

    bool finished = true;

    for (QMap<NTPClient*, bool>::iterator it = m_ntpClients.begin() ; it != m_ntpClients.end() ; it++)
    {
        if (!it.value())
        {
            finished = false;
            break;
        }
    }

    // Wait for all to finish

    m_syncDone = finished;
*/

    m_syncDone = true;

    // Calculate the average deviation.

    qint64 totaloffset = 0;
    qint64 maxoffset   = 0;
    qint64 minoffset   = INT64_MAX;
    qint8 validCount   = 0;

    for (QMap<NTPClient*, bool>::iterator it = m_ntpClients.begin() ; it != m_ntpClients.end() ; ++it)
    {
        if (!it.value())
        {
            continue;
        }

        if (!it.key()->done())
        {
            continue;
        }

        qint64 os    = it.key()->offset();
        totaloffset += os;

        maxoffset    = qMax(maxoffset, os);
        minoffset    = qMin(minoffset, os);

        ++validCount;
    }

    // Remove the maximum value and remove the minimum value before taking the average. If 10 are full, there are 8 left.

    if (validCount > 2)
    {
        totaloffset -= (maxoffset + minoffset);
        validCount  -= 2;
    }

    m_offsetTS = (validCount > 0) ? ((validCount == 8) ? (totaloffset >> 3)
                                                       :  totaloffset / validCount)
                                  : 0;
}

} // namespace QtSampleCodes
