#include "dlgeditphone.h"
#include "ui_dlgeditphone.h"

DlgEditPhone::DlgEditPhone(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgEditPhone)
{
    ui->setupUi(this);
}

DlgEditPhone::~DlgEditPhone()
{
    delete ui;
}
