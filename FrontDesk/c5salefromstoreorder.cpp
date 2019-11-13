#include "c5salefromstoreorder.h"
#include "ui_c5salefromstoreorder.h"
#include "c5mainwindow.h"
#include <QMenu>

C5SaleFromStoreOrder::C5SaleFromStoreOrder(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::C5SaleFromStoreOrder)
{
    ui->setupUi(this);
    ui->tblData->setColumnWidths(ui->tblData->columnCount(), 0, 300, 80, 80, 80, 80);
    ui->leID->setVisible(false);
    connect(ui->tblData, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tblContextMenu(QPoint)));
}

C5SaleFromStoreOrder::~C5SaleFromStoreOrder()
{
    delete ui;
}

void C5SaleFromStoreOrder::openOrder(const QStringList &dbParams, const QString &id)
{
    C5SaleFromStoreOrder *d = new C5SaleFromStoreOrder(dbParams);
    d->loadOrder(id);
    d->exec();
    delete d;
}

void C5SaleFromStoreOrder::closeEvent(QCloseEvent *e)
{
    if (fDeleteRows.count() > 0) {
        if (C5Message::question(tr("Save changes?")) == QDialog::Accepted) {
            saveChanges();
        }
    }
    C5Dialog::closeEvent(e);
}

void C5SaleFromStoreOrder::loadOrder(const QString &id)
{
    ui->leID->setText(id);
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select f_datecash, f_timeclose, concat(f_prefix, f_hallid) as f_userid from o_header where f_id=:f_id");
    if (db.nextRow()) {
        ui->deDate->setDate(db.getDate(0));
        ui->teTime->setText(db.getString(1));
        ui->leUserId->setText(db.getString("f_userid"));
    } else {
        C5Message::error(tr("No such order"));
        return;
    }
    db[":f_id"] = id;
    db.exec("select og.f_id, g.f_name, og.f_qty, gu.f_name, og.f_price, og.f_total "
            "from o_goods og "
            "left join c_goods g on g.f_id=og.f_goods "
            "left join c_units gu on gu.f_id=g.f_unit "
            "where og.f_header=:f_id");
    while (db.nextRow()) {
        int row = ui->tblData->addEmptyRow();
        ui->tblData->setString(row, 0, db.getString(0));
        ui->tblData->setString(row, 1, db.getString(1));
        ui->tblData->setDouble(row, 2, db.getDouble(2));
        ui->tblData->setString(row, 3, db.getString(3));
        ui->tblData->setDouble(row, 4, db.getDouble(4));
        ui->tblData->setDouble(row, 5, db.getDouble(5));
    }
    countTotal();
}

void C5SaleFromStoreOrder::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblData->rowCount(); i++) {
        total += ui->tblData->getDouble(i, 5);
    }
    ui->leTotal->setDouble(total);
}

void C5SaleFromStoreOrder::saveChanges()
{
    C5Database db(fDBParams);
    db.startTransaction();
    foreach (QString i, fDeleteRows) {
        db[":f_id"] = i;
        db.exec("select f_storerec, f_qty from o_goods where f_id=:f_id");
        if (db.nextRow()) {
            QString recid = db.getString("f_storerec");
            double qty = db.getDouble("f_qty");
            db[":f_id"] = recid;
            db.exec("select f_document, f_qty from a_store_draft where f_id=:f_id for update");
            if (db.nextRow()) {
                double draftQty = db.getDouble("f_qty");
                db[":f_qty"] = qty;
                db[":f_id"] = recid;
                db.exec("update a_store_draft set f_qty=f_qty-:f_qty where f_id=:f_id");
                if (draftQty - qty < 0.0001) {
                    db[":f_id"] = recid;
                    db.exec("delete from a_store_draft where f_id=:f_id");
                }
                db[":f_id"] = db.getString("f_document");
                db.exec("select * from a_header where f_id=:f_id");
                if (db.nextRow()) {
                    if (db.getInt("f_state") == DOC_STATE_SAVED) {
                        C5Message::error(tr("Store document has been registered for this sale"));
                        db.rollback();
                        return;
                    }
                }
            }
        }
        db[":f_id"] = i;
        db.exec("delete from o_goods where f_id=:f_id");
    }
    if (ui->tblData->rowCount() == 0) {
        db[":f_id"] = ui->leID->text();
        db.exec("delete from o_header where f_id=:f_id");
    } else {
        db[":f_amountTotal"] = ui->leTotal->getDouble();
        db[":f_amountCash"] = ui->leTotal->getDouble();
        db.update("o_header", where_id(ui->leID->text()));
    }
    db.commit();
}

void C5SaleFromStoreOrder::tblContextMenu(const QPoint &point)
{
    QMenu *menu=new QMenu(this);
    QAction *a = menu->addAction(tr("Delete"));
    connect(a, SIGNAL(triggered(bool)), this, SLOT(actionDelete(bool)));
    menu->popup(ui->tblData->viewport()->mapToGlobal(point));
}

void C5SaleFromStoreOrder::actionDelete(bool v)
{
    Q_UNUSED(v);
    QModelIndexList ml = ui->tblData->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    if (C5Message::question(tr("Are you sure to delete selected row?")) != QDialog::Accepted) {
        return;
    }
    fDeleteRows.append(ui->tblData->getString(ml.at(0).row(), 0));
    ui->tblData->removeRow(ml.at(0).row());
    countTotal();
}
