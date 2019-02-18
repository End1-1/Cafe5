#include "c5datasynchronize.h"
#include "ui_c5datasynchronize.h"
#include "c5datasyncthread.h"
#include <QMutex>

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
    }
}

C5DataSynchronize::~C5DataSynchronize()
{
    delete ui;
}

void C5DataSynchronize::on_btnStartUpload_clicked()
{
    ui->btnStartUpload->setEnabled(false);
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
        dst->runWithDate(ui->deStart->date(), ui->deEnd->date());
    }
}

void C5DataSynchronize::statusUpdate(const QString &name, int row, const QString &msg)
{
    QListWidgetItem *item = ui->lstDatabases->item(row);
    if (!item) {
        return;
    }
    item->setText(name + ": " + msg);
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
