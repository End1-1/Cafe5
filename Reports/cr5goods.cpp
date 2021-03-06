#include "cr5goods.h"
#include "ce5goods.h"
#include "c5goodspricing.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "cr5goodsfilter.h"
#include "c5changepriceofgroup.h"
#include "c5goodspricing.h"
#include <math.h>
#include <QFile>

CR5Goods::CR5Goods(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Goods");

    fSimpleQuery = false;

    fMainTable = "c_goods gg";
    fLeftJoinTables << "left join c_groups g on g.f_id=gg.f_group [g]"
                    << "left join c_partners cp on cp.f_id=gg.f_supplier [cp]"
                    << "left join c_goods_classes gca on gca.f_id=gg.f_group1 [gca]"
                    << "left join c_goods_classes gcb on gcb.f_id=gg.f_group2 [gcb]"
                    << "left join c_goods_classes gcc on gcc.f_id=gg.f_group3 [gcc]"
                    << "left join c_goods_classes gcd on gcd.f_id=gg.f_group4 [gcd]"
                    << "left join c_units u on u.f_id=gg.f_unit [u]"
                       ;

    fColumnsFields << "gg.f_id"
                   << "cp.f_taxname"
                   << "g.f_name as f_groupname"
                   << "u.f_name as f_unitname"
                   << "gg.f_name"
                   << "gg.f_scancode"
                   << "gg.f_saleprice"
                   << "gg.f_saleprice2"
                   << "gg.f_lowlevel"
                   << "gg.f_lastinputprice"
                   << "g.f_chargevalue"
                   << "(gg.f_saleprice/gg.f_lastinputprice*100)-100 as f_realchargevalue"
                   << "gca.f_name as gname1"
                   << "gcb.f_name as gname2"
                   << "gcc.f_name as gname3"
                   << "gcd.f_name as gname4"
                   << "gg.f_description"
                      ;

    fColumnsVisible["gg.f_id"] = true;
    fColumnsVisible["cp.f_taxname"] = true;
    fColumnsVisible["g.f_name as f_groupname"] = true;
    fColumnsVisible["u.f_name as f_unitname"] = true;
    fColumnsVisible["gg.f_name"] = true;
    fColumnsVisible["gg.f_saleprice"] = true;
    fColumnsVisible["gg.f_saleprice2"] = true;
    fColumnsVisible["gg.f_lowlevel"] = true;
    fColumnsVisible["gg.f_lastinputprice"] = true;
    fColumnsVisible["g.f_chargevalue"] = false;
    fColumnsVisible["(gg.f_saleprice/gg.f_lastinputprice*100)-100 as f_realchargevalue"] = false;
    fColumnsVisible["gca.f_name as gname1"] = true;
    fColumnsVisible["gcb.f_name as gname2"] = true;
    fColumnsVisible["gcc.f_name as gname3"] = true;
    fColumnsVisible["gcd.f_name as gname4"] = true;
    fColumnsVisible["gg.f_scancode"] = true;
    fColumnsVisible["gg.f_description"] = false;

    fTranslation["f_id"] = tr("Code");
    fTranslation["f_taxname"] = tr("Supplier");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_unitname"] = tr("Unit");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_saleprice"] = tr("Retail price");
    fTranslation["f_saleprice2"] = tr("Wholesale price");
    fTranslation["f_lowlevel"] = tr("Low level");
    fTranslation["f_lastinputprice"] = tr("Last input price");
    fTranslation["f_chargevalue"] = tr("Charge value");
    fTranslation["f_realchargevalue"] = tr("Real charge value");
    fTranslation["gname1"] = tr("Class 1");
    fTranslation["gname2"] = tr("Class 2");
    fTranslation["gname3"] = tr("Class 3");
    fTranslation["gname4"] = tr("Class 4");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_description"] = tr("Description");

    restoreColumnsVisibility();
    fEditor = new CE5Goods(dbParams);
    fFilterWidget = new CR5GoodsFilter(dbParams);
}

QToolBar *CR5Goods::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
            if (pr(fDBParams.at(1), cp_t1_goods_pricing)) {
                fToolBar->addAction(QIcon(":/pricing.png"), tr("Pricing"), this, SLOT(pricing()));
            }
            fToolBar->addAction(QIcon(":/dress.png"), tr("Group price"), this, SLOT(groupPrice()));
            fToolBar->addAction(QIcon(":/scales.png"), tr("Scales"), this, SLOT(exportToScales()));
    }
    return fToolBar;
}

bool CR5Goods::on_tblView_doubleClicked(const QModelIndex &index)
{
    if (!fColumnsVisible["gg.f_id"]) {
        C5Message::info(tr("ID Column must be included in report"));
        return false;
    }
    return C5ReportWidget::on_tblView_doubleClicked(index);
}

void CR5Goods::pricing()
{
    if (C5Message::question(tr("Warning! This operation will applied to all goods in the current report! Continue?")) != QDialog::Accepted) {
        return;
    }
    C5GoodsPricing *gp = new C5GoodsPricing(fDBParams);
    if (gp->exec() == QDialog::Accepted) {
        C5Database db(fDBParams);
        int colId = fModel->indexForColumnName("f_id");
        if (colId < 0) {
            C5Message::error(tr("You must select Code field in the report"));
            return;
        }
        int colSelfcost = fModel->indexForColumnName("f_lastinputprice");
        if (colSelfcost < 0) {
            C5Message::error(tr("You must select Last Input field in the report"));
            return;
        }
        int chargeValueCol = fModel->indexForColumnName("f_chargevalue");
        if (chargeValueCol < 0) {
            C5Message::error(tr("You must select Charge value field in the report"));
            return;
        }
        int colNewPrice = fModel->indexForColumnName("f_saleprice");
        for (int i = 0; i < fModel->rowCount(); i++) {
            double newPrice = fModel->data(i, colSelfcost, Qt::EditRole).toDouble();
            double chargeValue = fModel->data(i, chargeValueCol, Qt::EditRole).toDouble() / 100;
            if (newPrice < 0.0001) {
                continue;
            }
            if (chargeValue < 0.001) {
                continue;
            }
            newPrice += (newPrice * chargeValue);
            newPrice = ceil(trunc(newPrice));
            db[":f_saleprice"] = newPrice;
            db.update("c_goods", where_id(fModel->data(i, colId, Qt::EditRole).toInt()));
            if (colNewPrice > -1) {
                fModel->setData(i, colNewPrice, newPrice);
            }
        }
    }
    delete gp;
}

void CR5Goods::groupPrice()
{
    double price1, price2;
    if (C5ChangePriceOfGroup::groupPrice(fDBParams, price1, price2)) {
        QString p1, p2;
        if (price1 < 0.0001 && price2 < 0.0002) {
            return;
        }
        if (price1 > 0.0001) {
            p1 = " f_saleprice=" + QString::number(price1, 'f', 2);
        }
        if (price2 > 0.0001) {
            p2 = " f_saleprice2=" + QString::number(price2, 'f', 2);
        }
        QString codes;
        for (int i = 0; i < fModel->rowCount(); i++) {
            if (codes.length() > 0) {
                codes += ",";
            }
            codes += fModel->data(i, fModel->indexForColumnName("f_id"), Qt::EditRole).toString();
        }
        C5Database db(dbParams());
        QString query = "update c_goods set " + p1 + (p1.length() > 0 && p2.length() > 0? "," : "") + p2 + " where f_id in (" + codes + ")";
        db.exec(query);
        C5Message::tr("Done");
    }
}

void CR5Goods::exportToScales()
{
    C5Database db(fDBParams);
    QFile f("c:/scales/export.xml");
    f.open(QIODevice::WriteOnly);
    f.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
    f.write("<NewDataSet>\r\n");
    db.exec("select f_scancode, f_name, f_saleprice, f_wholenumber from c_goods where f_enabled=1 and length(f_scancode) between 1 and 5 ");
    while (db.nextRow()) {
        f.write("<Report>\r\n");
        f.write(QString("<CodeSort>%1</CodeSort>").arg(db.getString(0)).toUtf8());
        f.write(QString("<Code>%1</Code>").arg(db.getString(0)).toUtf8());
        f.write(QString("<GoodName>%1</GoodName>").arg(db.getString(1)).toUtf8());
        f.write(QString("<PriceOut2>%1</PriceOut2>").arg(db.getDouble(2)).toUtf8());
        if (db.getInt("f_wholenumber") > 0) {
            f.write(QString("<IsPiece>1</IsPiece>").toUtf8());
        }
        f.write("</Report>\r\n");
    }
    f.write("</NewDataSet>");
    f.close();
    C5Message::info(tr("Done"));
}
