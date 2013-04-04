#include "infodialog.h"
#include "ui_infodialog.h"

InfoDialog::InfoDialog(QString c_title, QString c_text, QWidget *parent) : QDialog(parent), ui(new Ui::InfoDialog) {
    ui->setupUi(this);

    ui->titleLabel->setText(c_title);
    ui->infoText->setText(c_text);
        // apply themeing
    setStyleSheet(SingleThemer::getStyleSheet("info"));

}

InfoDialog::~InfoDialog() {
    delete ui;
}
