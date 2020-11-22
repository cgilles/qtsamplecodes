/* ============================================================
 *
 * Date        : 2020-11-20
 * Description : Network Time Protocal Qt client - Thread to notify local time changes
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

#include "ntpnotifier.h"

// Qt includes

#include <QDateTime>

#define daemon_sleep        60000
#define daemon_interval     1000
#define daemon_precision    1000

namespace QtSampleCodes
{

NTPNotifier::NTPNotifier(QObject* const parent)
    : QThread(parent),
      m_predictTimestamp(0)
{
}

void NTPNotifier::run()
{
    m_predictTimestamp = 0;
    bool isSleep      = false;

    while (!isInterruptionRequested())
    {
        qint64 currentTS = QDateTime::currentMSecsSinceEpoch();
        qint64 offset    = currentTS - m_predictTimestamp;

        if ((offset < -daemon_precision) || (offset > daemon_precision))
        {
            m_predictTimestamp = currentTS;
            isSleep            = true;

            // Not as expected, indicating that the time stamp is not correct, resynchronize

            emit signalLocaltimeChanged();
        }

        if (isSleep)
        {
            // If Ntp synchronization is performed, it will sleep for 60s before the next detection

            msleep(daemon_sleep);
            m_predictTimestamp += daemon_sleep;
        }
        else
        {
            msleep(daemon_interval);
            m_predictTimestamp += daemon_interval;
        }

        isSleep = false;
    }
}

} // namespace QtSampleCodes
