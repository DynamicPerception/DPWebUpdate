/*

  DPWebUpdate Application

  Copyright (c) 2013 Dynamic Perception

 This file is part of DPWebUpdate.

    DPWebUpdate is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DPWebUpdate is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with DPWebUpdate.  If not, see <http://www.gnu.org/licenses/>.

    */


#ifndef AVRRUNNER_H
#define AVRRUNNER_H

#include <QObject>
#include <QProcess>
#include <QCoreApplication>



#ifdef OS_MACX
    const QString AVR_EXENAME = "avrdude";
#endif

#ifdef Q_WS_X11
    const QString AVR_EXENAME = "avrdude";
#endif

#ifdef Q_WS_MAC
    const QString AVR_EXENAME = "avrdude";
#endif

#ifdef Q_WS_QWS
    const QString AVR_EXENAME = "avrdude";
#endif

#ifdef Q_WS_WIN
    const QString AVR_EXENAME = "avrdude.exe";
#endif


class AVRRunner : public QObject
{
    Q_OBJECT
public:
    explicit AVRRunner(QObject *parent = 0);
    ~AVRRunner();
    
    void run(QString p_args, QString p_port, QString p_file);

        /** Status Flags */
    enum Status {
        WritingFlash,
        FlashWritten,
        FlashVerifying,
        FlashVerified
    };

signals:

        /** Output Signal
          Triggered every time output is read from the avrdude program
          */
    void avrOutput(QByteArray p_output);

        /** Finished signal
          Triggered every time the avrdude process finishes.

          @param p_stat
          Whether or not avrdude successfully completed its task.
          */
    void finished(bool p_stat);

        /** Status signal
          Triggered regularly during upload.  Indicates current step.
          */
    void status(AVRRunner::Status p_stat);

public slots:

private slots:

    void _readSTDERR();
    void _readSTDOUT();
    void _finished(int p_code, QProcess::ExitStatus p_stat);
    
private:
    QProcess* m_proc;

    QStringList _processArgs(QString p_args, QString p_port, QString p_file);
    void _checkOutput(QByteArray p_content);

};

#endif // AVRRUNNER_H
