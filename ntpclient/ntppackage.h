/*******************************************************************************
 *
 * Network Time Protocal Qt client
 *
 * 2015-2020 - Gilles Caulier <gilles.caulier@cea.fr>
 *
 * Licensed Material - Program Property of IRFM
 * (c) CEA Cadarache - DRF/CAD/IRFM/STEP/GEAC
 *
 *******************************************************************************/

#ifndef NTP_PACKAGE_H
#define NTP_PACKAGE_H

// Qt includes

#include <QByteArray>
#include <QUdpSocket>

#define UDP_TIMEOUT                 30000
#define UDP_RESEND_INTERVAL_COUNT   6

namespace QtSampleCodes
{

class NTPPackage
{

public:

    explicit NTPPackage();
    ~NTPPackage();

    /**
     * The packets sent are packaged according to the protocol.
     * currentLocalTimestamp is the time when the packet is called by this method.
     */
    QByteArray toByteArray();

    /**
     * Unpack the received package according to the agreement.
     * currentLocalTimestamp is the time when the method is unpacked
     */
    void parseByByteArray(const QByteArray& bytes);


    /**
     * Calculate offset.
     */
    qint64 calcOffset() const;

    /**
     * Parity package
     */
    bool checkByOriginTimestamp(const QByteArray& ots) const;

public:

    /**
     * Ntp protocol data.
     */
    quint8      m_li;
    quint8      m_vn;
    quint8      m_mode;
    quint8      m_stratum;
    quint8      m_poll;
    qint8       m_precision;
    qint32      m_rootdelay;
    qint32      m_rootDispersion;

    /**
     * Ntp server id.
     */
    quint32     m_referenceIdentifier;

    /**
     * The local time of the last synchronization of the Ntp server.
     */
    qint64      m_referenceTimestamp;

    /**
     * Time in the request received by the ntp server-client time when the client initiated the request: t0.
     */
    qint64      m_originTimestamp;

    /**
     * The local time when the Ntp server received the request: t1.
     */
    qint64      m_receiveTimestamp;

    /**
     * The local time when the Ntp server sends the response: t2.
     */
    qint64      m_translateTimestamp;

    /**
     * The byte of the timestamp in the protocol sent by the Ntp client is used for verification.
     */
    QByteArray  m_requestLocalTimestampByte;

    /**
     * Send or receive the local timestamp of the packet, the packet sent as t0, the packet received as t3, used to calculate the time difference.
     */
    qint64      m_currentLocalTimestamp;
};

} // namespace QtSampleCodes

#endif // NTP_PACKAGE_H
