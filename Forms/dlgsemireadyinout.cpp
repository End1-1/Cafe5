#include "dlgsemireadyinout.h"
#include "ui_dlgsemireadyinout.h"
#include "c5selector.h"
#include "c5database.h"
#include "c5utils.h"

DlgSemireadyInOut::DlgSemireadyInOut(C5User *user) :
    C5Dialog(user),
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

int DlgSemireadyInOut::dishId()
{
    return ui->leGoods->property("f_base").toInt();
}

double DlgSemireadyInOut::dishQty()
{
    return ui->leQty->getDouble();
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
    return ui->tbl->getDouble(r, 2);
}

void DlgSemireadyInOut::on_btnSelectGoods_clicked()
{
    QJsonArray vals;
    QHash<QString, QString> trans;

    if(!C5Selector::getValues(mUser,
                              "select distinct(g.f_id), g.f_name, g.f_complectout, u.f_name as f_unitname "
                              "from c_goods_complectation gc "
                              "left join c_goods g on g.f_id=gc.f_base "
                              "left join c_units u on u.f_id=g.f_unit ", vals, trans)) {
        return;
    }

    ui->tbl->setRowCount(0);
    ui->leQty->setProperty("qty", vals.at(3));
    ui->leQty->setDouble(vals.at(3).toDouble());
    ui->lbMeas->setText(vals.at(4).toString());
    ui->leGoods->setText(vals.at(2).toString());
    C5Database db;
    db[":f_base"] = vals.at(1).toInt();
    db.exec("select g.f_name, gc.f_goods, gc.f_qty "
            "from c_goods_complectation gc "
            "left join c_goods g on g.f_id=gc.f_goods "
            "where gc.f_base=:f_base ");
    ui->leGoods->setProperty("f_base", vals.at(1));
    bool alldone = true;

    while(db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setInteger(r, 0, db.getInt("f_goods"));
        ui->tbl->setString(r, 1, db.getString("f_name"));
        ui->tbl->setDouble(r, 2, db.getDouble("f_qty"));
        ui->tbl->item(r, 2)->setData(Qt::UserRole, db.getDouble("f_qty"));
    }

    do {
        alldone = true;
        int removerow = 0;

        for(int i = ui->tbl->rowCount() - 1; i > -1; i--) {
            db[":f_base"] = ui->tbl->getInteger(i, 0);
            db.exec("select g.f_name, gc.f_goods, gc.f_qty / g.f_complectout as f_qty "
                    "from c_goods_complectation gc "
                    "left join c_goods g on g.f_id=gc.f_base "
                    "where g.f_id=:f_base ");

            while(db.nextRow()) {
                removerow = i;
                alldone = false;
                int r = ui->tbl->addEmptyRow();
                ui->tbl->setInteger(r, 0, db.getInt("f_goods"));
                ui->tbl->setString(r, 1, db.getString("f_name"));
                ui->tbl->setDouble(r, 2, db.getDouble("f_qty") *ui->tbl->item(i, 2)->data(Qt::UserRole).toDouble());
                ui->tbl->item(r, 2)->setData(Qt::UserRole, db.getDouble("f_qty") *ui->tbl->item(i, 2)->data(Qt::UserRole).toDouble());
            }

            if(removerow > 0) {
                ui->tbl->removeRow(removerow);
                removerow = 0;
            }
        }
    } while(!alldone);
}

void DlgSemireadyInOut::on_btnCancel_clicked()
{
    reject();
}

void DlgSemireadyInOut::on_leQty_textEdited(const QString &arg1)
{
    for(int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->setDouble(i, 2, (ui->tbl->item(i, 2)->data(Qt::UserRole).toDouble()
                                  / ui->leQty->property("qty").toDouble())
                           * str_float(arg1));
    }
}

void DlgSemireadyInOut::on_btnOk_clicked()
{
    accept();
}
