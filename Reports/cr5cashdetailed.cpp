#include "cr5cashdetailed.h"
#include "cr5cashdetailedfilter.h"
#include "c5gridgilter.h"
#include "c5tablemodel.h"
#include "c5tablewidget.h"
#include "c5mainwindow.h"
#include "c5cashdoc.h"

CR5CashDetailed::CR5CashDetailed(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fIcon = ":/cash.png";
    fLabel = tr("Cash movement");
    fFilterWidget = new CR5CashDetailedFilter(dbParams);
    fFilter = static_cast<CR5CashDetailedFilter*>(fFilterWidget);
    fColumnNameIndex["f_header"] = 0;
    fColumnNameIndex["f_date"] = 1;
    fColumnNameIndex["f_remarks"] = 2;
    fColumnNameIndex["f_debit"] = 3;
    fColumnNameIndex["f_credit"] = 4;
    fColumnNameIndex["f_balance"] = 5;
    fTranslation["f_header"] = tr("UUID");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_remarks"] = tr("Remarks");
    fTranslation["f_debit"] = tr("Debit");
    fTranslation["f_credit"] = tr("Credit");
    fTranslation["f_balance"] = tr("Balance");
}

QToolBar *CR5CashDetailed::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbNew
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
    }
    return fToolBar;
}

bool CR5CashDetailed::on_tblView_doubleClicked(const QModelIndex &index)
{
    if (index.row() < 0 || index.column() < 0) {
        return false;
    }
    QList<QVariant> values = fModel->getRowValues(index.row());
    if (values.at(0).toString().isEmpty()) {
        return false;
    }
    auto *cd = __mainWindow->createTab<C5CashDoc>(fDBParams);
    if (!cd->openDoc(values.at(0).toString())) {
        __mainWindow->removeTab(cd);
    }
    return true;
}

void CR5CashDetailed::buildQuery()
{
    if (fFilter->cash() == 0) {
        if (!C5GridGilter::filter(fFilter, fWhereCondition, fColumnsVisible, fTranslation)) {
            return;
        }
        if (fFilter->cash() == 0) {
            C5Message::info(tr("Cash must be selected"));
            return;
        }
    }
    QList<QVariant> er;
    er << QVariant() << QVariant() << QVariant() << QVariant() << QVariant() << QVariant();
    QList<QList<QVariant> > &rows = fModel->fRawData;
    rows.clear();
    QString query = QString("select '' as f_header, '%1' as f_date, '%2' as f_remarks, c.f_sign, sum(c.f_amount*c.f_sign) as f_amount "
                            "from e_cash c "
                            "inner join a_header h on h.f_id=c.f_header and h.f_type=:f_type "
                            "where f_cash=:f_cash and h.f_date<'%1'")
            .arg(fFilter->date1().toString(FORMAT_DATE_TO_STR_MYSQL))
            .arg(tr("Brought forward"));
    C5Database db(fDBParams);
    db[":f_cash"] = fFilter->cash();
    db[":f_type"] = DOC_TYPE_CASH;
    db.exec(query);
    if (db.nextRow()) {
        int r;
        rows.append(er);
        r = rows.count() - 1;
        rows[r][0] = db.getString("f_header");
        rows[r][1] = db.getDate("f_date");
        rows[r][2] = db.getString("f_remarks");
        if (db.getDouble("f_amount") > 0.001) {
            rows[r][3] = db.getDouble("f_amount");
            rows[r][4] = 0;
        } else {
            rows[r][3] = 0;
            rows[r][4] = abs(db.getDouble("f_amount"));
        }
    }
    query = "select c.f_header, h.f_date, c.f_remarks, c.f_sign, c.f_amount "
                                "from e_cash c "
                                "inner join a_header h on h.f_id=c.f_header and h.f_type=:f_type "
                                "where f_cash=:f_cash and h.f_date between :f_date1 and :f_date2 "
                                "order by h.f_date, h.f_userid ";
    db[":f_cash"] = fFilter->cash();
    db[":f_date1"] = fFilter->date1();
    db[":f_date2"] = fFilter->date2();
    db[":f_type"] = DOC_TYPE_CASH;
    db.exec(query);
    while (db.nextRow()) {
        int r;
        rows.append(er);
        r = rows.count() - 1;
        rows[r][0] = db.getString("f_header");
        rows[r][1] = db.getDate("f_date");
        rows[r][2] = db.getString("f_remarks");
        if (db.getInt("f_sign") == 1) {
            rows[r][3] = db.getDouble("f_amount");
            rows[r][4] = 0;
        } else {
            rows[r][3] = 0;
            rows[r][4] = abs(db.getDouble("f_amount"));
        }
    }
    fModel->setExternalData(fColumnNameIndex, fTranslation);
    restoreColumnsWidths();
    sum();
}

void CR5CashDetailed::refreshData()
{
    buildQuery();
}

void CR5CashDetailed::restoreColumnsWidths()
{
    C5ReportWidget::restoreColumnsWidths();
    fTableView->setColumnWidth(0, 0);
}

int CR5CashDetailed::newRow()
{
    __mainWindow->createTab<C5CashDoc>(fDBParams);
    return 0;
}

void CR5CashDetailed::sum()
{
    double debit = 0;
    double credit = 0;
    double total = 0;
    for (int i = 0; i < fModel->rowCount(); i++) {
        debit += fModel->fRawData[i][3].toDouble();
        credit += fModel->fRawData[i][4].toDouble();
        total = total + fModel->fRawData[i][3].toDouble() - fModel->fRawData[i][4].toDouble();
        fModel->setData(i, 5, total);
    }
    QStringList vheader;
    vheader << QString::number(fModel->rowCount());
    fTableTotal->setVerticalHeaderLabels(vheader);
    fTableTotal->setDouble(0, 3, debit);
    fTableTotal->setDouble(0, 4, credit);
    fTableTotal->setDouble(0, 5, total);
    fTableTotal->setVisible(true);
}
