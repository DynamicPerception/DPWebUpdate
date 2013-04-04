#include "avrrunner.h"


#include <QDebug>

AVRRunner::AVRRunner(QObject *parent) : QObject(parent) {

    m_proc = 0;
}

AVRRunner::~AVRRunner() {
    if( m_proc != 0 )
        delete m_proc;
}

void AVRRunner::run(QString p_args, QString p_port, QString p_file) {

    if( m_proc != 0 ) {
        disconnect(m_proc, SIGNAL(readyReadStandardError()), this, SLOT(_readSTDERR()));
        disconnect(m_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(_readSTDOUT()));
        disconnect(m_proc, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(_finished(int,QProcess::ExitStatus)));
        delete m_proc;
    }

    m_proc = new QProcess(this);

    connect(m_proc, SIGNAL(readyReadStandardError()), this, SLOT(_readSTDERR()));
    connect(m_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(_readSTDOUT()));
    connect(m_proc, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(_finished(int,QProcess::ExitStatus)));

    QString avrPath = QCoreApplication::applicationDirPath() + "/avrdude/" + AVR_EXENAME;

    QStringList argList = _processArgs(p_args, p_port, p_file);

    qDebug() << "AVRRunner: Starting" << avrPath << argList;

    m_proc->start(avrPath, argList);
}

void AVRRunner::_readSTDERR() {
    QByteArray content = m_proc->readAllStandardError();
    qDebug() << "AVRRunner: Got STDERR:" << content;
    emit avrOutput(content);
    _checkOutput(content);
}

void AVRRunner::_readSTDOUT() {
    QByteArray content = m_proc->readAllStandardOutput();
    qDebug() << "AVRRunner: Got STDOUT:" << content;
    emit avrOutput(content);
    _checkOutput(content);
}

void AVRRunner::_finished(int p_code, QProcess::ExitStatus p_stat) {
    qDebug() << "AVRRunner: Process Exited with" << p_code << p_stat;
    bool sstat = p_code == 0 ? true : false;
    emit finished(sstat);
}


void AVRRunner::_checkOutput(QByteArray p_content) {
    QString str(p_content);

    if( str.contains("avrdude: writing flash") )
        emit status(WritingFlash);
    if( str.contains("bytes of flash written") )
        emit status(FlashWritten);
    if( str.contains("avrdude: verifying flash memory") )
        emit status(FlashVerifying);
    if( str.contains("bytes of flash verified") )
        emit status(FlashVerified);

}

QStringList AVRRunner::_processArgs(QString p_args, QString p_port, QString p_file) {
    QString avrConf = QCoreApplication::applicationDirPath() + "/avrdude/" + "avrdude.conf";

    p_args.replace("%c", avrConf);
    p_args.replace("%p", p_port);
    p_args.replace("%f", p_file);

    QStringList argList = p_args.split(";");

    return argList;
}
