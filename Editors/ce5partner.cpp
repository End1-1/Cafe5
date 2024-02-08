#include "ce5partner.h"
#include "ui_ce5partner.h"
#include <QCompleter>
#include <stdexcept>

CE5Partner::CE5Partner(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5Partner)
{
    ui->setupUi(this);
    C5Database db(dbParams);
    db.exec("select f_id, f_name from as_list");
    while (db.nextRow()) {
        int r = ui->tblAs->rowCount();
        ui->tblAs->setRowCount(r + 1);
        ui->tblAs->setInteger(r, 0, db.getInt("f_id"));
        ui->tblAs->setString(r, 1, db.getString("f_name"));
        ui->tblAs->createLineEdit(r, 2);
    }
    QCompleter *c = new QCompleter();
    c->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cbGroup->setCompleter(c);
    ui->cbState->setCompleter(c);
    ui->cbCategory->setCompleter(c);
    ui->cbGroup->setDBValues(fDBParams, "select f_id, f_name from c_partners_group");
    ui->cbState->setDBValues(fDBParams, "select f_id, f_name from c_partners_state");
    ui->cbCategory->setDBValues(fDBParams, "select f_id, f_name from c_partners_category");
    ui->cbSaleType->setDBValues(fDBParams, "select f_id, f_name from o_sale_type where f_id in (1,2)");
    ui->cbManager->setDBValues(fDBParams, "select f_id, concat_ws(' ', f_last, f_first) from s_user order by 2");
}

CE5Partner::~CE5Partner()
{
    delete ui;
}

QString CE5Partner::title()
{
    return tr("Partner");
}

QString CE5Partner::table()
{
    return "c_partners";
}

void CE5Partner::setId(int id)
{
    CE5Editor::setId(id);
    C5Database db(fDBParams);
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("select f_asdbid, f_ascode from as_convert where f_table='c_partners' and f_tableid=:f_id");
    while (db.nextRow()) {
        int asrow = -1;
        for (int i = 0; i < ui->tblAs->rowCount(); i++) {
            if (ui->tblAs->getInteger(i, 0) == db.getInt("f_asdbid")) {
                asrow = i;
                break;
            }
        }
        if (asrow < 0) {
            throw std::runtime_error(QString("The database id (%1) not exists. Check database structure.").arg(asrow).toLocal8Bit().data());
        }
        ui->tblAs->lineEdit(asrow, 2)->setText(db.getString("f_ascode"));
    }
}

bool CE5Partner::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    if (!CE5Editor::save(err, data)) {
        return false;
    }
    C5Database db(fDBParams);
    db[":f_table"] = "c_partners";
    db[":f_tableid"] = ui->leCode->getInteger();
    db.exec("delete from as_convert where f_table=:f_table and f_tableid=:f_tableid");
    for (int i = 0; i < ui->tblAs->rowCount(); i++) {
        db[":f_asdbid"] = ui->tblAs->getInteger(i, 0);
        db[":f_table"] = "c_partners";
        db[":f_tableid"] = ui->leCode->getInteger();
        db[":f_ascode"] = ui->tblAs->lineEdit(i, 2)->text();
        db.insert("as_convert");
    }
    return true;
}

void CE5Partner::clear()
{
    CE5Editor::clear();
    for (int i = 0; i < ui->tblAs->rowCount(); i++) {
        ui->tblAs->lineEdit(i, 2)->clear();
    }
}

bool CE5Partner::checkData(QString &err)
{
    CE5Editor::checkData(err);
    if (ui->cbCategory->currentData().toInt() == 0) {
        err += tr("Select category") + "\r\n";
    }
    if (ui->cbGroup->currentData().toInt() == 0) {
        err += tr("Select group") + "\r\n";
    }
    if (ui->cbState->currentData().toInt() == 0) {
        err += tr("Select state") + "\r\n";
    }
    return err.isEmpty();
}

void CE5Partner::on_btnClearManager_clicked()
{
    if (ui->leCode->getInteger() == 0) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = ui->leCode->getInteger();
    db[":f_manager"] = 0;
    db.exec("update c_partners set f_manager=:f_manager where f_id=:f_id");
    ui->cbManager->setCurrentIndex(-1);
}
