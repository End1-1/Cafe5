#include "cr5documents.h"
#include "c5storedoc.h"
#include "c5storeinventory.h"
#include "cr5documentsfilter.h"
#include "c5tablemodel.h"

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
                       ;

    fColumnsFields << "h.f_id"
                   << "ds.f_name as f_statename"
                   << "dt.f_name as f_typename"
                   << "h.f_date"
                   << "p.f_taxname"
                   << "h.f_amount"
                   << "h.f_comment"
                   << "u.f_login as f_operatorname"
                   << "h.f_createdate"
                   << "h.f_createtime"
                      ;

    fColumnsSum << "f_amount";

    fTranslation["f_id"] = tr("Code");
    fTranslation["f_statename"] = tr("State");
    fTranslation["f_typename"] = tr("Type");
    fTranslation["f_date"] = tr("Date");
    fTranslation["f_taxname"] = tr("Partner");
    fTranslation["f_amount"] = tr("Amount");
    fTranslation["f_comment"] = tr("Comment");
    fTranslation["f_operatorname"] = tr("Operator");
    fTranslation["f_createdate"] = tr("Last change date");
    fTranslation["f_createtime"] = tr("Last change time");

    fColumnsVisible["h.f_id"] = true;
    fColumnsVisible["ds.f_name as f_statename"] = true;
    fColumnsVisible["dt.f_name as f_typename"] = true;
    fColumnsVisible["h.f_date"] = true;
    fColumnsVisible["p.f_taxname"] = true;
    fColumnsVisible["h.f_amount"] = true;
    fColumnsVisible["h.f_comment"] = true;
    fColumnsVisible["u.f_login as f_operatorname"] = true;
    fColumnsVisible["h.f_createdate"] = true;
    fColumnsVisible["h.f_createtime"] = true;

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

void CR5Documents::tblDoubleClicked(int row, int column, const QList<QVariant> &values)
{
    Q_UNUSED(column);
    Q_UNUSED(row);
    openDoc(values.at(0).toInt());
}

void CR5Documents::callEditor(int id)
{
    openDoc(id);
}

void CR5Documents::openDoc(int id)
{
    switch (docType(id)) {
    case DOC_TYPE_STORE_INPUT:
    case DOC_TYPE_STORE_OUTPUT:
    case DOC_TYPE_STORE_MOVE: {
        C5StoreDoc *sd = __mainWindow->createTab<C5StoreDoc>(fDBParams);
        if (!sd->openDoc(id)) {
            __mainWindow->removeTab(sd);
        }
        break;
    }
    case DOC_TYPE_STORE_INVENTORY: {
        C5StoreInventory *si = __mainWindow->createTab<C5StoreInventory>(fDBParams);
        if (!si->openDoc(id)) {
            __mainWindow->removeTab(si);
        }
        break;
    }
    }
}

int CR5Documents::docType(int id)
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
    if (!fColumnsVisible["h.f_id"]) {
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
        int docid = fModel->data(row, fModel->indexForColumnName("f_id"), Qt::EditRole).toInt();
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
    if (!fColumnsVisible["h.f_id"]) {
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
        int docid = fModel->data(row, fModel->indexForColumnName("f_id"), Qt::EditRole).toInt();
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
    if (!fColumnsVisible["h.f_id"]) {
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
        int id = fModel->data(r, fModel->indexForColumnName("f_id"), Qt::EditRole).toInt();
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
        }
    }
}
