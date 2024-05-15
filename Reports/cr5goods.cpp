#include "cr5goods.h"
#include "ce5goods.h"
#include "c5goodspricing.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5user.h"
#include "cr5goodsfilter.h"
#include "c5changepriceofgroup.h"
#include "c5goodspricing.h"
#include "goodsasmap.h"
#include "c5storebarcode.h"
#include <math.h>
#include <QFile>

QMap <QString, QString> l;

CR5Goods::CR5Goods(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/goods.png";
    fLabel = tr("Goods");

    l["ա"]="a";
    l["բ"]="b";
    l["գ"]="g";
    l["դ"]="d";
    l["ե"]="e";
    l["զ"]="z";
    l["է"]="e";
    l["ը"]="@";
    l["թ"]="t";
    l["ժ"]="zh";
    l["ի"]="i";
    l["լ"]="l";
    l["խ"]="kh";
    l["ծ"]="ts";
    l["կ"]="k";
    l["հ"]="h";
    l["ձ"]="dz";
    l["ղ"]="h";
    l["ճ"]="tch";
    l["մ"]="m";
    l["յ"]="y";
    l["ն"]="n";
    l["շ"]="sh";
    l["ո"]="o";
    l["չ"]="ch";
    l["պ"]="p";
    l["ջ"]="j";
    l["ռ"]="r";
    l["ս"]="s";
    l["վ"]="v";
    l["տ"]="t";
    l["ր"]="r";
    l["ց"]="tc";
    l["ւ"]="u";
    l["փ"]="p";
    l["ք"]="q";
    l["օ"]="o";
    l["ֆ"]="f";

    fSimpleQuery = false;

    fMainTable = "c_goods gg";
    fLeftJoinTables << "left join c_groups g on g.f_id=gg.f_group [g]"
                    << "left join c_partners cp on cp.f_id=gg.f_supplier [cp]"
                    << "left join c_goods_classes gca on gca.f_id=gg.f_group1 [gca]"
                    << "left join c_goods_classes gcb on gcb.f_id=gg.f_group2 [gcb]"
                    << "left join c_goods_classes gcc on gcc.f_id=gg.f_group3 [gcc]"
                    << "left join c_goods_classes gcd on gcd.f_id=gg.f_group4 [gcd]"
                    << "left join c_goods_prices gpr on gpr.f_goods=gg.f_id [gpr]"
                    << "left join c_units u on u.f_id=gg.f_unit [u]"
                    << "left join e_currency cr on cr.f_id=gpr.f_currency [cr]"
                    << "left join c_goods_model cm on cm.f_id=gg.f_model [cm]"
                       ;

    fColumnsFields << "gg.f_id"
                   << "cp.f_taxname"
                   << "g.f_name as f_groupname"
                   << "cm.f_name as f_modelname"
                   << "u.f_name as f_unitname"
                   << "gg.f_name"
                   << "gg.f_scancode"
                   << "gpr.f_price1"
                   << "gpr.f_price2"
                << "gpr.f_price1disc"
                << "gpr.f_price2disc"
                   << "cr.f_symbol as f_currencyname"
                   << "gg.f_lowlevel"
                   << "gg.f_lastinputprice"
                   << "g.f_chargevalue"
                   << "(ggpr.f_price1/gg.f_lastinputprice*100)-100 as f_realchargevalue"
                   << "gg.f_acc"
                   << "gca.f_name as gname1"
                   << "gcb.f_name as gname2"
                   << "gcc.f_name as gname3"
                   << "gcd.f_name as gname4"
                   << "gg.f_description"
                   << "gg.f_weight"
                   << "gg.f_fiscalname"
                      ;

    fColumnsVisible["gg.f_id"] = true;
    fColumnsVisible["cp.f_taxname"] = true;
    fColumnsVisible["g.f_name as f_groupname"] = true;
    fColumnsVisible["cm.f_name as f_modelname"] = true;
    fColumnsVisible["u.f_name as f_unitname"] = true;
    fColumnsVisible["gg.f_name"] = true;
    fColumnsVisible["gpr.f_price1"] = true;
    fColumnsVisible["gpr.f_price2"] = true;
    fColumnsVisible["gpr.f_price1disc"] = false;
    fColumnsVisible["gpr.f_price2disc"] = false;
    fColumnsVisible["cr.f_symbol as f_currencyname"] = true;
    fColumnsVisible["gg.f_lowlevel"] = true;
    fColumnsVisible["gg.f_lastinputprice"] = true;
    fColumnsVisible["g.f_chargevalue"] = false;
    fColumnsVisible["gg.f_acc"] = true;
    fColumnsVisible["(gpr.f_price1/gg.f_lastinputprice*100)-100 as f_realchargevalue"] = false;
    fColumnsVisible["gca.f_name as gname1"] = true;
    fColumnsVisible["gcb.f_name as gname2"] = true;
    fColumnsVisible["gcc.f_name as gname3"] = true;
    fColumnsVisible["gcd.f_name as gname4"] = true;
    fColumnsVisible["gg.f_scancode"] = true;
    fColumnsVisible["gg.f_description"] = false;
    fColumnsVisible["gg.f_weight"] = true;
    fColumnsVisible["gg.f_fiscalname"] = true;

    fTranslation["f_id"] = tr("Code");
    fTranslation["f_taxname"] = tr("Supplier");
    fTranslation["f_groupname"] = tr("Group");
    fTranslation["f_modelname"] = tr("Model");
    fTranslation["f_unitname"] = tr("Unit");
    fTranslation["f_name"] = tr("Name");
    fTranslation["f_price1"] = tr("Retail price");
    fTranslation["f_price1disc"] = tr("Retail discounted");
    fTranslation["f_price2"] = tr("Wholesale price");
    fTranslation["f_price2disc"] = tr("Whosale discounted");
    fTranslation["f_currencyname"] = tr("Currency");
    fTranslation["f_lowlevel"] = tr("Low level");
    fTranslation["f_lastinputprice"] = tr("Last input price");
    fTranslation["f_chargevalue"] = tr("Charge value");
    fTranslation["f_acc"] = tr("Account");
    fTranslation["f_realchargevalue"] = tr("Real charge value");
    fTranslation["gname1"] = tr("Class 1");
    fTranslation["gname2"] = tr("Class 2");
    fTranslation["gname3"] = tr("Class 3");
    fTranslation["gname4"] = tr("Class 4");
    fTranslation["f_scancode"] = tr("Scancode");
    fTranslation["f_description"] = tr("Description");
    fTranslation["f_weight"] = tr("Weight");
    fTranslation["f_fiscalname"] = tr("Fiscal name");

    restoreColumnsVisibility();
    fEditor = new CE5Goods(dbParams);
    fFilterWidget = new CR5GoodsFilter(dbParams);
    fFilter = static_cast<CR5GoodsFilter*>(fFilterWidget);
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
            if (__user->check(cp_t1_goods_pricing)) {
                fToolBar->addAction(QIcon(":/pricing.png"), tr("Pricing"), this, SLOT(pricing()));
            }
            fToolBar->addAction(QIcon(":/dress.png"), tr("Group price"), this, SLOT(groupPrice()));
            fToolBar->addAction(QIcon(":/scales.png"), tr("Scales"), this, SLOT(exportToScales()));
            fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(deleteGoods()));
            fToolBar->addAction(QIcon(":/barcode.png"), tr("Print\nbarcode"), this, SLOT(printBarCodes()));
            fToolBar->addAction(QIcon(":/AS.png"), tr("ArmSoft map"), this, SLOT(armSoftMap()));
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
    double price1, price2, price1disc, price2disc;
    if (C5ChangePriceOfGroup::groupPrice(fDBParams, price1, price2, price1disc , price2disc)) {
        QString p1, p2, p1d, p2d;
//        if (price1 < 0.0001 && price2 < 0.0002 && price1disc < 0.0002 && price2disc < 0.0002) {
//            return;
//        }

        QString codes;
        for (int i = 0; i < fModel->rowCount(); i++) {
            if (codes.length() > 0) {
                codes += ",";
            }
            codes += fModel->data(i, fModel->indexForColumnName("f_id"), Qt::EditRole).toString();
        }
        C5Database db(dbParams());
        QString query ;

        if (price1 > -1) {
            p1 = " f_price1=" + QString::number(price1, 'f', 2);
        }
        if (price2 > -1) {
            p2 = " f_price2=" + QString::number(price2, 'f', 2);
        }
        if (price1disc > -1) {
            p1d = " f_price1disc =" + QString::number(price1disc, 'f', 2);
        }
        if (price2disc > -1) {
            p2d = " f_price2disc=" + QString::number(price2disc, 'f', 2);
        }
        QStringList l;
        if (!p1.isEmpty()) {
            l.append(p1);
        }
        if (!p2.isEmpty()) {
            l.append(p2);
        }
        if (!p1d.isEmpty()) {
            l.append(p1d);
        }
        if (!p2d.isEmpty()) {
            l.append(p2d);
        }
        query = "update c_goods_prices set ";
        bool f = true;
        for (int i = 0; i < l.count(); i++) {
            if (f) {
                f = false;
            } else {
                query += ",";
            }
            query += l.at(i);
        }
        query  += " where f_currency=1 and f_goods in (" + codes + ")";
        db.exec(query);
        C5Message::tr("Done");
    }
}

void CR5Goods::exportToScales()
{
    C5Database db(fDBParams);
    QString sql ="select f_scancode, f_name, f_saleprice, f_wholenumber from c_goods where f_enabled=1 and length(f_scancode) between 1 and 5 ";
    if (static_cast<CR5GoodsFilter*>(fFilterWidget)->group().isEmpty() == false) {
        sql += " and f_group=" + static_cast<CR5GoodsFilter*>(fFilterWidget)->group();
    }
    db.exec(sql);

    if (__c5config.getValue(param_frontdesk_scale_dir).isEmpty() == false) {
        QFile f(__c5config.getValue(param_frontdesk_scale_dir) + "/export.xml");
        f.open(QIODevice::WriteOnly);
        f.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n");
        f.write("<NewDataSet>\r\n");
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
    if (__c5config.getValue(param_fd_russian_scale1).isEmpty() == false) {
        C5Database dbo("QODBC3");
        dbo.setDatabase("", __c5config.getValue(param_fd_russian_scale1), "", "");
        if (dbo.open()) {
            while (db.nextRow()) {
                QString name = db.getString("f_name").toLower();
                name.replace("ու", "u");
                for (QMap<QString, QString>::const_iterator it = l.constBegin(); it != l.constEnd(); it++) {
                    name.replace(it.key(), it.value());
                }
                dbo[":PLU"] = db.getString("f_scancode").toInt();
                dbo.exec("delete from Products where PLU=:PLU");
                QString sql = QString("insert into Products (ID, PLU, Status, LabelNo, BarcodeNo, "
                                        "Prefix, Price, Tare, Code, DuringDate, "
                                        "[Group], Barcode, RVTName, "
                                        "Type, LiteDuringDate) "
                                        "values (%1, %2, %3, %4, %5, '%6', %7, %8, %9, %10, %11, %12, '%13', '%14', %15)")
                        .arg(db.getString("f_scancode"), db.getString("f_scancode"), QString("0"), QString("0"), QString("0"),
                             QString("0"), QString::number(db.getDouble("f_saleprice") / 100, 'f', 2))
                        .arg(QString("0"), QString("0"), QString("0"),
                             QString("0"), db.getString("f_scancode"), name,
                             QString::fromUtf8("вес."), QString("0"));
                dbo.exec(sql);
            }
            C5Message::info(tr("Done"));
        } else {
            C5Message::error(dbo.fLastError);
        }
    }
}

void CR5Goods::deleteGoods()
{
    int row = 0;
    int id = rowId(row, 0);
    if (id == 0) {
        return;
    }
    if (C5Message::question(QString("%1<br>%2")
                            .arg(tr("Confirm to remove"),
                                 fModel->data(row, fModel->indexForColumnName("f_name"), Qt::EditRole).toString())) != QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_goods"] = id;
    db.exec("select * from d_recipes where f_goods=:f_goods");
    QString err;
    if (db.nextRow()) {
        err.append("<br>" + tr("Used in recipes"));
    }
    db[":f_goods"] = id;
    db.exec("select * from c_goods_complectation where f_goods=:f_goods or f_base=:f_goods");
    if (db.nextRow()) {
        err.append("<br>" + tr("Used in complectation"));
    }
    db[":f_goods"] = id;
    db.exec("select * from a_store_draft where f_goods=:f_goods");
    if (db.nextRow()) {
        err.append("<br>" + tr("Used in store documents"));
    }
    db[":f_goods"] = id;
    db.exec("select * from o_goods where f_goods=:f_goods");
    if (db.nextRow()) {
        err.append("<br>" + tr("Used in sales (shop)"));
    }
    if (!err.isEmpty()) {
        C5Message::error(tr("Cannot remove ") + err);
        return;
    }
    db[":f_goods"] = id;
    db.exec("delete from c_goods_prices where f_goods=:f_goods");
    db[":f_id"] = id;
    if (db.exec("delete from c_goods where f_id=:f_id")) {
        removeRow(row);
        //refreshData();
    } else {
        C5Message::error(db.fLastError);
    }
}

void CR5Goods::printBarCodes()
{
    C5Database db(fDBParams);
    db[":f_id"] = fFilter->currency();
    db.exec("select f_symbol from e_currency where f_id=:f_id");
    db.nextRow();
    QString s = db.getString("f_symbol");
    C5StoreBarcode *b = __mainWindow->createTab<C5StoreBarcode>(fDBParams);
    b->fCurrencyName = s;
    for (int i = 0; i < fModel->rowCount(); i++) {
        b->addRow(fModel->data(i, fModel->indexForColumnName("f_goods"), Qt::EditRole).toString(),
                  fModel->data(i, fModel->indexForColumnName("f_scancode"), Qt::EditRole).toString(),
                  1, fFilter->currency(), "");
    }
}

void CR5Goods::armSoftMap()
{
    __mainWindow->createTab<GoodsAsMap>(fDBParams);
}
