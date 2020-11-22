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

#ifndef NTP_NOTIFIER_H
#define NTP_NOTIFIER_H

// Qt includes

#include <QObject>
#include <QThread>

namespace QtSampleCodes
{

class NTPNotifier : public QThread
{
    Q_OBJECT

public:

    explicit NTPNotifier(QObject* const parent = nullptr);

Q_SIGNALS:

    void signalLocaltimeChanged();

protected:

    void run() Q_DECL_OVERRIDE;

private:

    qint64 m_predictTimestamp;
};

} // namespace QtSampleCodes

#endif // NTP_NOTIFIER_H
