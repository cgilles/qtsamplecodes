/* ============================================================
 *
 * Date        : 2020-11-20
 * Description : unit test of QProcess with lambda function to
 *               catch output stream in QDebug.
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
#include <QProcess>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QElapsedTimer>

int main(int argc, char** argv)
{
    if (argc <= 1)
    {
        qDebug() << "Pass process name and arguments on CLI...";

        return -1;
    }

    QCoreApplication app(argc, argv);
    QStringList args     = app.arguments().mid(1);

    // ---

    QProcess* const proc = new QProcess(qApp);

#ifdef Q_OS_WIN

    proc->setProgram(QLatin1String("cmd"));
    proc->setArguments(QStringList() << QLatin1String("/c") << args);

#else   // Linux

    proc->setProgram(args.value(0));
    proc->setArguments(args.mid(1));

#endif

    qInfo() << "=== Starting process:" << proc->program() << proc->arguments();

    proc->setProcessChannelMode(QProcess::MergedChannels);

    QObject::connect(proc, &QProcess::readyRead,
                     [proc]()
        {
            QByteArray data = proc->readAll();

            if (!data.isEmpty())
            {
                QString str       = QString::fromLocal8Bit(data.data(), data.size());
                QStringList lines = str.split(QLatin1Char('\n'));

                foreach (const QString& str, lines)
                {
                    if (!str.isEmpty())
                    {
                        qDebug().noquote() << str;
                    }
                }
            }
        }
    );

    QElapsedTimer etimer;
    etimer.start();

    proc->start();

    if (proc->waitForStarted())
    {
        bool timedOut = !proc->waitForFinished(30000);
        int  exitCode = proc->exitCode();

        qInfo() << "=== Process execution is complete!";
        qInfo() << "> Process timed-out        :" << timedOut;
        qInfo() << "> Process exit code        :" << exitCode;
        qInfo() << "> Process elasped time (ms):" << etimer.elapsed();
    }
    else
    {
        qWarning() << "=== Process execution failed!";
        return (-1);
    }

    // ---

    return (0);
}
