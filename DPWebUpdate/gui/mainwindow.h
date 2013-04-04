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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "core/Config/config.h"
#include "core/UpdateIndex/updateindex.h"
#include "core/Dialogs/updatedialog.h"
#include "core/Dialogs/confirmdialog.h"
#include "core/Dialogs/aboutdialog.h"
#include "core/AVRRunner/avrrunner.h"
#include "core/Themer/singlethemer.h"

#include "qextserialenumerator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void updateSerialPorts();


public slots:

        // inputs
    void on_rescanButton_clicked();
    void on_updateButton_clicked();
    void on_deviceCombo_currentIndexChanged(int p_idx);
    void on_versionCombo_currentIndexChanged(int p_idx);
    void on_outputButton_clicked();
    void on_portCombo_currentIndexChanged(int p_idx);

        // actions
    void on_actionCheckforUpdates_triggered();
    void on_actionClearCache_triggered();
    void on_actionAbout_triggered();

private slots:
    void _updateCompleted(bool p_stat);
    void _avrOutput(QByteArray p_data);
    void _avrDone(bool p_stat);
    void _avrStatus(AVRRunner::Status p_stat);
    
private:
    Ui::MainWindow *ui;

    Config* m_conf;
    UpdateIndex* m_upd;
    AVRRunner* m_avr;

    QList<IndexItem> m_devices;
    int m_curDevice;
    int m_curUpdate;
    bool m_outputHidden;

    void _prepInputs();
};

#endif // MAINWINDOW_H
