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

#ifndef NTP_TIMESTAMP_H
#define NTP_TIMESTAMP_H

// Qt includes

#include <QtCore>

// Local includes

#include "ntpnotifier.h"
#include "ntpclient.h"

namespace QtSampleCodes
{

class NTPTimeStamp : public QObject
{
    Q_OBJECT

public:

    static NTPTimeStamp* instance();
    ~NTPTimeStamp();

    qint64 currentMSTimestamp();

    QStringList ntpServers() const;

private:

    NTPTimeStamp();

    void init();
    void syncTimestamp();

private Q_SLOTS:

    void slotLocaltimeChanged();
    void slotNTPFinished();

private:

    /**
     * The thread checks whether there is a deviation in the local time, and if necessary, it will resynchronize the time.
     */
    NTPNotifier*           m_daemonThread;

    /**
     * Synchronization is not completed, take local time directly.
     */
    bool                   m_syncDone;

    /**
     * Deviation from network time.
     */
    qint64                 m_offsetTS;

    /**
     * Hosts list.
     */
    QStringList            m_ntpServers;
    QMap<NTPClient*, bool> m_ntpClients;
};

} // namespace QtSampleCodes

#endif // NTP_TIMESTAMP_H
