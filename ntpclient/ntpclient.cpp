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

#include "ntpclient.h"

// Qt includes

#include <QDateTime>
#include <QTimer>

// Local includes

#include "ntppackage.h"

namespace QtSampleCodes
{

const qint32 s_delayResendIntervals[UDP_RESEND_INTERVAL_COUNT] =
{
    2000,
    5000,
    10000,
    15000,
    20000,
    30000
};

// ----------------------------------------------------------------------

NTPClient::NTPClient(const QString& host, quint16 port)
    : QObject(nullptr),
      m_ntpServerHost(host),
      m_ntpServerPort(port),
      m_done(false),
      m_originTimestampByte(),
      m_offset(0),
      m_failedTimes(0),
      m_udpsocket(nullptr),
      m_socketTimerID(0),
      m_delayResnedTimerID(0)
{
    connect(this, SIGNAL(signalNtpStart()),
            this, SLOT(slotNTPStart()));
}

NTPClient::~NTPClient()
{
    releaseSocket();
}

void NTPClient::start()
{
    emit signalNtpStart();
}

bool NTPClient::done() const
{
    return m_done;
}

qint64 NTPClient::offset() const
{
    return m_offset;
}

void NTPClient::initSocket()
{
    m_udpsocket = new QUdpSocket(this);

    connect(m_udpsocket, SIGNAL(connected()),
            this, SLOT(slotNtpConnected()));

    connect(m_udpsocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotNtpError(QAbstractSocket::SocketError)));

    connect(m_udpsocket, SLOT(readyRead()),
            this, SLOT(slotNtpReadyRead()));
}

QByteArray NTPClient::readSocket()
{
    QByteArray data;

    if (!m_udpsocket)
    {
        return data;
    }

    while (m_udpsocket->hasPendingDatagrams())
    {
        data.clear();
        data.resize(int(m_udpsocket->pendingDatagramSize()));
        m_udpsocket->readDatagram(data.data(), data.size());
    }

    return data;
}

void NTPClient::releaseSocket()
{
    if (m_udpsocket)
    {
        m_udpsocket->disconnectFromHost();
        m_udpsocket->close();
        m_udpsocket->deleteLater();
        m_udpsocket = nullptr;
    }

    if (m_socketTimerID != 0)
    {
        killTimer(m_socketTimerID);
        m_socketTimerID = 0;
    }

    if (m_delayResnedTimerID != 0)
    {
        killTimer(m_delayResnedTimerID);
        m_delayResnedTimerID = 0;
    }
}

NTPPackage NTPClient::generateRequestPackage()
{
    NTPPackage pk;

    return pk;
}

NTPPackage NTPClient::generateResponsePackageFromByte(const QByteArray& bytes)
{
    NTPPackage pk;
    pk.parseByByteArray(bytes);

    return pk;
}

void NTPClient::slotNTPStart()
{
    cancel();
    initSocket();

    m_done          = false;
    m_offset        = 0;
    m_udpsocket->connectToHost(m_ntpServerHost, m_ntpServerPort);

    m_socketTimerID = startTimer(UDP_TIMEOUT);
}

void NTPClient::cancel()
{
    releaseSocket();
}

void NTPClient::delayResend()
{
    m_delayResnedTimerID = startTimer(s_delayResendIntervals[qBound(1, m_failedTimes, UDP_RESEND_INTERVAL_COUNT) - 1]);
}

void NTPClient::timerEvent(QTimerEvent* event)
{
    if ((m_delayResnedTimerID == 0) && (m_socketTimerID == 0))
    {
        return QObject::timerEvent(event);
    }

    if      (event->timerId() == m_socketTimerID)
    {
        qDebug() << "NTPClient::timerEvent timeout:" << QDateTime::currentMSecsSinceEpoch() << ", for:" << m_ntpServerHost;

        killTimer(m_socketTimerID);
        m_socketTimerID = 0;
        slotNtpError(QAbstractSocket::TemporaryError);
    }
    else if (event->timerId() == m_delayResnedTimerID)
    {
        qDebug() << "NTPClient::timerEvent delay resend:" << QDateTime::currentMSecsSinceEpoch();

        killTimer(m_delayResnedTimerID);
        m_delayResnedTimerID = 0;
        start();
    }
    else
    {
        return QObject::timerEvent(event);
    }

    return;
}

void NTPClient::slotNtpConnected()
{
    NTPPackage requestPackage = generateRequestPackage();
    QByteArray bytesToSend    = requestPackage.toByteArray();

    // Save the timestamp byte of the sent packet, used to verify the received packet

    m_originTimestampByte      = requestPackage.m_requestLocalTimestampByte;

    m_udpsocket->flush();
    m_udpsocket->write(bytesToSend, 48);
    m_udpsocket->flush();
}

void NTPClient::slotNtpError(QAbstractSocket::SocketError error)
{
    m_failedTimes++;
    m_offset = 0;
    m_done   = false;
    releaseSocket();

    qDebug() << "NTPClient::onNtpError: " << error << ", failed times:" << m_failedTimes;

    delayResend();
}

void NTPClient::slotNtpReadyRead()
{
    QByteArray data = readSocket();

    if (m_socketTimerID != 0)
    {
        killTimer(m_socketTimerID);
        m_socketTimerID = 0;
    }

    if (data.size() != 48)
    {
        qDebug() << "NTPClient::failed, content error";
        slotNtpError(QAbstractSocket::TemporaryError);

        return;
    }

    NTPPackage responsePackage = generateResponsePackageFromByte(data);

    if (responsePackage.checkByOriginTimestamp(m_originTimestampByte))
    {
        m_failedTimes = 0;
        m_offset      = responsePackage.calcOffset();
        m_done        = true;
        releaseSocket();

        emit signalNtpFinished();

        qDebug() << "NTPClient::ntpFinished, offset:" << m_offset
                 << ", for:" << m_ntpServerHost;
    }
    else
    {
        qDebug() << "NTPClient::failed, not match origin:"
                 << m_originTimestampByte.toHex() << " to :"
                 << responsePackage.m_requestLocalTimestampByte.toHex();

        slotNtpError(QAbstractSocket::TemporaryError);
    }
}

} // namespace QtSampleCodes
