#include "taxprint.h"
#include "ui_taxprint.h"
#include "working.h"
#include "c5lineedit.h"
#include "printtaxn.h"

TaxPrint::TaxPrint() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::TaxPrint)
{
    ui->setupUi(this);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 120, 200, 80, 80, 80, 0);
}

TaxPrint::~TaxPrint()
{
    delete ui;
}

void TaxPrint::on_btnPlus_clicked()
{
    addGoods(ui->leCode->text());
    ui->leCode->clear();
}

void TaxPrint::qtyChanged(const QString &arg1)
{
    int r, c;
    if (ui->tbl->findWidget(static_cast<QWidget*>(sender()), r, c)) {
        ui->tbl->lineEdit(r, 4)->setDouble(arg1.toDouble() * ui->tbl->lineEdit(r, 3)->getDouble());
        countTotal();
    }
}

void TaxPrint::priceChanged(const QString &arg1)
{
    int r, c;
    if (ui->tbl->findWidget(static_cast<QWidget*>(sender()), r, c)) {
        ui->tbl->lineEdit(r, 4)->setDouble(arg1.toDouble() * ui->tbl->lineEdit(r, 2)->getDouble());
        countTotal();
    }
}

void TaxPrint::totalChanged(const QString &arg1)
{
    int r, c;
    if (ui->tbl->findWidget(static_cast<QWidget*>(sender()), r, c)) {
        if (ui->tbl->lineEdit(r, 2)->getDouble() > 0.00001) {
            ui->tbl->lineEdit(r, 3)->setDouble(arg1.toDouble() / ui->tbl->lineEdit(r, 2)->getDouble());
            countTotal();
        }
    }
}

void TaxPrint::addGoods(const QString &code)
{
    if (!Working::fGoods.contains(code)) {
        C5Message::error(tr("Could not find goods with this code"));
        return;
    }
    Goods g = Working::fGoods[code];
    int row = ui->tbl->addEmptyRow();
    ui->tbl->setString(row, 0, g.fCode);
    ui->tbl->setString(row, 1, g.fName);
    C5LineEdit *le = ui->tbl->createLineEdit(row, 2);
    le->setText("1");
    connect(le, SIGNAL(textEdited(QString)), this, SLOT(qtyChanged(QString)));
    le = ui->tbl->createLineEdit(row, 3);
    le->setDouble(g.fRetailPrice);
    connect(le, SIGNAL(textEdited(QString)), this, SLOT(priceChanged(QString)));
    le = ui->tbl->createLineEdit(row, 4);
    le->setDouble(g.fRetailPrice);
    connect(le, SIGNAL(textEdited(QString)), this, SLOT(totalChanged(QString)));
    ui->tbl->setString(row, 5, g.fAdgCode);
    countTotal();
}

void TaxPrint::countTotal()
{
    double t = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        t += ui->tbl->lineEdit(i, 4)->getDouble();
    }
    ui->leTotal->setDouble(t);
}

void TaxPrint::on_leCode_returnPressed()
{
    addGoods(ui->leCode->text());
    ui->leCode->clear();
}

void TaxPrint::on_btnPrint_clicked()
{
    PrintTaxN pt(C5Config::taxIP(), C5Config::taxPort(), C5Config::taxPassword(), C5Config::taxUseExtPos(), C5Config::taxCashier(), C5Config::taxPin(), this);
    pt.fPartnerTin = ui->leTaxpayerPin->text();
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        Goods gg = Working::fGoods[Working::fGoodsCodeForPrint[ui->tbl->getInteger(i, 0)]];
        pt.addGoods(QString::number(gg.fTaxDept), //dep
                    gg.fAdgCode, //adg
                    gg.fCode, //goods id
                    gg.fName, //name
                    ui->tbl->lineEdit(i, 2)->getDouble(), //price
                    ui->tbl->lineEdit(i, 3)->getDouble(), //qty
                    0); //discount
    }
    QString jsonIn, jsonOut, err;
    int result = 0;
    result = pt.makeJsonAndPrint(0, 0, jsonIn, jsonOut, err);
    if (result == pt_err_ok) {
        C5Message::info(tr("Printed"));
    } else {
        C5Message::error(err +"<br>" + jsonOut);
    }
}

void TaxPrint::on_btnClear_clicked()
{
    if (C5Message::info(tr("Confirm to clear"))) {
        ui->tbl->setRowCount(0);
    }
}
