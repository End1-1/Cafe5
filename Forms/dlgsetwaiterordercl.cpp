#include "dlgsetwaiterordercl.h"
#include "ui_dlgsetwaiterordercl.h"
#include "dlgnewcl.h"

DlgSetWaiterOrderCL::DlgSetWaiterOrderCL(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgSetWaiterOrderCL)
{
    ui->setupUi(this);
    C5Database db(dbParams);
    db.exec(QString("select f_id, f_name from %1.f_city_ledger ").arg(__c5config.hotelDatabase()));
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, db.getString(0));
        ui->tbl->setString(r, 1, db.getString(1));
    }
}

DlgSetWaiterOrderCL::~DlgSetWaiterOrderCL()
{
    delete ui;
}

bool DlgSetWaiterOrderCL::getCL(const QStringList &dbParams, QString &code, QString &name)
{
    DlgSetWaiterOrderCL d(dbParams);
    if (d.exec() == QDialog::Accepted) {
        code = d.property("code").toString();
        name = d.property("name").toString();
        return true;
    }
    return false;
}

void DlgSetWaiterOrderCL::on_btnNew_clicked()
{
    QString code, name;
    if (DlgNewCL::createCL(fDBParams, code, name)) {
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setString(r, 0, code);
        ui->tbl->setString(r, 1, name);
        ui->leSearch->setText(code);
    }
}

void DlgSetWaiterOrderCL::on_leSearch_textChanged(const QString &arg1)
{
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        bool h = true;
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            if (ui->tbl->getString(r, c).contains(arg1, Qt::CaseInsensitive)) {
                h = false;
                break;
            }
        }
        ui->tbl->setRowHidden(r, h);
    }
}

void DlgSetWaiterOrderCL::on_tbl_doubleClicked(const QModelIndex &index)
{
    if (index.isValid() == false) {
        return;
    }
    setProperty("code", ui->tbl->getString(index.row(), 0));
    setProperty("name", ui->tbl->getString(index.row(), 1));
    accept();
}
