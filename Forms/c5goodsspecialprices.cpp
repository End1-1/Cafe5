#include "c5goodsspecialprices.h"
#include "ui_c5goodsspecialprices.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5lineedit.h"
#include <QTableWidgetItem>

C5GoodsSpecialPrices::C5GoodsSpecialPrices(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5GoodsSpecialPrices),
    fPartner(0)
{
    ui->setupUi(this);
    C5Database db(fDBParams);
    ui->tbl->setColumnWidths(7, 0, 200, 200, 100, 100, 100);
    ui->tblp->setColumnWidths(4, 0, 100, 200, 300);
    ui->tblp->setMinimumWidth(620);
    db.exec("select f_id, f_taxcode, f_name, f_address from c_partners order by 3, 4");
    ui->tblp->setRowCount(db.rowCount());
    int r = 0;
    while (db.nextRow()) {
        for (int i = 0; i < db.columnCount(); i++) {
            ui->tblp->setItem(r, i, new C5TableWidgetItem(db.getValue(i).toString()));
        }
        r++;
    }
}

C5GoodsSpecialPrices::~C5GoodsSpecialPrices()
{
    delete ui;
}

QToolBar *C5GoodsSpecialPrices::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbSave;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

void C5GoodsSpecialPrices::calcPercent1(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit *>(sender());
    int r, c;
    if (!ui->tbl->findWidget(l, r, c)) {
        return;
    }
    ui->tbl->setDouble(r, 6, ((l->getDouble() * 100) / ui->tbl->getDouble(r, 3) - 100));
}

void C5GoodsSpecialPrices::calcPercent2(const QString &arg1)
{
    Q_UNUSED(arg1);
    C5LineEdit *l = static_cast<C5LineEdit *>(sender());
    int r, c;
    if (!ui->tbl->findWidget(l, r, c)) {
        return;
    }
    ui->tbl->setDouble(r, 7, ((l->getDouble() * 100) / ui->tbl->getDouble(r, 4) - 100));
}

void C5GoodsSpecialPrices::saveDataChanges()
{
    if (fPartner == 0) {
        return;
    }
    C5Database db(fDBParams);
    db.startTransaction();
    db[":f_partner"] = fPartner;
    db.exec("delete from c_goods_special_prices where f_partner=:f_partner");
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->lineEdit(i, 5)->getDouble() > 0.01) {
            db[":f_partner"] = fPartner;
            db[":f_goods"] = ui->tbl->getInteger(i, 0);
            db[":f_price"] = ui->tbl->lineEdit(i, 5)->getDouble();
            db.insert("c_goods_special_prices", false);
        }
    }
    db.commit();
    C5Message::info(tr("Saved"));
}

void C5GoodsSpecialPrices::getPrices()
{
    if (fPartner == 0) {
        return;
    }
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    C5Database db(fDBParams);
    db[":f_partner"] = fPartner;
    db.exec("select g.f_id, gr.f_name as f_groupname, g.f_name as f_goodsname, gp.f_price1, gp.f_price2, sp.f_price "
            "from c_goods g "
            "left join c_groups gr on gr.f_id=g.f_group "
            "left join c_goods_prices gp on gp.f_goods=g.f_id "
            "left join c_goods_special_prices sp on sp.f_goods=g.f_id and sp.f_partner=:f_partner "
            "where gp.f_currency=1 order by 2, 3");
    ui->tbl->setRowCount(db.rowCount());
    int r = 0;
    while (db.nextRow()) {
        ui->tbl->setInteger(r, 0, db.getInt("f_id"));
        ui->tbl->setString(r, 1, db.getString("f_groupname"));
        ui->tbl->setString(r, 2, db.getString("f_goodsname"));
        ui->tbl->setDouble(r, 3, db.getDouble("f_price1"));
        ui->tbl->setDouble(r, 4, db.getDouble("f_price2"));
        C5LineEdit *l = ui->tbl->createLineEdit(r, 5);
        l->setValidator(new QDoubleValidator(0, 99999999999, 2));
        connect(l, &C5LineEdit::textChanged, this, &C5GoodsSpecialPrices::calcPercent1);
        l->setDouble(db.getDouble("f_price"));
        r++;
    }
    filterGoods();
}

void C5GoodsSpecialPrices::filterGoods()
{
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        bool h = false;
        if (!ui->leGoodsFilter->isEmpty()) {
            if (!ui->tbl->getString(i, 2).contains(ui->leGoodsFilter->text(), Qt::CaseInsensitive)) {
                h = true;
            }
        }
        if (!ui->leGroupFilter->isEmpty() && !h) {
            if (!ui->tbl->getString(i, 2).contains(ui->leGroupFilter->text(), Qt::CaseInsensitive)) {
                h = true;
            }
        }
        ui->tbl->setRowHidden(i, h);
    }
}

void C5GoodsSpecialPrices::on_tblp_itemActivated(QTableWidgetItem *item)
{
}

void C5GoodsSpecialPrices::on_tblp_itemClicked(QTableWidgetItem *item)
{
    saveDataChanges();
    fPartner = ui->tblp->item(item->row(), 0)->data(Qt::EditRole).toInt();
    getPrices();
}

void C5GoodsSpecialPrices::on_lePartnerFilter_textChanged(const QString &arg1)
{
    for (int i = 0; i < ui->tblp->rowCount(); i++) {
        bool h = true;
        for (int c = 1; c < ui->tblp->columnCount(); c++) {
            if (ui->tblp->getString(i, c).contains(arg1, Qt::CaseInsensitive)) {
                h = false;
                break;
            }
        }
        ui->tblp->setRowHidden(i, h);
    }
}

void C5GoodsSpecialPrices::on_leGoodsFilter_textChanged(const QString &arg1)
{
    filterGoods();
}

void C5GoodsSpecialPrices::on_leGroupFilter_textChanged(const QString &arg1)
{
    filterGoods();
}
