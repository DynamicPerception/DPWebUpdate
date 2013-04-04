#include "updateindex.h"

#include <QDebug>

UpdateIndex::UpdateIndex(Config* c_conf, QObject *parent) : QObject(parent) {

    m_conf       = c_conf;
    m_net        = new QNetworkAccessManager(this);
    m_lastUpdate = QDateTime::fromTime_t(0);
    m_updRep     = 0;
    m_fileRep    = 0;
    m_inUpd      = false;
    m_cancel     = false;

    connect(m_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(_netReply(QNetworkReply*)));

}


UpdateIndex::~UpdateIndex() {

    if( m_updRep != 0 ) {

        if( m_updRep->isRunning() )
            m_updRep->abort();

        delete m_updRep;
    }

    if( m_fileRep != 0 ) {

        if( m_fileRep->isRunning() )
            m_fileRep->abort();

        delete m_fileRep;
    }

    delete m_net;
}


/** Check for Updates

  Issues a request for any changes to the Update Index.

  All network activity is non-blocking and asynchronous.  Watch for the updateComplete signal
  to be emitted when done.

  If any errors occur during processing, an error dialog will be presented to the user.

  If you attempt to issue a new checkUpdate() request while a previous one is still running,
  this slot will return immediately and nothing will happen.

  */

void UpdateIndex::checkUpdates() {

    if( m_inUpd == true )
        return;

    m_inUpd      = true;
    m_cancel     = false;
    m_url        = m_conf->url();
    m_lastUpdate = m_conf->updateTime();

    qDebug() << "UpdateIndex: Check Update from" << m_url << m_lastUpdate.toString();

    emit taskStarted("Checking Index File");

    m_updRep     = m_net->head(QNetworkRequest(QUrl(m_url)));

}

/** Cancel Running Update Slot

  Cancels a running update.  Immediately stops any running downloads, but will not
  stop local operations in progress (such as writing or reading a file).

  The updateComplete() signal will be sent pretty much immediately.

  */

void UpdateIndex::cancelUpdate() {

    m_cancel = true;

    if( m_updRep != 0 && m_updRep->isRunning() )
        m_updRep->abort();

    if( m_fileRep != 0 && m_fileRep->isRunning() )
        m_fileRep->abort();

    emit updateComplete(false);
}

/** Get All Devices in Update Index

  Gets information about all devices and updates from the Update Index.

  You should most definitely call this after receiving an updateCompleted() signal.
  */

QList<IndexItem> UpdateIndex::getDevices() {
    return m_devices;
}


/** Permanently Remove all Cached Files

  Static method.

  Does what it says.  Obliterates the local UpdateIndex.  No going back from here.
  */

void UpdateIndex::flushCache() {
    QString dir = QCoreApplication::applicationDirPath() + "/" + UI_FILE_PATH;
    _removeDir(dir);
}

void UpdateIndex::_indexData() {
    QByteArray data = m_curRep->readAll();

    qDebug() << "UpdateIndex: Received" << data.length() << "Bytes";

    m_indexData.append(data);
}

void UpdateIndex::_downloadProgress(qint64 p_recv, qint64 p_total) {
    int prog;

    if( p_total < 1 || p_recv < 1 )
        prog = 0;
    else
        prog = ( ((float) p_recv / (float) p_total) * 100.0 ) ;

    emit taskProgress(prog);

}

void UpdateIndex::_netReply(QNetworkReply *p_reply) {

        // Stop working if we were cancelled
    if( m_cancel == true ) {
        m_inUpd = false;
        return;
    }


        // check to see if this is a reply to our index update request
    if( p_reply == m_updRep ) {

          // this is one of our index update requests


        QString idxFile = QCoreApplication::applicationDirPath() + "/" + UI_FILE_PATH + "/" + "index.xml";

            // ouch, an error?!
        if( p_reply->error() != QNetworkReply::NoError ) {

            QString err = p_reply->errorString();
            _error(err);

            emit error(err);

            disconnect(m_updRep, SIGNAL(readyRead()), this, SLOT(_indexData()));
            m_inUpd  = false;

            emit updateComplete(false);
            return;
        }

        if( p_reply->operation() == QNetworkAccessManager::HeadOperation ) {

            // this was our HEAD request, check last modified header
            QDateTime lastMod = p_reply->header(QNetworkRequest::LastModifiedHeader).toDateTime();

            bool doGet = true;

            if( m_lastUpdate >= lastMod) {
                m_inUpd = false;

                bool success = _readfile(idxFile, m_indexData);

                if( success ) {
                    _processIndex();
                    doGet = false;

                    qDebug() << "UpdateIndex: Index has not changed since last update.";
                    emit updateComplete(false);
                }

            }

            if( doGet ) {
                // we need to get the index...
                m_lastUpdate = lastMod;
                m_updRep     = m_net->get(QNetworkRequest(QUrl(m_url)));

                m_indexData.clear();

                emit taskStarted("Retrieving Remote Index File: " + m_url);

                m_curRep = m_updRep;

                connect(m_updRep, SIGNAL(readyRead()), this, SLOT(_indexData()));
                connect(m_updRep, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_downloadProgress(qint64,qint64)));

            }
        } // end if head operation
        else {
            // this was our GET request for the update index

            emit taskStarted("Processing Index File");

            disconnect(m_updRep, SIGNAL(readyRead()), this, SLOT(_indexData()));
            disconnect(m_updRep, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_downloadProgress(qint64,qint64)));

            qDebug() << "UpdateIndex: Got new Index" << m_indexData;

                // save xml data to disk

            _writeFile(idxFile, m_indexData);
            _processIndex();

        }
    } // end if p_reply == m_updRep
    else if( p_reply == m_fileRep ) {
        // completed retrieving a file

        if( p_reply->error() != QNetworkReply::NoError ) {

            QString err = p_reply->errorString();
            qDebug() << "UpdateIndex: Got Error for file" << err;
            emit error(err);

        }
        else {
                // write file to disk
            _writeFile(m_curFile, m_indexData);
        }
            // get next file (if any)
        _retrieveNext();
    }
}


void UpdateIndex::_parseXML() {

    QDomDocument doc;

    qDebug() << "UpdateIndex: Parsing XML Data";

    QString error = "";
    int errLine   = 0;
    int errCol    = 0;
    bool success  = doc.setContent(m_indexData, false, &error, &errLine, &errCol);

    if( ! success ) {
        QString msg = "Error Processing Update XML:\nLine " + QString::number(errLine) + ", Column " + QString::number(errCol) + "\n" + error;
        _error(msg);
    }

    m_devices.clear();

    QDomNodeList devs = doc.elementsByTagName("device");

    qDebug() << "UpdateIndex: Found " << devs.count() << "devices";

    for( int i = 0; i < devs.count(); i++ ) {

        QDomNode node = devs.item(i);

        if( node.isNull() )
            break;

        if( node.isElement() ) {
            QDomElement e = node.toElement();

            IndexItem device;

            for( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() ) {

                qDebug() << "Found Child Tag" << n.nodeName();

                if( n.nodeName() == "name" )
                    device.deviceName = n.toElement().text();
                else if( n.nodeName() == "desc" )
                    device.deviceDesc = n.toElement().text();
                else if( n.nodeName() == "uploadParams")
                    device.uploadParams = n.toElement().text();
                else if( n.nodeName() == "updates" ) {

                    QDomElement upElem = n.toElement();
                    QHash< QString, DeviceUpdate > updates = _parseUpdates(&upElem);
                    device.deviceUpdates = updates;

                }

            } // end for(QDomNode...

            m_devices.append(device);

            qDebug() << "UpdateIndex: New Device Created in Index:" << device.deviceName << device.deviceDesc << device.uploadParams;

            foreach( QString key, device.deviceUpdates.keys() ) {
                DeviceUpdate info = device.deviceUpdates.value(key);
                qDebug() << "UpdateIndex: Device Update:" << key << info.updateDesc << info.imagePath;
            }
        }

    }
}

QHash<QString, DeviceUpdate> UpdateIndex::_parseUpdates(QDomElement *p_elem) {

    QDomNodeList updates = p_elem->elementsByTagName("update");
    QHash<QString, DeviceUpdate> ret;

    qDebug() << "UpdateIndex: Found" << updates.count() << "updates for this device type";

    for( int i = 0; i < updates.count(); i++ ) {
        QDomNode node = updates.item(i);

        if( node.isNull() )
            break;

        if( node.isElement() ) {
            QDomElement e = node.toElement();

            QString version;
            DeviceUpdate info;

            for( QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling() ) {

                if( n.nodeName() == "version" )
                    version = n.toElement().text();
                else if( n.nodeName() == "desc" )
                    info.updateDesc = n.toElement().text();
                else if( n.nodeName() == "file" )
                    info.imagePath = n.toElement().text();
            }

            ret.insert(version, info);
        }

    }

    return ret;
}

void UpdateIndex::_error(QString p_err) {
    ErrorDialog dia(p_err);
    dia.exec();
}

/** Convert URL to Local Image Path

  Static method.

  Converts an update image URL to a local file path.

  @param p_path
  The URL of the image

  @return
  The local path to the image
  */

QString UpdateIndex::localPath(QString p_path) {
    // remove http://site/ from the url

    QRegExp rx("^http://.*/");
    rx.setMinimal(true);
    p_path.remove(rx);

     // create a path to the local file
    QString lPath = QCoreApplication::applicationDirPath() + "/" + UI_FILE_PATH + "/" + p_path;
    return lPath;
}

void UpdateIndex::_retrieveFile(QString p_path) {
    QString lPath = localPath(p_path);

    QFile localFile(lPath);

    emit taskStarted("Checking Local File: " + p_path);

    if( localFile.exists() ) {
        qDebug() << "UpdateIndex: File" << lPath << "already exists, will not fetch.";
        _retrieveNext();
    }
    else {
        qDebug() << "UpdateIndex: File" << lPath << "does not exist, will fetch.";

        emit taskStarted("Retrieving Remote File: " + p_path);

        m_curFile   = lPath;
        m_fileRep   = m_net->get(QNetworkRequest(QUrl(p_path)));
        m_curRep    = m_fileRep;

        m_indexData.clear();

        disconnect(m_fileRep, SIGNAL(readyRead()), this, SLOT(_indexData()));
        disconnect(m_fileRep, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_downloadProgress(qint64,qint64)));

        connect(m_fileRep, SIGNAL(readyRead()), this, SLOT(_indexData()));
        connect(m_fileRep, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(_downloadProgress(qint64,qint64)));

    }
}

void UpdateIndex::_retrieveNext() {

    emit tasksRemaining(m_getFiles.count());

    if( m_getFiles.count() > 0 ) {
        QString file = m_getFiles.takeFirst();
        _retrieveFile(file);
    }
    else {
        m_inUpd = false;
        // record last complete update time
        m_conf->updateTime(m_lastUpdate);
        emit updateComplete(true);
    }
}

void UpdateIndex::_writeFile(QString p_path, QByteArray &p_data) {

    QFileInfo pathInfo(p_path);
    QString dirPath = pathInfo.path();
    QDir dir;

    qDebug() << "UpdateIndex: Checking for existing directory and creating if need be:" << dirPath;

        // make the directory path if it does not exist

    dir.mkpath(dirPath);

    QFile file(p_path);

    if( ! file.open(QIODevice::WriteOnly) ) {
        qDebug() << "UpdateIndex: Got Error" << file.error();
        return;
    }

    qDebug() << "UpdateIndex: Writing" << p_data.length() << "Bytes to File";

    file.write(p_data);
    file.close();
}


bool UpdateIndex::_readfile(QString p_path, QByteArray &p_data) {

    QFile file(p_path);

    if( ! file.open(QIODevice::ReadOnly) ) {
        qDebug() << "UpdateIndex: Could not open" << p_path << "for writing, got" << file.error();
        return false;
    }

    p_data = file.readAll();
    file.close();

    return true;
}

void UpdateIndex::_processIndex() {
        // process xml
    _parseXML();

        // process referenced files, make sure we retrieve nay we're missing
    m_getFiles.clear();

        // create a list of files to check for/get
    foreach( IndexItem device, m_devices ) {
        foreach( QString version, device.deviceUpdates.keys() )
            m_getFiles.append( device.deviceUpdates.value(version).imagePath );
    }

    _retrieveNext();

}


/* Recursively Delete a Directory and All Contents

  unabashedly taken and adapted from http://john.nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
  */

bool UpdateIndex::_removeDir(QString p_dir) {
    bool result = true;
    QDir dir(p_dir);

    if (dir.exists(p_dir)) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = _removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(p_dir);
    }

    return result;

}
