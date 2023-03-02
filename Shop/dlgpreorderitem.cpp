#include "dlgpreorderitem.h"
#include "ui_dlgpreorderitem.h"
#include "dlgdataonline.h"
#include <QTableWidget>

DlgPreorderItem::DlgPreorderItem(QWidget *parent) :
    C5Dialog(__c5config.replicaDbParams()),
    ui(new Ui::DlgPreorderItem)
{
    ui->setupUi(this);
}

DlgPreorderItem::~DlgPreorderItem()
{
    delete ui;
}

void DlgPreorderItem::configToTable(QTableWidget *t, int r)
{
    t->setItem(r, 0, new QTableWidgetItem(ui->leUUID->text()));
    t->setItem(r, 1, new QTableWidgetItem(ui->leGoodsId->text()));
    t->setItem(r, 2, new QTableWidgetItem(ui->leScancode->text()));
    t->setItem(r, 3, new QTableWidgetItem(ui->leName->text()));
    t->setItem(r, 4, new QTableWidgetItem(ui->leQty->text()));
    t->setItem(r, 5, new QTableWidgetItem(ui->lePrice->text()));
    t->setItem(r, 6, new QTableWidgetItem(ui->leTotal->text()));
    t->resizeColumnsToContents();
}

void DlgPreorderItem::configFromTable(QTableWidget *t, int r)
{
    ui->leUUID->setText(t->item(r, 0)->text());
    ui->leGoodsId->setText(t->item(r, 1)->text());
    ui->leScancode->setText(t->item(r, 2)->text());
    ui->leName->setText(t->item(r, 3)->text());
    ui->leQty->setText(t->item(r, 4)->text());
    ui->lePrice->setText(t->item(r, 5)->text());
    ui->leTotal->setText(t->item(r, 6)->text());
}

void DlgPreorderItem::on_btnGetGoods_clicked()
{
    DlgDataOnline::DataResult r;
    if (DlgDataOnline::get(__c5config.replicaDbParams(), "c_goods", r)) {
        ui->leName->setText(r.value(0, tr("Name")).toString());
        ui->leScancode->setText(r.value(0, tr("Scancode")).toString());
        ui->leGoodsId->setText(r.value(0, tr("Code")).toString());
    }
}

void DlgPreorderItem::on_leQty_textEdited(const QString &arg1)
{
    ui->leTotal->setDouble(str_float(arg1) * ui->lePrice->getDouble());
}

void DlgPreorderItem::on_lePrice_textEdited(const QString &arg1)
{
    ui->leTotal->setDouble(str_float(arg1) * ui->leQty->getDouble());
}

void DlgPreorderItem::on_leTotal_textEdited(const QString &arg1)
{
    if (ui->leQty->getDouble() > 0) {
        ui->lePrice->setDouble(str_float(arg1) / ui->leQty->getDouble());
    }
}

void DlgPreorderItem::on_btnCancel_clicked()
{
    reject();
}

void DlgPreorderItem::on_btnSave_clicked()
{
    QString err;
    if (ui->leGoodsId->getInteger() == 0) {
        err += tr("Goods is not selected") + "<br>";
    }
    if (ui->leQty->getDouble()  < 0.001) {
        err += tr("Qty is not defined")  + "<br>";
    }
    if (ui->lePrice->getDouble() < 0.001) {
        err += tr("Price is not defined")  + "<br>";
    }
    if (err.isEmpty() == false) {
        C5Message::error(err);
        return;
    }
    accept();
}
