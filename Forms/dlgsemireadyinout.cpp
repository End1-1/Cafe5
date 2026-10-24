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

QString DlgSemireadyInOut::dishName()
{
    return ui->leGoods->text();
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
    const int baseId = vals.at(1).toInt();
    ui->leGoods->setProperty("f_base", baseId);

    C5Database db;
    db[":f_base"] = baseId;
    db.exec("select g.f_name, gc.f_goods, gc.f_qty "
            "from c_goods_complectation gc "
            "left join c_goods g on g.f_id=gc.f_goods "
            "where gc.f_base=:f_base ");

    const QVariantList rootPath{baseId};

    while(db.nextRow()) {
        int r = ui->tbl->addEmptyRow();
        ui->tbl->setInteger(r, 0, db.getInt("f_goods"));
        ui->tbl->setString(r, 1, db.getString("f_name"));
        ui->tbl->setDouble(r, 2, db.getDouble("f_qty"));
        if(QTableWidgetItem *it = ui->tbl->item(r, 2)) {
            it->setData(Qt::UserRole, db.getDouble("f_qty"));
            it->setData(Qt::UserRole + 1, rootPath);
        }
    }

    // Nested expand; path in UserRole+1 breaks cycles (A→B→A) without blocking diamond reuse.
    bool alldone = false;
    int guard = 0;

    while(!alldone && guard++ < 100) {
        alldone = true;

        for(int i = ui->tbl->rowCount() - 1; i > -1; i--) {
            const int goodsId = ui->tbl->getInteger(i, 0);
            QTableWidgetItem *qtyItem = ui->tbl->item(i, 2);
            if(!qtyItem) {
                continue;
            }
            QVariantList path = qtyItem->data(Qt::UserRole + 1).toList();

            if(path.contains(goodsId)) {
                continue;
            }

            db[":f_base"] = goodsId;
            db.exec("select g.f_name, gc.f_goods, "
                    "gc.f_qty / nullif(g.f_complectout, 0) as f_qty "
                    "from c_goods_complectation gc "
                    "left join c_goods g on g.f_id=gc.f_base "
                    "where gc.f_base=:f_base ");

            if(!db.nextRow()) {
                continue;
            }

            const double parentQty = qtyItem->data(Qt::UserRole).toDouble();
            QVariantList childPath = path;
            childPath.append(goodsId);
            int added = 0;

            do {
                const int childId = db.getInt("f_goods");

                if(childPath.contains(childId)) {
                    continue;
                }

                const double qty = db.getDouble("f_qty") * parentQty;
                int r = ui->tbl->addEmptyRow();
                ui->tbl->setInteger(r, 0, childId);
                ui->tbl->setString(r, 1, db.getString("f_name"));
                ui->tbl->setDouble(r, 2, qty);
                if(QTableWidgetItem *it = ui->tbl->item(r, 2)) {
                    it->setData(Qt::UserRole, qty);
                    it->setData(Qt::UserRole + 1, childPath);
                }
                ++added;
            } while(db.nextRow());

            if(added > 0) {
                alldone = false;
                ui->tbl->removeRow(i);
            } else {
                // Only cyclic refs — leave row as leaf, mark so we do not retry.
                qtyItem->setData(Qt::UserRole + 1, childPath);
            }
        }
    }
}

void DlgSemireadyInOut::on_btnCancel_clicked()
{
    reject();
}

void DlgSemireadyInOut::on_leQty_textEdited(const QString &arg1)
{
    const double baseQty = ui->leQty->property("qty").toDouble();
    if(baseQty < 0.0000001) {
        return;
    }

    for(int i = 0; i < ui->tbl->rowCount(); i++) {
        QTableWidgetItem *it = ui->tbl->item(i, 2);
        if(!it) {
            continue;
        }
        ui->tbl->setDouble(i, 2, (it->data(Qt::UserRole).toDouble() / baseQty) * str_float(arg1));
    }
}

void DlgSemireadyInOut::on_btnOk_clicked()
{
    accept();
}
