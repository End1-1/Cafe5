#include "c5licensedlg.h"
#include "ui_c5licensedlg.h"

C5LicenseDlg::C5LicenseDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::C5LicenseDlg)
{
    ui->setupUi(this);
}

C5LicenseDlg::~C5LicenseDlg()
{
    delete ui;
}
