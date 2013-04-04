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

#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

#include "core/UpdateIndex/updateindex.h"
#include "core/Themer/singlethemer.h"

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT
    
public:
    UpdateDialog(UpdateIndex* c_idx, QWidget *parent = 0);
    ~UpdateDialog();
    
    void hideNoUpdate(bool p_hide);

public slots:

    void taskStarted(QString p_task);
    void taskProgress(int p_prog);
    void tasksRemaining(int p_tasks);
    void error(QString p_error);
    void updateComplete(bool p_updated);

    void on_cancelButton_clicked();
    void on_doneButton_clicked();

private:
    Ui::UpdateDialog *ui;

    UpdateIndex* m_idx;

    int m_taskCount;
    bool m_taskStarted;
    bool m_taskError;
    bool m_hide;

};

#endif // UPDATEDIALOG_H
