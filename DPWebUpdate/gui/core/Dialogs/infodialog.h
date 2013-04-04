#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>

#include "core/Themer/singlethemer.h"

namespace Ui {
class InfoDialog;
}

class InfoDialog : public QDialog
{
    Q_OBJECT
    
public:
    InfoDialog(QString c_title, QString c_text, QWidget *parent = 0);
    ~InfoDialog();
    
private:
    Ui::InfoDialog *ui;
};

#endif // INFODIALOG_H
