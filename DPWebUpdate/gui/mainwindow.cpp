#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_conf = new Config(this);
    m_upd  = new UpdateIndex(m_conf, this);
    m_avr  = new AVRRunner(this);

    m_curDevice    = 0;
    m_curUpdate    = 0;
    m_outputHidden = true;

    connect(m_upd, SIGNAL(updateComplete(bool)), this, SLOT(_updateCompleted(bool)));

    connect(m_avr, SIGNAL(avrOutput(QByteArray)), this, SLOT(_avrOutput(QByteArray)));
    connect(m_avr, SIGNAL(finished(bool)), this, SLOT(_avrDone(bool)));
    connect(m_avr, SIGNAL(status(AVRRunner::Status)), this, SLOT(_avrStatus(AVRRunner::Status)));

    UpdateDialog updateScreen(m_upd, this);

    updateSerialPorts();

    m_upd->checkUpdates();

    updateScreen.hideNoUpdate(true);
    updateScreen.exec();

    ui->actProgress->setRange(0, 4);
    ui->outputText->hide();

        // apply themeing
    setStyleSheet(SingleThemer::getStyleSheet("main"));
}

MainWindow::~MainWindow() {
    delete m_avr;
    delete m_upd;
    delete m_conf;
    delete ui;
}


void MainWindow::_updateCompleted(bool p_stat) {
    Q_UNUSED(p_stat);

    qDebug() << "MainWindow: Got Update Completed";
    m_devices = m_upd->getDevices();
    _prepInputs();
}

void MainWindow::_prepInputs() {

    ui->deviceCombo->clear();
    ui->versionCombo->clear();

    foreach(IndexItem item, m_devices) {
        ui->deviceCombo->addItem(item.deviceName);
    }


    if( m_devices.count() > 0 ) {
        ui->deviceCombo->setCurrentIndex(0);
    }
}

void MainWindow::_avrOutput(QByteArray p_data) {
    ui->outputText->append(p_data);
}

void MainWindow::_avrDone(bool p_stat) {
    if( p_stat )
        ui->actLabel->setText("Upload Successful");
    else
        ui->actLabel->setText("Upload Failed");

    ui->actProgress->setValue(4);
    ui->updateButton->setEnabled(true);
}

void MainWindow::_avrStatus(AVRRunner::Status p_stat) {
    if( p_stat == AVRRunner::WritingFlash ) {
        ui->actLabel->setText("Writing Flash");
        ui->actProgress->setValue(1);
    }
    else if( p_stat == AVRRunner::FlashWritten ) {
        ui->actLabel->setText("Flash Written");
        ui->actProgress->setValue(2);
    }
    else if( p_stat == AVRRunner::FlashVerifying ) {
        ui->actLabel->setText("Verifying Flash");
        ui->actProgress->setValue(3);
    }
    else if( p_stat == AVRRunner::FlashVerified ) {
        ui->actLabel->setText("Flash Verified");
        ui->actProgress->setValue(4);
    }

}

/** Update Com Port List

  Updates the serial port combo box with a list of all currently found
  serial ports.
  */

void MainWindow::updateSerialPorts() {
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    ui->portCombo->clear();

    foreach( QextPortInfo port, ports ) {

        ui->portCombo->addItem(port.portName, QVariant(port.friendName));
    }

    ui->portHintLabel->setText(ui->portCombo->itemData(ui->portCombo->currentIndex()).toString());


}

void MainWindow::on_actionClearCache_triggered() {
    ConfirmDialog confirm("This will remove all locally cached files, are you sure?");

    int ret = confirm.exec();

    if( ret == QDialog::Accepted ) {
        UpdateIndex::flushCache();

        UpdateDialog updateScreen(m_upd, this);

        m_upd->checkUpdates();

        updateScreen.exec();
    }
}

void MainWindow::on_actionCheckforUpdates_triggered() {
    UpdateDialog updateScreen(m_upd, this);

    m_upd->checkUpdates();

    updateScreen.exec();

}

void MainWindow::on_actionAbout_triggered() {
    AboutDialog about;
    about.exec();
}


void MainWindow::on_deviceInfoButton_clicked() {

    QString info = m_devices.at(m_curDevice).deviceDesc;
    QString name = m_devices.at(m_curDevice).deviceName;

    InfoDialog show(name, info, this);
    show.exec();
}

void MainWindow::on_versionInfoButton_clicked() {
    QList<QString> upds = m_devices.at(m_curDevice).deviceUpdates.keys();
    QString        name = upds.at(m_curUpdate);
    QString        info = m_devices.at(m_curDevice).deviceUpdates.value(name).updateDesc;

    InfoDialog show(name, info, this);
    show.exec();

}

void MainWindow::on_outputButton_clicked() {
    if( ! m_outputHidden ) {
        ui->outputText->hide();
        ui->outputButton->setText("Show Output");
    }
    else {
        ui->outputButton->setText("Hide Output");
        ui->outputText->show();
        ui->outputText->setReadOnly(true);
    }

    m_outputHidden = ! m_outputHidden;
}

/** rescanButton click slot */
void MainWindow::on_rescanButton_clicked() {
    updateSerialPorts();
}

void MainWindow::on_updateButton_clicked() {


    ui->actProgress->setValue(0);
    ui->actLabel->setText("Uploading...");

    QList<QString> upds = m_devices.at(m_curDevice).deviceUpdates.keys();
    QString     verName = upds.at(m_curUpdate);

    QString params = m_devices.at(m_curDevice).uploadParams;
    QString   file = m_devices.at(m_curDevice).deviceUpdates.value(verName).imagePath;
    QString   port = ui->portCombo->currentText();

        // convert to local path
    file = UpdateIndex::localPath(file);

    m_avr->run(params, port, file);
    ui->updateButton->setEnabled(false);
}

void MainWindow::on_portCombo_currentIndexChanged(int p_idx) {
    qDebug() << "MainWindow: Got Port Change" << p_idx;

    ui->portHintLabel->setText(ui->portCombo->itemData(p_idx).toString());

}

void MainWindow::on_deviceCombo_currentIndexChanged(int p_idx) {

    qDebug() << "MainWindow: Got Device Change" << p_idx;
    if( p_idx < 0 )
        return;

    if( m_devices.count() < p_idx )
        return;

    m_curDevice = p_idx;

    ui->versionCombo->clear();

    QHash<QString, DeviceUpdate> upds = m_devices.at(p_idx).deviceUpdates;

    foreach( QString version,  upds.keys() ) {
        qDebug() << "MainWindow: Found Version" << version;
        ui->versionCombo->addItem(version);
    }

        // choose newest version
    ui->versionCombo->setCurrentIndex( upds.count() - 1 );

    m_curUpdate = upds.count() - 1;

}

void MainWindow::on_versionCombo_currentIndexChanged(int p_idx) {
    qDebug() << "MainWindow: Got New Version Index" << p_idx;
    m_curUpdate = p_idx;
}
