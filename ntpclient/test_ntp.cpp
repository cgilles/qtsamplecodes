/* ============================================================
 *
 * Date        : 2020-11-20
 * Description : Network Time Protocal Qt client - Unit test
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

// Qt includes

#include <QCoreApplication>
#include <QDebug>

// Local includes

#include "ntptimestamp.h"

using namespace QtSampleCodes;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDateTime time;
    time.setMSecsSinceEpoch(NTPTimeStamp::instance()->currentMSTimestamp());

    qDebug() << "Using Ntp servers:" << NTPTimeStamp::instance()->ntpServers();
    qDebug() << time.toString();
    qDebug() << time.toMSecsSinceEpoch() << "milli-seconds since Epoch";

    return -1;
}
