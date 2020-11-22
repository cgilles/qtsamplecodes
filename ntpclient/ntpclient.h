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

#ifndef NTP_CLIENT_H
#define NTP_CLIENT_H

// Qt includes

#include <QtCore>
#include <QUdpSocket>

// Local includes

#include "ntppackage.h"

namespace QtSampleCodes
{

class NTPClient : public QObject
{
    Q_OBJECT

public:

    explicit NTPClient(const QString& host, quint16 port = 123);
    ~NTPClient();

    /**
     * Start synchronization timestamp
     */
    void start();

    /**
     * Whether synchronization is complete
     */
    bool done()     const;

    /**
     * Timestamp to sync after done
     */
    qint64 offset() const;

Q_SIGNALS:

    /**
     * Signal emitted when synchronization is complete.
     */
    void signalNtpFinished();

    /**
     * Start sync signal. start() is issued, thread safe.
     */
    void signalNtpStart();

protected:

    /**
     * Asynchronous processing Udp timeout
     */
    void timerEvent(QTimerEvent* event) Q_DECL_OVERRIDE;

private Q_SLOTS:

    /**
     * Start ntp sync timestamp.
     */
    void slotNTPStart();

    /**
     * Manage Udp connection
     */
    void slotNtpConnected();
    void slotNtpReadyRead();
    void slotNtpError(QAbstractSocket::SocketError);

private:

    void       cancel();

    NTPPackage generateRequestPackage();
    NTPPackage generateResponsePackageFromByte(const QByteArray& bytes);

    void       initSocket();
    QByteArray readSocket();
    void       releaseSocket();
    void       delayResend();

private:

    const QString m_ntpServerHost;
    const quint16 m_ntpServerPort; // Note: Standard Ntp port is 123

    bool          m_done;
    QByteArray    m_originTimestampByte;
    qint64        m_offset;
    qint32        m_failedTimes;

    QUdpSocket*   m_udpsocket;
    qint32        m_socketTimerID;
    qint32        m_delayResnedTimerID;
};

} // namespace QtSampleCodes

#endif // NTP_CLIENT_H
