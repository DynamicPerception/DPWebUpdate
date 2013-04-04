#include "config.h"


#include <QDebug>

Config::Config(QObject *parent) : QObject(parent) {

    m_iniFile = QCoreApplication::applicationDirPath() + "/" + CONFIG_FILE;
    m_set     = new QSettings(m_iniFile, QSettings::IniFormat);
    m_date    = new QDateTime;

    readConfig();
}


Config::~Config() {
    delete m_set;
    delete m_date;
}

/** Get Last Update Index Modified Time

  Returns the last time the update index was modified.
  */

QDateTime Config::updateTime() {
    return *m_date;
}

/** Set Last Update Index Modified Time

  Sets the last time the update index was modified.

  @param p_time
  A QDateTime object

  @param p_bcast
  (Optional) emit a configChanged() signal after update. Defaults to true.
  */

void Config::updateTime(QDateTime p_time, bool p_bcast) {
    *m_date = p_time;
    m_set->setValue("index_date", m_date->toString());

    if( p_bcast )
        emit configChanged();
}


/** Get Update Index URL

  Returns the URL of the update index.

  @return
  The URL to the index html file.
  */

QString Config::url() {
    return m_indexURL;
}

/** Set Update Index URL

  Sets the URL of the update index. e.g.:

  'http://foo.com/foo/bar/file.xml'

  @param p_url
  The URL to the index xml file

  @param p_broadcast
  (Optional) emit a configChanged() signal after completion?  Defaults to true.

  */

void Config::url(QString p_url, bool p_bcast) {
    m_indexURL = p_url;
    m_set->setValue("index_url", m_indexURL);

    if( p_bcast )
        emit configChanged();
}

/** Read Config from Disk and Update Settings

    Emits the configChanged() signal when complete.

*/
void Config::readConfig() {


    m_indexURL  = m_set->value("index_url", CONFIG_URL).toString();

        // note that default is the beginning of the epoch, so if we're running
        // for the first time (or our ini file has been blown away) we always get
        // updates.

    QString dateString = m_set->value("index_date", QDateTime::fromTime_t(0).toString()).toString();

    *m_date = QDateTime::fromString(dateString);

    qDebug() << "Config: Configuration File Read Complete" << m_indexURL << dateString << m_date->toString();

    emit configChanged();
}
