#include "cr5documents.h"
#include "c5storedoc.h"
#include "c5storeinventory.h"
#include "cr5documentsfilter.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5cashdoc.h"
#include <QMenu>

CR5Documents::CR5Documents(const QStringList &dbParams, QWidget *parent) :
    C5ReportWidget(dbParams, parent)
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
                       ;

    fColumnsFields << "distinct(h.f_id) as f_id"
                   << "h.f_userid as f_docnum"
                   << "ds.f_name as f_statename"
                   << "dt.f_name as f_typename"
                   << "h.f_date"
                   << "p.f_taxname"
                   << "h.f_amount"
                   << "h.f_comment"
                   << "h.f_invoice"
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
    fColumnsVisible["h.f_invoice"] = true;
    fColumnsVisible["u.f_login as f_operatorname"] = true;
    fColumnsVisible["h.f_createdate"] = true;
    fColumnsVisible["h.f_createtime"] = true;

    fOrderCondition = " order by h.f_date, h.f_userid ";

    connect(this, SIGNAL(tblDoubleClicked(int,int,QList<QVariant>)), this, SLOT(tblDoubleClicked(int,int,QList<QVariant>)));

    restoreColumnsVisibility();

    fFilterWidget = new CR5DocumentsFilter(dbParams);
}

QToolBar *CR5Documents::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbEdit
            << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        QAction *a = new QAction(QIcon(":/save.png"), tr("Save\ndocuments"));
        connect(a, SIGNAL(triggered()), this, SLOT(saveDocs()));
        fToolBar->insertAction(fToolBar->actions().at(0), a);
        a = new QAction(QIcon(":/draft.png"), tr("Create\ndraft"));
        connect(a, SIGNAL(triggered()), this, SLOT(draftDocs()));
        fToolBar->insertAction(fToolBar->actions().at(1), a);
        a = new QAction(QIcon(":/recycle.png"), tr("Remove"));
        connect(a, SIGNAL(triggered()), this, SLOT(removeDocs()));
        fToolBar->insertAction(fToolBar->actions().at(2), a);
    }
    return fToolBar;
}

QMenu *CR5Documents::buildTableViewContextMenu(const QPoint &point)
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
    if (fColumnsVisible["distinct(h.f_id) as f_id"]) {
        fTableView->setColumnWidth(fModel->fColumnNameIndex["f_id"], 0);
    }
}

void CR5Documents::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(column);
    Q_UNUSED(row);
    if (!fColumnsVisible["distinct(h.f_id) as f_id"]) {
        C5Message::error(tr("Op.num. column must be included in the report"));
        return;
    }
    openDoc(values.at(fModel->indexForColumnName("f_id")).toString());
}

void CR5Documents::callEditor(const QString &id)
{
    openDoc(id);
}

void CR5Documents::openDoc(QString id)
{
    switch (docType(id)) {
    case DOC_TYPE_STORE_INPUT:
    case DOC_TYPE_STORE_OUTPUT:
    case DOC_TYPE_STORE_MOVE:
    case DOC_TYPE_COMPLECTATION: {
        auto *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
        if (!sd->openDoc(id)) {
            __mainWindow->removeTab(sd);
        }
        break;
    }
    case DOC_TYPE_STORE_INVENTORY: {
        auto *si = __mainWindow->createTab<C5StoreInventory>(fDBParams);
        if (!si->openDoc(id)) {
            __mainWindow->removeTab(si);
        }
        break;
    }
    case DOC_TYPE_CASH: {
        auto *cd = __mainWindow->createTab<C5CashDoc>(fDBParams);
        if (!cd->openDoc(id)) {
            __mainWindow->removeTab(cd);
        }
        break;
    }
    }
}

int CR5Documents::docType(QString id)
{
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select f_type from a_header where f_id=:f_id");
    if (!db.nextRow()) {
        return 0;
    }
    return db.getInt(0);
}

void CR5Documents::saveDocs()
{
    if (!fColumnsVisible["distinct(h.f_id) as f_id"]) {
        C5Message::error(tr("Please, set the 'Code' column to visible."));
        return;
    }
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to save selected documents")) != QDialog::Accepted) {
        return;
    }
    QSet<int> rowsTemp;
    foreach (QModelIndex mi, ml) {
        rowsTemp << mi.row();
    }
    QList<int> rows = rowsTemp.toList();
    std::sort(rows.begin(), rows.end());
    C5Database db(fDBParams);
    QString err;
    foreach (int row, rows) {
        QString docid = fModel->data(row, fModel->indexForColumnName("f_id"), Qt::EditRole).toString();
        db[":f_id"] = docid;
        db.exec("select * from a_header where f_id=:f_id");
        if (!db.nextRow()) {
            continue;
        }
        if (db.getInt("f_state") == DOC_STATE_SAVED) {
            continue;
        }
        C5StoreDoc d(fDBParams);
        QString e;
        switch (db.getInt("f_type")) {
        case DOC_TYPE_STORE_INPUT:
        case DOC_TYPE_STORE_MOVE:
        case DOC_TYPE_STORE_OUTPUT:
            d.openDoc(docid);
            if (d.save(DOC_STATE_SAVED, e, false)) {
                fModel->setData(row, fModel->indexForColumnName("f_statename"), tr("Saved"));
                fModel->setData(row, fModel->indexForColumnName("f_amount"), 0);
            } else {
                err += e;
            }
            e = "";
            break;
        default:
            continue;
            break;
        }
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
    }
}

void CR5Documents::draftDocs()
{
    if (!fColumnsVisible["distinct(h.f_id) as f_id"]) {
        C5Message::error(tr("Please, set the 'Code' column to visible."));
        return;
    }
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to create draft for selected documents")) != QDialog::Accepted) {
        return;
    }
    QSet<int> rowsTemp;
    foreach (QModelIndex mi, ml) {
        rowsTemp << mi.row();
    }
    QList<int> rows = rowsTemp.toList();
    std::sort(rows.begin(), rows.end());
    C5Database db(fDBParams);
    QString err;
    foreach (int row, rows) {
        QString docid = fModel->data(row, fModel->indexForColumnName("f_id"), Qt::EditRole).toString();
        db[":f_id"] = docid;
        db.exec("select * from a_header where f_id=:f_id");
        if (!db.nextRow()) {
            continue;
        }
        if (db.getInt("f_state") == DOC_STATE_DRAFT) {
            continue;
        }
        C5StoreDoc d(fDBParams);
        switch (db.getInt("f_type")) {
        case DOC_TYPE_STORE_INPUT:
        case DOC_TYPE_STORE_MOVE:
        case DOC_TYPE_STORE_OUTPUT:
            d.openDoc(docid);
            if (d.save(DOC_STATE_DRAFT, err, false)) {
                fModel->setData(row, fModel->indexForColumnName("f_statename"), tr("Draft"));
                fModel->setData(row, fModel->indexForColumnName("f_amount"), 0);
            }
            break;
        default:
            continue;
            break;
        }
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
    }
}

void CR5Documents::removeDocs()
{
    if (!fColumnsVisible["distinct(h.f_id) as f_id"]) {
        C5Message::error(tr("Please, set the 'Code' column to visible."));
        return;
    }
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove selected documents")) != QDialog::Accepted) {
        return;
    }
    QSet<int> rowsTemp;
    foreach (QModelIndex mi, ml) {
        rowsTemp << mi.row();
    }
    QList<int> rows = rowsTemp.toList();
    std::sort(rows.begin(), rows.end());
    std::reverse(rows.begin(), rows.end());
    foreach (int r, rows) {
        QString id = fModel->data(r, fModel->indexForColumnName("f_id"), Qt::EditRole).toString();
        switch (docType(id)) {
        case DOC_TYPE_STORE_INPUT:
        case DOC_TYPE_STORE_OUTPUT:
        case DOC_TYPE_STORE_MOVE:
            if (C5StoreDoc::removeDoc(fDBParams, id, false)) {
                fModel->removeRow(r);
            } else {
                return;
            }
            break;
        case DOC_TYPE_STORE_INVENTORY:
            if (C5StoreInventory::removeDoc(fDBParams, id)) {
                fModel->removeRow(r);
            } else {
                return;
            }
            break;
        case DOC_TYPE_CASH:
            if (C5CashDoc::removeDoc(fDBParams, id)) {
                fModel->removeRow(r);
            } else {
                return;
            }
        }
    }
}

void CR5Documents::copySelectedDocs()
{
    if (!fColumnsVisible["distinct(h.f_id) as f_id"]) {
        C5Message::error(tr("Please, set the 'Code' column to visible."));
        return;
    }
    QModelIndexList ml = fTableView->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    QSet<int> rowsTemp;
    foreach (QModelIndex mi, ml) {
        rowsTemp << mi.row();
    }
    QList<int> rows = rowsTemp.toList();
    std::sort(rows.begin(), rows.end());
    std::reverse(rows.begin(), rows.end());
    C5Database db1(fDBParams);
    C5Database db2(fDBParams);
    foreach (int r, rows) {
        QString id = fModel->data(r, fModel->indexForColumnName("f_id"), Qt::EditRole).toString();
        db1[":f_id"] = id;
        db1.exec("select * from a_header where f_id=:f_id");
        if (!db1.nextRow()) {
            continue;
        }
        QString newDocId = C5Database::uuid();
        db2.setBindValues(db1.getBindValues());
        db2.removeBindValue(":f_id");
        db2[":f_state"] = DOC_STATE_DRAFT;
        db2[":f_id"] = newDocId;
        db2.insert("a_header", false);
        QString tableName;
        switch (db1.getInt("f_type")) {
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
        while (db1.nextRow()) {
            db2.setBindValues(db1.getBindValues());
            db2[":f_id"] = C5Database::uuid();
            db2[":f_document"] = newDocId;
            if (!db2.insert(tableName, false)) {
                C5Message::error(db2.fLastError);
            }
        }
        openDoc(newDocId);
    }
}
