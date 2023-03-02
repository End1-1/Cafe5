#include "cr5reports.h"
#include "c5tablemodel.h"
#include "c5tablewidget.h"
#include "c5mainwindow.h"
#include "c5saledoc.h"
#include "cr5reportsfilter.h"

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
    QStringList sumColumns = db.getString("f_sumcolumn_indexes").split(",", Qt::SkipEmptyParts);
    for (const QString &s: qAsConst(sumColumns)) {
        fColumnsSumIndex.append(s.toInt());
    }
    fFilter->restoreFilter(this);
    fSqlQuery = fQuery;
    fSqlQuery.replace("%date1", fFilter->d1()).replace("%date2", fFilter->d2());
    buildQuery();
}

void CR5Reports::buildQuery()
{
    C5ReportWidget::buildQuery();
}

bool CR5Reports::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
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
        auto *retaildoc = __mainWindow->createTab<C5SaleDoc>(fDBParams);
        retaildoc->setMode(1);
        if (!retaildoc->reportHandler(REPORT_HANDLER_SALE_DOC_OPEN_DRAFT, values.at(0))) {

        }
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
    QSet<int> rowsSet;
    for (const QModelIndex &ml: mil) {
        rowsSet.insert(ml.row());
    }
    QList<int> rows = rowsSet.toList();
    qSort(rows);
    QStringList queries = fDeleteHandler.split(";", Qt::SkipEmptyParts);
    C5Database db(fDBParams);
    for (int r: rows) {
        for (QString &q: queries) {
            db.exec(q.replace("%handler%", fModel->data(r, 0, Qt::EditRole).toString()));
        }
    }
    std::reverse(rows.begin(), rows.end());
    for (int r: rows) {
        fModel->removeRow(r);
    }
}

