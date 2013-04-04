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


#ifndef UPDATEINDEX_H
#define UPDATEINDEX_H

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QDateTime>
#include <QNetworkReply>
#include <QByteArray>
#include <QDomDocument>
#include <QHash>
#include <QRegexp>
#include <QFile>
#include <QDir>
#include <QFileInfo>

#include "core/Dialogs/errordialog.h"
#include "core/Config/config.h"


const QString UI_FILE_PATH = "UpdateIndex";

 /** Information about a Specific Update */
struct DeviceUpdate {
    QString updateDesc;
    QString imagePath;
};

/** An Update Index Item

  Each update index item describes a device and the available updates for it.
  */
struct IndexItem {

    QString deviceName;
    QString deviceDesc;
    QString uploadParams;
        /** A QHash with a version string pointing to the DeviceUpdate information */
    QHash<QString, DeviceUpdate > deviceUpdates;
};

/** Update Index Class

  This class handles accessing, downloading, and managing the remote Update Index.

  All network access is managed in this class, and all methods for accessing and
  retrieving the update index are handled through this class.

  @author
  C. A. Church
  */

class UpdateIndex : public QObject
{
    Q_OBJECT
public:
    UpdateIndex(Config* c_conf, QObject *parent = 0);
    ~UpdateIndex();
    
    QList<IndexItem> getDevices();
    static QString localPath(QString p_path);
    static void flushCache();

signals:
    
    /** Update Complete Signal

      When an update check request is completed, this signal is emitted.

      @param p_updated
      Whether or not new files were downloaded
      */
    void updateComplete(bool p_updated);

    /** New Task Started Signal

      When a task is started, this signal is sent with the name of the task
      */
    void taskStarted(QString p_file);

    /** Task Progress Signal

      When a task is in progress, this signal will be emitted with the progress,
      from 0-100 of the task.
      */
    void taskProgress(int p_progress);

    /** Error Occurred Signal

      Ouch, that must hurt.  Well, at least you'll learn what happened!
      */
    void error(QString p_error);

    /** Tasks Remaining Signal

      Indicates how many remaining tasks exist at each point triggered.
      */
    void tasksRemaining(int p_tasks);

public slots:
    void checkUpdates();
    void cancelUpdate();

private slots:
    void _netReply(QNetworkReply* p_reply);
    void _indexData();
    void _downloadProgress(qint64 p_recv, qint64 p_total);

private:

    Config* m_conf;
    QNetworkAccessManager* m_net;
    QDateTime m_lastUpdate;
    QNetworkReply* m_updRep;
    QNetworkReply* m_fileRep;
    QNetworkReply* m_curRep;
    QString m_url;
    QByteArray m_indexData;
    QList<IndexItem> m_devices;
    QList<QString> m_getFiles;
    QString m_curFile;

    bool m_inUpd;
    bool m_cancel;

    void _parseXML();
    void _error(QString p_err);
    QHash<QString, DeviceUpdate> _parseUpdates(QDomElement* p_elem);
    void _retrieveFile(QString p_path);
    void _retrieveNext();
    void _writeFile(QString p_path, QByteArray &p_data);
    bool _readfile(QString p_path, QByteArray &p_data);
    void _processIndex();
    static bool _removeDir(QString p_dir);

};

#endif // UPDATEINDEX_H
