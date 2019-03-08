#include "c5datasynchronize.h"
#include "ui_c5datasynchronize.h"
#include "c5datasyncthread.h"
#include <QMutex>
#include <QScrollBar>
#include <QMenu>

QMutex mutex;

C5DataSynchronize::C5DataSynchronize(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::C5DataSynchronize)
{
    ui->setupUi(this);
    C5Database db(dbParams);
    db.exec("select f_id, f_name, f_host, f_db, f_user, f_password from s_db where f_main=0");
    while (db.nextRow()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lstDatabases);
        item->setText(db.getString(1));
        item->setData(Qt::UserRole + 0, db.getString(2));
        item->setData(Qt::UserRole + 1, db.getString(3));
        item->setData(Qt::UserRole + 2, db.getString(4));
        item->setData(Qt::UserRole + 3, db.getString(5));
        item->setData(Qt::UserRole + 4, db.getString(1));
        ui->lstDatabases->addItem(item);

        int row = ui->tblUp->addEmptyRow();
        QTableWidgetItem *tItem = ui->tblUp->item(row, 0);
        tItem->setText(db.getString(1));
        tItem->setData(Qt::UserRole + 0, db.getString(2));
        tItem->setData(Qt::UserRole + 1, db.getString(3));
        tItem->setData(Qt::UserRole + 2, db.getString(4));
        tItem->setData(Qt::UserRole + 3, db.getString(5));
        tItem->setData(Qt::UserRole + 4, db.getString(1));
    }
    ui->tblSyncTables->setColumnWidths(ui->tblSyncTables->columnCount(), 0, 200);
}

C5DataSynchronize::~C5DataSynchronize()
{
    delete ui;
}

void C5DataSynchronize::removeSyncTable()
{
    QModelIndexList ml = ui->tblSyncTables->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    fRemovedSyncTables << ui->tblSyncTables->getString(ml.at(0).row(), 1);
    ui->tblSyncTables->removeRow(ml.at(0).row());
}

void C5DataSynchronize::on_btnStartUpload_clicked()
{
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        downloadData();
        break;
    case 1:
        uploadData();
        break;
    default:
        return;
    }
    ui->btnStartUpload->setEnabled(false);
}

void C5DataSynchronize::statusUpdate(const QString &name, int row, const QString &msg)
{
    QListWidgetItem *item = ui->lstDatabases->item(row);
    if (!item) {
        return;
    }
    item->setText(name + ": " + msg);
}

void C5DataSynchronize::statusUpdateUpload(int row, int column, const QString &msg)
{
    ui->tblUp->setString(row, column, msg);
}

void C5DataSynchronize::progressBarMax(int max)
{
    ui->progressBar->setMaximum(ui->progressBar->maximum() + max);
}

void C5DataSynchronize::progressBarValue(int value)
{
    ui->progressBar->setValue(ui->progressBar->value() + value);
}

void C5DataSynchronize::dstFinished()
{
    fThreadsCount--;
    if (fThreadsCount == 0) {
        ui->btnStartUpload->setEnabled(true);
        ui->progressBar->setValue(ui->progressBar->maximum());
    }
}

void C5DataSynchronize::errLog(const QString &msg)
{
    ui->teLog->moveCursor(QTextCursor::End);
    ui->teLog->textCursor().insertText(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    ui->teLog->textCursor().insertText(": ");
    ui->teLog->textCursor().insertText(msg);
    ui->teLog->textCursor().insertText("\n");
    ui->teLog->verticalScrollBar()->setValue(ui->teLog->verticalScrollBar()->maximum());
}

void C5DataSynchronize::downloadData()
{
    fThreadsCount = ui->lstDatabases->count();
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
    for (int i = 0; i < ui->lstDatabases->count(); i++) {
        QListWidgetItem *item = ui->lstDatabases->item(i);
        QStringList dbParams;
        dbParams << item->data(Qt::UserRole + 0).toString()
                 << item->data(Qt::UserRole + 1).toString()
                 << item->data(Qt::UserRole + 2).toString()
                 << item->data(Qt::UserRole + 3).toString()
                 << item->data(Qt::UserRole + 4).toString();
        C5DataSyncThread *dst = new C5DataSyncThread(dbParams, fDBParams, i, this);
        connect(dst, SIGNAL(statusUpdated(QString,int,QString)), this, SLOT(statusUpdate(QString,int,QString)));
        connect(dst, SIGNAL(finished()), this, SLOT(dstFinished()));
        connect(dst, SIGNAL(progressBarMax(int)), this, SLOT(progressBarMax(int)));
        connect(dst, SIGNAL(progressBarValue(int)), this, SLOT(progressBarValue(int)));
        dst->runWithDate(ui->deStart->date(), ui->deEnd->date(), ui->chRemoveFromSource->isChecked());
    }
}

void C5DataSynchronize::uploadData()
{
    fThreadsCount = ui->lstDatabases->count();
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
    for (int i = 0; i < ui->lstDatabases->count(); i++) {
        QListWidgetItem *item = ui->lstDatabases->item(i);
        QStringList dbParams;
        dbParams << item->data(Qt::UserRole + 0).toString()
                 << item->data(Qt::UserRole + 1).toString()
                 << item->data(Qt::UserRole + 2).toString()
                 << item->data(Qt::UserRole + 3).toString()
                 << item->data(Qt::UserRole + 4).toString();
        C5DataSyncThread *dst = new C5DataSyncThread(fDBParams, dbParams, i, this);
        connect(dst, SIGNAL(statusUploadUpdated(int,int,QString)), this, SLOT(statusUpdateUpload(int,int,QString)));
        connect(dst, SIGNAL(finished()), this, SLOT(dstFinished()));
        connect(dst, SIGNAL(progressBarMax(int)), this, SLOT(progressBarMax(int)));
        connect(dst, SIGNAL(progressBarValue(int)), this, SLOT(progressBarValue(int)));
        dst->runUploadData();
    }
}

void C5DataSynchronize::getSyncTables()
{
    fRemovedSyncTables.clear();
    ui->tblSyncTables->clearContents();
    ui->tblSyncTables->setRowCount(0);
    C5Database db(fDBParams);
    db.exec("select f_id, f_name from s_sync_tables");
    while (db.nextRow()) {
        int row = ui->tblSyncTables->addEmptyRow();
        ui->tblSyncTables->setString(row, 0, db.getString(0));
        ui->tblSyncTables->setString(row, 1, db.getString(1));
    }
}

void C5DataSynchronize::on_tblSyncTables_customContextMenuRequested(const QPoint &pos)
{
    QModelIndexList ml = ui->tblSyncTables->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Delete"), this, SLOT(removeSyncTable()));
    menu->popup(ui->tblSyncTables->mapToGlobal(pos));
}

void C5DataSynchronize::on_btnSaveSyncTables_2_clicked()
{
    getSyncTables();
}

void C5DataSynchronize::on_btnSaveSyncTables_clicked()
{
    C5Database dbmain(fDBParams);
    foreach (QString s, fRemovedSyncTables) {
        dbmain[":f_name"] = s;
        dbmain.exec("delete from s_sync_tables where f_name=:f_name");
    }
    for (int i = 0; i < ui->tblSyncTables->rowCount(); i++) {
        if (ui->tblSyncTables->getInteger(i, 0) == 0) {
            dbmain[":f_name"] = ui->tblSyncTables->getString(i, 1);
            ui->tblSyncTables->setInteger(i, 0, dbmain.insert("s_sync_tables"));
            dbmain.execDirect(QString("alter table %1 add column a timestamp").arg(ui->tblSyncTables->getString(i, 1)));
            dbmain.execDirect(QString("drop trigger if exists %1_before_insert").arg(ui->tblSyncTables->getString(i, 1)));
            dbmain.execDirect(QString("drop trigger if exists %1_before_update").arg(ui->tblSyncTables->getString(i, 1)));
            dbmain.execDirect(QString("CREATE  TRIGGER `%1_before_insert` \
                BEFORE INSERT ON `%1`  \
                FOR EACH ROW \
                BEGIN \
                set NEW.a = current_timestamp(); \
                END").arg(ui->tblSyncTables->getString(i, 1)));
            dbmain.execDirect(QString("CREATE  TRIGGER `%1_before_update` \
                BEFORE update ON `%1` \
                FOR EACH ROW  \
                BEGIN \
                set NEW.a = current_timestamp(); \
                END ").arg(ui->tblSyncTables->getString(i, 1)));
        }
    }
    for (int i = 0; i < ui->lstDatabases->count(); i++) {
        QStringList dbParams;
        QListWidgetItem *item = ui->lstDatabases->item(i);
        dbParams << item->data(Qt::UserRole + 0).toString()
                 << item->data(Qt::UserRole + 1).toString()
                 << item->data(Qt::UserRole + 2).toString()
                 << item->data(Qt::UserRole + 3).toString()
                 << item->data(Qt::UserRole + 4).toString();
        C5Database db(dbParams);
        foreach (QString s, fRemovedSyncTables) {
            db.exec(QString("drop trigger if exists %1_before_insert").arg(s));
            db.exec(QString("drop trigger if exists %1_before_update").arg(s));
        }
    }
    fRemovedSyncTables.clear();
    C5Message::info(tr("Saved"));
}

void C5DataSynchronize::on_leAddSyncTable_returnPressed()
{
    if (ui->leAddSyncTable->isEmpty()) {
        return;
    }
    int row = ui->tblSyncTables->addEmptyRow();
    ui->tblSyncTables->setString(row, 1, ui->leAddSyncTable->text());
    ui->leAddSyncTable->clear();
}

void C5DataSynchronize::on_btnAddSyncTable_clicked()
{
    on_leAddSyncTable_returnPressed();
}

void C5DataSynchronize::on_tabWidget_currentChanged(int index)
{
    switch (index) {
    case 3:
        if (!pr(fDBParams.at(1), cp_t7_upload_date_settings)) {
            ui->tabWidget->setCurrentIndex(0);
            C5Message::error(tr("Access denied"));
            return;
        }
        getSyncTables();
        break;
    }
}
