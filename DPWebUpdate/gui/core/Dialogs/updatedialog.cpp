#include "updatedialog.h"
#include "ui_updatedialog.h"

#include <QDebug>

UpdateDialog::UpdateDialog(UpdateIndex *c_idx, QWidget *parent) : QDialog(parent), ui(new Ui::UpdateDialog) {
    ui->setupUi(this);

    m_idx         = c_idx;
    m_taskCount   = 1;
    m_taskStarted = false;
    m_taskError   = false;
    m_hide        = false;

    connect(m_idx, SIGNAL(taskStarted(QString)), this, SLOT(taskStarted(QString)));
    connect(m_idx, SIGNAL(taskProgress(int)), this, SLOT(taskProgress(int)));
    connect(m_idx, SIGNAL(tasksRemaining(int)), this, SLOT(tasksRemaining(int)));
    connect(m_idx, SIGNAL(error(QString)), this, SLOT(error(QString)));
    connect(m_idx, SIGNAL(updateComplete(bool)), this, SLOT(updateComplete(bool)));

    // OSX has issues laying out these buttons w/o overlap,
    // this is a work-around
#ifdef Q_WS_MAC
    ui->cancelButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
    ui->doneButton->setAttribute(Qt::WA_LayoutUsesWidgetRect);
#endif

        // apply themeing
    setStyleSheet(SingleThemer::getStyleSheet("update"));

    ui->stepProgress->setMinimum(0);
    ui->stepProgress->setMaximum(100);

    ui->totalProgress->setMinimum(0);
    ui->totalProgress->setMaximum(m_taskCount);

    ui->doneButton->setEnabled(false);

}

UpdateDialog::~UpdateDialog() {
    delete ui;
}

void UpdateDialog::on_cancelButton_clicked() {
    m_idx->cancelUpdate();
    done(QDialog::Rejected);
}

void UpdateDialog::on_doneButton_clicked() {
    done(QDialog::Accepted);
}

void UpdateDialog::updateComplete(bool p_updated) {
    ui->doneButton->setEnabled(true);
    ui->cancelButton->setEnabled(false);

    ui->statLabel->setText("Update Completed");

    if( p_updated == false && m_taskError == false && m_hide == true )
        done(QDialog::Accepted);
}

void UpdateDialog::taskProgress(int p_prog) {
    qDebug() << "Update Dialog: Got Task Progress:" << p_prog;
    ui->stepProgress->setValue(p_prog);
}

void UpdateDialog::taskStarted(QString p_task) {
   // if( m_taskStarted && ! m_taskError )
   //     ui->stepText->appendPlainText("\t----> Done!");

    ui->stepText->appendPlainText(p_task);
    ui->stepProgress->setValue(0);

    //m_taskError = false;
    m_taskStarted = true;

}

void UpdateDialog::error(QString p_error) {
    ui->stepText->appendPlainText("\tERROR: " + p_error);
    m_taskError = true;
}

void UpdateDialog::tasksRemaining(int p_tasks) {

    qDebug() << "UpdateDialog: Got Tasks Remaining:" << p_tasks;

    if( p_tasks > m_taskCount ) {
            // incoming value is greater than task count, this happens
            // after we process the index and determine how many files must be checked
        ui->totalProgress->setMaximum(p_tasks);
        ui->totalProgress->setValue(0);
        m_taskCount = p_tasks;
    }
    else {
            // fewer tasks now, which means we're processing through them
        int curTask = m_taskCount - p_tasks;
        ui->totalProgress->setValue(curTask);
    }

}

/** Hide Screen if there are no Updates? */
void UpdateDialog::hideNoUpdate(bool p_hide) {
    m_hide = p_hide;
}
