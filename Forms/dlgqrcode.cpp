#include "dlgqrcode.h"
#include "ui_dlgqrcode.h"

DlgQrCode::DlgQrCode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgQrCode)
{
    ui->setupUi(this);
}

DlgQrCode::~DlgQrCode()
{
    delete ui;
}
