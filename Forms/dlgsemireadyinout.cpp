#include "dlgsemireadyinout.h"
#include "ui_dlgsemireadyinout.h"
#include "c5selector.h"

DlgSemireadyInOut::DlgSemireadyInOut(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::DlgSemireadyInOut)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidth(0, 0);
    ui->tbl->setColumnWidth(1, 300);
    ui->tbl->setColumnWidth(2, 80);
}

DlgSemireadyInOut::~DlgSemireadyInOut()
{
    delete ui;
}

int DlgSemireadyInOut::rowCount()
{
    return ui->tbl->rowCount();
}

int DlgSemireadyInOut::goodsId(int r)
{
    return ui->tbl->getInteger(r, 0);
}

QString DlgSemireadyInOut::goodsName(int r)
{
    return ui->tbl->getString(r, 1);
}

double DlgSemireadyInOut::goodsQty(int r)
{
    return ui->tbl->getDouble(r, 2) * (ui->rbIncrease->isChecked() ? 1 : -1);
}

void DlgSemireadyInOut::on_btnSelectGoods_clicked()
{
    QList<QVariant> vals;
    QHash<QString, QString> trans;
    if (!C5Selector::getValues(fDBParams,
                               "select distinct(g.f_id), g.f_name, g.f_complectout "
                               "from c_goods_complectation gc "
                               "left join c_goods g on g.f_id=gc.f_base", vals, trans)) {
        return;
    }
    ui->leQty->setProperty("qty", vals.at(3));
    ui->leQty->setDouble(vals.at(3).toDouble());
    ui->leGoods->setText(vals.at(2).toString());
    C5Database db(fDBParams);
    db[":f_base"] = vals.at(1);
    db.exec("select g.f_name, gc.f_goods, gc.f_qty "
            "from c_goods_complectation gc "
            "left join c_goods g on g.f_id=gc.f_goods "
            "where gc.f_base=:f_base ");
    while (db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setInteger(r, 0, db.getInt("f_goods"));
        ui->tbl->setString(r, 1, db.getString("f_name"));
        ui->tbl->setDouble(r, 2, db.getDouble("f_qty"));
        ui->tbl->item(r, 2)->setData(Qt::UserRole, db.getDouble("f_qty"));
    }
}

void DlgSemireadyInOut::on_btnCancel_clicked()
{
    reject();
}

void DlgSemireadyInOut::on_leQty_textEdited(const QString &arg1)
{
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->setDouble(i, 2, (ui->tbl->item(i, 2)->data(Qt::UserRole).toDouble() / ui->leQty->property("qty").toDouble()) * str_float(arg1));
    }
}

void DlgSemireadyInOut::on_btnOk_clicked()
{
    accept();
}