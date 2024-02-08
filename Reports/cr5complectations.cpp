#include "cr5complectations.h"
#include "c5tablemodel.h"

CR5Complectations::CR5Complectations(const QStringList &dbparams, QWidget *parent) :
    C5ReportWidget(dbparams, parent)
{
    fLabel = tr("Complectations");
    fIcon = ":/goods.png";
}

QToolBar *CR5Complectations::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5Complectations::buildQuery()
{
    fTranslation["f_goodscode"] = tr("Goods code");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_goodsqty"] = tr("Qty");
    fTranslation["f_unit"] = tr("Unit");
    fTranslation["f_goodsprice"] = tr("Price");
    fTranslation["f_goodstotal"] = tr("Total");

    int col = 0;
    fModel->insertColumn(col++, tr("Goods code"));
    fModel->insertColumn(col++, tr("Goods name"));
    fModel->insertColumn(col++, tr("Qty"));
    fModel->insertColumn(col++, tr("Unit"));
    fModel->insertColumn(col++, tr("Price"));
    fModel->insertColumn(col++, tr("Total"));

    C5Database db(fDBParams);
    QList<QList<QVariant> > complectNames;
    db.exec("select distinct(g.f_name), c.f_base, g.f_complectout, u.f_name from "
            "c_goods_complectation c "
            "left join c_goods g on g.f_id=c.f_base "
            "left join c_units u on u.f_id=g.f_unit ", complectNames);
    QMap<int, int> complectIdRowMap;
    for (int i = 0; i < complectNames.count(); i++) {
        complectIdRowMap[complectNames.at(i).at(1).toInt()] = i;
    }
    db.exec("select c.f_base, c.f_goods, g.f_name, c.f_qty, u.f_name as f_unitname, g.f_lastinputprice "
            "from c_goods_complectation c "
            "inner join c_goods g on g.f_id=c.f_goods "
            "inner join c_units u on u.f_id=g.f_unit "
            "order by c.f_base ");
    int row = 0;
    int curbase = 0;
    int baserow = -1;
    double totalamount = 0;
    while (db.nextRow()) {
        if (curbase != db.getInt(0)) {
            if (baserow > -1) {
                fModel->setData(baserow, 5, totalamount);
                if (baserow > 0) {
                    fModel->insertRow(baserow - 1);
                    row++;
                }
            }
            fModel->insertRow(row);
            baserow = row;
            fModel->setData(row, 0, complectNames[complectIdRowMap[db.getInt(0)]].at(1));
            fModel->setData(row, 1, complectNames[complectIdRowMap[db.getInt(0)]].at(0));
            fModel->setData(row, 2, complectNames[complectIdRowMap[db.getInt(0)]].at(2));
            fModel->setData(row, 3, complectNames[complectIdRowMap[db.getInt(0)]].at(3));
            totalamount = 0;
            curbase = db.getInt(0);
            row++;
        }
        fModel->insertRow(row);
        for (int i = 1; i < db.columnCount(); i++) {
            fModel->setData(row, i - 1, db.getValue(i));
            fModel->setData(row, 5, fModel->data(row, 4, Qt::EditRole).toDouble() * fModel->data(row, 2, Qt::EditRole).toDouble());
            totalamount += fModel->data(row, 4, Qt::EditRole).toDouble() * fModel->data(row, 2, Qt::EditRole).toDouble();
        }
        row++;
    }
    if (baserow > -1) {
        fModel->setData(baserow, 5, totalamount);
        if (baserow > 0) {
            fModel->insertRow(baserow - 1);
            row++;
        }
    }
    for (int i = 0; i < fModel->rowCount(); i++) {
        if (!fModel->data(i, 0, Qt::EditRole).toString().isEmpty() && fModel->data(i, 4, Qt::EditRole).toString().isEmpty()) {
            fModel->setRowColor(i, QColor::fromRgb(200, 200, 200));
        }
    }
}
