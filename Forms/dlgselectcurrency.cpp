#include "dlgselectcurrency.h"
#include "ui_dlgselectcurrency.h"
#include "c5database.h"
#include "c5config.h"

DlgSelectCurrency::DlgSelectCurrency(QWidget *parent) :
    C5Dialog(parent),
    ui(new Ui::DlgSelectCurrency)
{
    ui->setupUi(this);
    C5Database db;
    db.exec("select * from e_currency order by f_id");
    while (db.nextRow()) {
        ui->cbCurrency->addItem(db.getString("f_name"), db.getInt("f_id"));
    }
    ui->cbCurrency->setCurrentIndex(ui->cbCurrency->findData(__c5config.getValue(param_default_currency).toInt()));
}

DlgSelectCurrency::~DlgSelectCurrency()
{
    delete ui;
}

bool DlgSelectCurrency::getCurrency(int &id, QString &name, QWidget *parent)
{
    DlgSelectCurrency d(parent);
    if (d.exec() == QDialog::Accepted) {
        id = d.ui->cbCurrency->currentData().toInt();
        name = d.ui->cbCurrency->currentText();
        return true;
    }
    return false;
}

void DlgSelectCurrency::on_btnReject_clicked()
{
    reject();
}

void DlgSelectCurrency::on_btnOK_clicked()
{
    accept();
}
