#include "cr5reports.h"
#include "c5tablemodel.h"
#include "c5tablewidget.h"
#include "c5mainwindow.h"
#include "c5storedoc.h"
#include "c5saledoc.h"
#include "cr5reportsfilter.h"
#include "ntablewidget.h"
#include <QJsonObject>

#define REPORT_HANDLER_GIFT_CARD_TOTAL "ff64e987-a0b2-11ef-b479-022165c6dab1"

CR5Reports::CR5Reports(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
{
    fFilter = new CR5ReportsFilter(dbParams);
    fFilterWidget = fFilter;
}

QToolBar *CR5Reports::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }
    return fToolBar;
}

void CR5Reports::setReport(int id)
{
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from reports where f_id=:f_id");
    if (db.nextRow() == false) {
        C5Message::error(tr("Invalid report id"));
        return;
    }
    fHandlerUuid = db.getString("f_handler");
    fDeleteHandler = db.getString("f_deletehandler");
    fFilterHandler = db.getString("f_filterhandler");
    fQuery = db.getString("f_query");
    fLabel = db.getString("f_name");
    QStringList sumColumns = db.getString("f_sumcolumn_indexes").split(",", Qt::SkipEmptyParts);
    for (const QString &s : qAsConst(sumColumns)) {
        fColumnsSumIndex.append(s.toInt());
    }
    db[":f_id"] = fFilterHandler;
    db.exec("select * from reports_handler where f_id=:f_id");
    if (db.nextRow()) {
        fFilter->setFields(db.getString("f_query").split(",", Qt::SkipEmptyParts));
    }
    if (!fDeleteHandler.isEmpty()) {
        db[":f_id"] = fDeleteHandler;
        db.exec("select * from reports_handler where f_id=:f_id");
        if (db.nextRow()) {
            fDeleteHandler = db.getString("f_query");
            if (!fDeleteHandler.isEmpty()) {
                fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(removeHandler(bool)));
            }
        }
        //connect(a, &QAction::triggered, this, &CR5Reports::removeHandler);
    }
    fFilter->restoreFilter(this);
    buildQuery();
}

void CR5Reports::buildQuery()
{
    fSqlQuery = fQuery;
    fSqlQuery.replace("\r\n", " ");
    fSqlQuery.replace("%date1", fFilter->d1()).replace("%date2", fFilter->d2());
    fSqlQuery.replace("%filter", fFilter->replacement());
    C5ReportWidget::buildQuery();
    if (fColumnsFields.contains("color")) {
        int col = fColumnsFields.indexOf("color");
        for (int i = 0; i < fModel->rowCount(); i++) {
            fModel->setRowColor(i,  fModel->data(i, col, Qt::EditRole).toInt() == 0 ? Qt::white : Qt::red);
        }
        fTableView->setColumnWidth(col, 0);
    }
}

bool CR5Reports::tblDoubleClicked(int row, int column, const QJsonArray &values)
{
    Q_UNUSED(row);
    Q_UNUSED(column);
    if (fHandlerUuid.isEmpty()) {
        return true;
    }
    if (values.isEmpty()) {
        return true;
    }
    if (fHandlerUuid == REPORT_HANDLER_SALE_DOC_OPEN_DRAFT) {
        C5Database db(fDBParams);
        db[":f_id"] = values.at(0).toString();
        db.exec("select * from o_draft_sale where f_id=:f_id");
        if (!db.nextRow()) {
            C5Message::error(tr("Program error"), "draft not found by id");
            return false;
        }
        int type = db.getInt("f_saletype");
        if (type == 3) {
            QString err;
            auto *storedoc = __mainWindow->createTab<C5StoreDoc>(fDBParams);
            storedoc->setProperty("fromdraft", values.at(0).toString());
            storedoc->setMode(C5StoreDoc::sdInput);
            if (!storedoc->openDraft(values.at(0).toString(), err))  {
                C5Message::error(err);
                return false;
            }
        } else {
            auto *retaildoc = __mainWindow->createTab<C5SaleDoc>(fDBParams);
            retaildoc->setMode(1);
            if (!retaildoc->reportHandler(REPORT_HANDLER_SALE_DOC_OPEN_DRAFT, values.at(0))) {
            }
        }
    } else if (fHandlerUuid == REPORT_HANDLER_GIFT_CARD_TOTAL) {
        __mainWindow->createNTab("/engine/reports/gifthistorydetails.php", "", QJsonObject{{"card", values.at(1).toString()}});
    }
    return true;
}

void CR5Reports::setSearchParameters()
{
    C5ReportWidget::setSearchParameters();
    fSqlQuery = fQuery;
    fSqlQuery.replace("%date1", fFilter->d1()).replace("%date2", fFilter->d2());
    buildQuery();
}

void CR5Reports::completeRefresh()
{
    fTableView->resizeColumnsToContents();
    fTableView->setColumnWidth(0, 0);
    for (int i = 0; i < fModel->columnCount(); i++) {
        fTableTotal->setColumnWidth(i, fTableView->columnWidth(i));
    }
}

void CR5Reports::removeHandler(bool checked)
{
    Q_UNUSED(checked);
    QModelIndexList mil = fTableView->selectionModel()->selectedIndexes();
    if (mil.isEmpty()) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove selected objects")) != QDialog::Accepted) {
        return;
    }
    QSet<int> rowsSet;
    for (const QModelIndex &ml : mil) {
        rowsSet.insert(ml.row());
    }
    QList<int> rows = rowsSet.values();
    std::sort(rows.begin(), rows.end());
    QStringList queries = fDeleteHandler.split(";", Qt::SkipEmptyParts);
    C5Database db(fDBParams);
    for (int r : rows) {
        for (QString &q : queries) {
            db.exec(q.replace("%handler%", fModel->data(r, 0, Qt::EditRole).toString()));
        }
    }
    std::reverse(rows.begin(), rows.end());
    for (int r : rows) {
        fModel->removeRow(r);
    }
}
