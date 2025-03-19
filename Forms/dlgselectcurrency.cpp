#include "dlgselectcurrency.h"
#include "ui_dlgselectcurrency.h"

DlgSelectCurrency::DlgSelectCurrency(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::DlgSelectCurrency)
{
    ui->setupUi(this);
    C5Database db(dbParams);
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

bool DlgSelectCurrency::getCurrency(const QStringList &dbParams, int &id, QString &name, QWidget *parent)
{
    DlgSelectCurrency d(dbParams, parent);
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
