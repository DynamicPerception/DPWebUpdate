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

#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QCoreApplication>
#include <QDateTime>


const QString CONFIG_FILE = "dpwebupdate.ini";
const QString CONFIG_URL  = "http://dynamicperception.com/dpwu/index.xml";

/** Configuration Management Class

  Manages parameters, etc.
  */

class Config : public QObject
{
    Q_OBJECT
public:
    explicit Config(QObject *parent = 0);
    ~Config();
    
    void readConfig();

    void updateTime(QDateTime p_time, bool p_bcast = true);
    QDateTime updateTime();


    void url(QString p_url, bool p_bcast = true);
    QString url();

signals:
    
    void configChanged();

public slots:

private:

    QSettings* m_set;

    QDateTime* m_date;
    QString m_iniFile;
    QString m_indexURL;


};

#endif // CONFIG_H
