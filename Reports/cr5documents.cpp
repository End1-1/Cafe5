#include "cr5documents.h"
#include "c5storedoc.h"
#include "c5storeinventory.h"
#include "cr5documentsfilter.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5salarydoc.h"
#include "c5storedraftwriter.h"
#include "c5cashdoc.h"
#include "c5progressdialog.h"
#include "c5waiterorder.h"
#include "c5dlgselectreporttemplate.h"
#include "c5salefromstoreorder.h"
#include "storeinputdocument.h"
#include <QMenu>

CR5Documents::CR5Documents(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIcon = ":/documents.png";
    fLabel = tr("Documents");
    fSimpleQuery = false;
    fMainTable = "a_header h";
    fLeftJoinTables << "left join s_user u on u.f_id=h.f_operator [u]"
                    << "left join c_partners p on p.f_id=h.f_partner [p]"
                    << "left join a_state ds on ds.f_id=h.f_state [ds]"
                    << "left join a_type dt on dt.f_id=h.f_type [dt]"
                    << "left join a_store sa on sa.f_document=h.f_id [sa]"
                    << "left join a_header_store hs on hs.f_id=h.f_id [hs]"
                    ;
    fColumnsFields << "distinct(h.f_id) as f_id"
                   << "h.f_userid as f_docnum"
                   << "ds.f_name as f_statename"
                   << "dt.f_name as f_typename"
                   << "h.f_date"
                   << "p.f_taxname"
                   << "h.f_amount"
                   << "h.f_comment"
                   << "hs.f_invoice"
                   << "u.f_login as f_operatorname"
                   << "h.f_createdate"
                   << "h.f_createtime"
                   ;
    fColumnsSum << "f_amount";
    fTranslation["f_id"] = tr("Op.num.");
    fTranslation["f_docnum"] = tr("Code");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_typename"] = tr("Type");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_taxname"] = tr("Partner");
    fTranslation["f_amount"] = tr("Amount");
    fTranslation["f_comment"] = tr("Comment");
    fTranslation["f_invoice"] = tr("Invoice");
    fTranslation["f_operatorname"] = tr("Operator");
    fTranslation["f_createdate"] = tr("Last change date");
    fTranslation["f_createtime"] = tr("Last change time");
    fColumnsVisible["distinct(h.f_id) as f_id"] = true;
    fColumnsVisible["h.f_userid as f_docnum"] = true;
    fColumnsVisible["ds.f_name as f_statename"] = true;
    fColumnsVisible["dt.f_name as f_typename"] = true;
    fColumnsVisible["h.f_date"] = true;
    fColumnsVisible["p.f_taxname"] = true;
    fColumnsVisible["h.f_amount"] = true;
    fColumnsVisible["h.f_comment"] = true;
    fColumnsVisible["hs.f_invoice"] = true;
    fColumnsVisible["u.f_login as f_operatorname"] = true;
    fColumnsVisible["h.f_createdate"] = true;
    fColumnsVisible["h.f_createtime"] = true;
    fOrderCondition = " order by h.f_date, h.f_userid ";
    restoreColumnsVisibility();
    fFilterWidget = new CR5DocumentsFilter();
    fFilter = static_cast<CR5DocumentsFilter*>(fFilterWidget);
}

QToolBar* CR5Documents::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbEdit
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        QAction *a = new QAction(QIcon(":/save.png"), tr("Save\ndocuments"), this);
        connect(a, SIGNAL(triggered()), this, SLOT(saveDocs()));
        fToolBar->insertAction(fToolBar->actions().at(0), a);
        a = new QAction(QIcon(":/draft.png"), tr("Create\ndraft"), this);
        connect(a, SIGNAL(triggered()), this, SLOT(draftDocs()));
        fToolBar->insertAction(fToolBar->actions().at(1), a);
        a = new QAction(QIcon(":/recycle.png"), tr("Remove"), this);
        connect(a, SIGNAL(triggered()), this, SLOT(removeDocs()));
        fToolBar->insertAction(fToolBar->actions().at(2), a);
        a = new QAction(QIcon(":/template.png"), tr("Templates"), this);
        connect(a, SIGNAL(triggered()), this, SLOT(templates()));
        fToolBar->insertAction(fToolBar->actions().at(3), a);
    }

    return fToolBar;
}

QMenu* CR5Documents::buildTableViewContextMenu(const QPoint &point)
{
    QMenu *m = C5Grid::buildTableViewContextMenu(point);
    m->addSeparator();
    QAction *a = new QAction(QIcon(":/copy.png"), tr("Copy selected documents"), this);
    connect(a, SIGNAL(triggered()), this, SLOT(copySelectedDocs()));
    m->addAction(a);
    return m;
}

void CR5Documents::restoreColumnsWidths()
{
    C5Grid::restoreColumnsWidths();

    if(fColumnsVisible["distinct(h.f_id) as f_id"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
    }
}

bool CR5Documents::tblDoubleClicked(int row, int column, const QJsonArray &values)
{
    Q_UNUSED(column);
    Q_UNUSED(row);

    if(!fColumnsVisible["distinct(h.f_id) as f_id"]) {
        C5Message::error(tr("Op.num. column must be included in the report"));
        return true;
    }

    openDoc(values.at(fModel->indexForColumnName("f_id")).toString());
    return true;
}

void CR5Documents::callEditor(const QString &id)
{
    openDoc(id);
}

void CR5Documents::openDoc(QString id)
{
    QString err;

    switch(docType(id)) {
    case DOC_TYPE_STORE_INPUT: {
#ifdef NEWVERSION
        auto *si = __mainWindow->createTab<StoreInputDocument>();

        if(!si->openDoc(id)) {
            __mainWindow->removeTab(si);
        }

#else
        auto *sa = __mainWindow->createTab<C5StoreDoc>();

        if(!sa->openDoc(id, err)) {
            __mainWindow->removeTab(sa);
            C5Message::error(err);
        }

#endif
        break;
    }

    case DOC_TYPE_STORE_OUTPUT:
    case DOC_TYPE_STORE_MOVE:
    case DOC_TYPE_COMPLECTATION: {
        auto *sd = __mainWindow->createTab<C5StoreDoc>();

        if(!sd->openDoc(id, err)) {
            __mainWindow->removeTab(sd);
            C5Message::error(err);
        }

        break;
    }

    case DOC_TYPE_STORE_INVENTORY: {
        auto *si = __mainWindow->createTab<C5StoreInventory>();

        if(!si->openDoc(id)) {
            __mainWindow->removeTab(si);
        }

        break;
    }

    case DOC_TYPE_CASH: {
        auto *cd = __mainWindow->createTab<C5CashDoc>();

        if(!cd->openDoc(id)) {
            __mainWindow->removeTab(cd);
        }

        break;
    }

    case DOC_TYPE_SALE_INPUT:
        C5Database db;
        db[":f_id"] = id;
        db.exec("select f_source from o_header where f_id=:f_id");

        if(db.nextRow()) {
            switch(abs(db.getInt(0))) {
            case 1: {
                C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>();
                wo->setOrder(id);
                break;
            }

            case 2: {
                C5SaleFromStoreOrder::openOrder(id);
                break;
            }

            default: {
                C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>();
                wo->setOrder(id);
                break;
            }
            }
        } else {
            C5WaiterOrder *wo = __mainWindow->createTab<C5WaiterOrder>();
            wo->setOrder(id);
        }

        break;
    }
}

int CR5Documents::docType(QString id)
{
    C5Database db;
    db[":f_id"] = id;
    db.exec("select f_type from a_header where f_id=:f_id");

    if(!db.nextRow()) {
        return 0;
    }

    return db.getInt(0);
}

void CR5Documents::saveDocs()
{
    if(!fColumnsVisible["distinct(h.f_id) as f_id"]) {
        C5Message::error(tr("Please, set the 'Code' column to visible."));
        return;
    }

    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();

    if(ml.count() == 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to save selected documents")) != QDialog::Accepted) {
        return;
    }

    QSet<int> rowsTemp;

    foreach(QModelIndex mi, ml) {
        rowsTemp << mi.row();
    }

    QList<int> rows = rowsTemp.values();
    std::sort(rows.begin(), rows.end());
    auto *pd = new C5ProgressDialog(this);
    pd->setMax(rows.count());
    connect(this, SIGNAL(updateProgressValueWithMessage(int, QString)), pd, SLOT(updateProgressValueWithMessage(int, QString)));
    pd->show();
    C5Database db;
    QString err;
    int rowLeft = 0;
    int complete = 0;

    foreach(int row, rows) {
        QString docid = fModel->data(row, fModel->indexForColumnName("f_id"), Qt::EditRole).toString();
        db[":f_id"] = docid;
        db.exec("select * from a_header where f_id=:f_id");

        if(!db.nextRow()) {
            continue;
        }

        if(db.getInt("f_state") == DOC_STATE_SAVED) {
            emit updateProgressValueWithMessage(++rowLeft, QString("%1 %2, %3 %4").arg(tr("Success")).arg(complete).arg(tr("Fail")).arg(rowLeft - complete));
            continue;
        }

        C5StoreDoc d;
        QString e;

        switch(db.getInt("f_type")) {
        case DOC_TYPE_STORE_INPUT:
        case DOC_TYPE_STORE_MOVE:
        case DOC_TYPE_STORE_OUTPUT:
        case DOC_TYPE_COMPLECTATION:
            if(d.openDoc(docid, e)) {
                if(d.writeDocument(DOC_STATE_SAVED, e)) {
                    fModel->setData(row, fModel->indexForColumnName("f_statename"), tr("Saved"));
                    fModel->setData(row, fModel->indexForColumnName("f_amount"), 0);
                    complete++;
                } else {
                    err += e;
                }
            } else {
                err += e;
            }

            e = "";
            break;

        default:
            continue;
            break;
        }

        emit updateProgressValueWithMessage(++rowLeft, QString("%1 %2, %3 %4").arg(tr("Success")).arg(complete).arg(tr("Fail")).arg(rowLeft - complete));
    }

    delete pd;

    if(!err.isEmpty()) {
        C5Message::error(err);
    }
}

void CR5Documents::draftDocs()
{
    if(!fColumnsVisible["distinct(h.f_id) as f_id"]) {
        C5Message::error(tr("Please, set the 'Code' column to visible."));
        return;
    }

    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();

    if(ml.count() == 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to create draft for selected documents")) != QDialog::Accepted) {
        return;
    }

    QSet<int> rowsTemp;

    foreach(QModelIndex mi, ml) {
        rowsTemp << mi.row();
    }

    QList<int> rows = rowsTemp.values();
    std::sort(rows.begin(), rows.end());
    C5Database db;
    QString err;

    foreach(int row, rows) {
        QString docid = fModel->data(row, fModel->indexForColumnName("f_id"), Qt::EditRole).toString();
        db[":f_id"] = docid;
        db.exec("select * from a_header where f_id=:f_id");

        if(!db.nextRow()) {
            continue;
        }

        if(db.getInt("f_state") == DOC_STATE_DRAFT) {
            continue;
        }

        QString e;
        C5StoreDoc d;

        switch(db.getInt("f_type")) {
        case DOC_TYPE_STORE_INPUT:
        case DOC_TYPE_STORE_MOVE:
        case DOC_TYPE_STORE_OUTPUT:
        case DOC_TYPE_COMPLECTATION:
            if(d.openDoc(docid, e)) {
                if(d.writeDocument(DOC_STATE_DRAFT, e)) {
                    fModel->setData(row, fModel->indexForColumnName("f_statename"), tr("Draft"));
                    fModel->setData(row, fModel->indexForColumnName("f_amount"), 0);
                } else {
                    err += e;
                }
            } else {
                err += e;
            }

            break;

        default:
            continue;
            break;
        }
    }

    if(!err.isEmpty()) {
        C5Message::error(err);
    }
}

void CR5Documents::removeDocs()
{
    if(!fColumnsVisible["distinct(h.f_id) as f_id"]) {
        C5Message::error(tr("Please, set the 'Code' column to visible."));
        return;
    }

    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();

    if(ml.count() == 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove selected documents")) != QDialog::Accepted) {
        return;
    }

    QSet<int> rowsTemp;

    foreach(QModelIndex mi, ml) {
        rowsTemp << mi.row();
    }

    QList<int> rows = rowsTemp.values();
    std::sort(rows.begin(), rows.end());
    std::reverse(rows.begin(), rows.end());

    foreach(int r, rows) {
        QString id = fModel->data(r, fModel->indexForColumnName("f_id"), Qt::EditRole).toString();

        switch(docType(id)) {
        case DOC_TYPE_STORE_INPUT:
        case DOC_TYPE_STORE_OUTPUT:
        case DOC_TYPE_STORE_MOVE:
        case DOC_TYPE_COMPLECTATION:
            if(C5StoreDoc::removeDoc(id, false)) {
                fModel->removeRow(r);
            } else {
                return;
            }

            break;

        case DOC_TYPE_STORE_INVENTORY:
            if(C5StoreInventory::removeDoc(id)) {
                fModel->removeRow(r);
            } else {
                return;
            }

            break;

        case DOC_TYPE_CASH:
            if(C5CashDoc::removeDoc(id)) {
                fModel->removeRow(r);
            } else {
                return;
            }

            break;
        }
    }
}

void CR5Documents::copySelectedDocs()
{
    if(!fColumnsVisible["distinct(h.f_id) as f_id"]) {
        C5Message::error(tr("Please, set the 'Code' column to visible."));
        return;
    }

    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();

    if(ml.count() == 0) {
        return;
    }

    QSet<int> rowsTemp;

    foreach(QModelIndex mi, ml) {
        rowsTemp << mi.row();
    }

    QList<int> rows = rowsTemp.values();
    std::sort(rows.begin(), rows.end());
    std::reverse(rows.begin(), rows.end());
    C5Database db1;
    C5Database db2;

    foreach(int r, rows) {
        QString id = fModel->data(r, fModel->indexForColumnName("f_id"), Qt::EditRole).toString();
        db1[":f_id"] = id;
        db1.exec("select * from a_header where f_id=:f_id");

        if(!db1.nextRow()) {
            continue;
        }

        QString newDocId = C5Database::uuid();
        db2.setBindValues(db1.getBindValues());
        db2.removeBindValue(":f_id");
        db2[":f_state"] = DOC_STATE_DRAFT;
        db2[":f_id"] = newDocId;
        db2.insert("a_header", false);
        QString tableName;

        switch(db1.getInt("f_type")) {
        case DOC_TYPE_STORE_INPUT:
        case DOC_TYPE_STORE_MOVE:
        case DOC_TYPE_STORE_OUTPUT:
            tableName = "a_store_draft";
            break;

        default:
            tableName = "UNKNOWN";
        }

        db1[":f_document"] = id;
        db1.exec(QString("select * from %1 where f_document=:f_document").arg(tableName));

        while(db1.nextRow()) {
            db2.setBindValues(db1.getBindValues());
            db2[":f_id"] = C5Database::uuid();
            db2[":f_document"] = newDocId;

            if(!db2.insert(tableName, false)) {
                C5Message::error(db2.fLastError);
            }
        }

        openDoc(newDocId);
    }
}

void CR5Documents::templates()
{
    C5DlgSelectReportTemplate d(1);

    if(d.exec() == QDialog::Accepted) {
        QString sql = d.fSelectedTemplate.sql;
        sql.replace("%date1", fFilter->date1().toString(FORMAT_DATE_TO_STR_MYSQL));
        sql.replace("%date2", fFilter->date2().toString(FORMAT_DATE_TO_STR_MYSQL));
        executeSql(sql);
    }
}
