#include "ce5partner.h"
#include "ui_ce5partner.h"
#include "c5database.h"
#include <QCompleter>
#include <stdexcept>

CE5Partner::CE5Partner(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5Partner)
{
    ui->setupUi(this);

    QCompleter *c = new QCompleter();
    c->setCaseSensitivity(Qt::CaseInsensitive);
    ui->cbGroup->setCompleter(c);
    ui->cbState->setCompleter(c);
    ui->cbCategory->setCompleter(c);
    ui->cbGroup->setDBValues("select f_id, f_name from c_partners_group");
    ui->cbState->setDBValues("select f_id, f_name from c_partners_state");
    ui->cbCategory->setDBValues("select f_id, f_name from c_partners_category");
    ui->cbSaleType->setDBValues("select f_id, f_name from o_sale_type where f_id in (1,2)");
    ui->cbManager->setDBValues("select f_id, concat_ws(' ', f_last, f_first) from s_user order by 2");
    fNew = true;
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
    fNew = false;
}

bool CE5Partner::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    C5Database db;
    if (fNew && ui->leCode->getInteger() > 0) {
        db[":f_id"] = ui->leCode->getInteger();
        db.insert("c_partners", false);
    }
    if (!CE5Editor::save(err, data)) {
        return false;
    }
    return true;
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

bool CE5Partner::canCopy()
{
    return true;
}

void CE5Partner::copyObject()
{
    ui->leCode->clear();
}

QString CE5Partner::savePathV2() const
{
    return "partners";
}

void CE5Partner::on_btnClearManager_clicked()
{
    if (ui->leCode->getInteger() == 0) {
        return;
    }
    C5Database db;
    db[":f_id"] = ui->leCode->getInteger();
    db[":f_manager"] = 0;
    db.exec("update c_partners set f_manager=:f_manager where f_id=:f_id");
    ui->cbManager->setCurrentIndex(-1);
}
