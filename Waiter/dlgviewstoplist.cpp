#include "dlgviewstoplist.h"
#include "ui_dlgviewstoplist.h"
#include "stoplist.h"
#include "datadriver.h"
#include "dlgpassword.h"

DlgViewStopList::DlgViewStopList() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgViewStopList)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    ui->tbl->setColumnCount(width() / 200);
    auto *sh = createSocketHandler(SLOT(handleStopList(QJsonObject)));
    sh->bind("cmd", sm_stoplist);
    sh->bind("state", sl_get);
    sh->send();
}

DlgViewStopList::~DlgViewStopList()
{
    delete ui;
}

void DlgViewStopList::handleStopList(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    QJsonArray ja = obj["list"].toArray();
    int r = 0, c = 0;
    for (int i = 0; i < ja.count(); i++) {
        if (r > ui->tbl->rowCount() - 1) {
            ui->tbl->addEmptyRow();
        }
        QJsonObject o = ja.at(i).toObject();
        C5TableWidgetItem *item = new C5TableWidgetItem(QString("[%1] %2").arg(o["qty"].toDouble()).arg(dbdish->name(o["dish"].toInt())));
        item->setData(Qt::UserRole, o["dish"].toInt());
        ui->tbl->setItem(r, c, item);
        if (++c > ui->tbl->columnCount() - 1) {
            c = 0;
            r++;
        }
    }
}

void DlgViewStopList::handleStopListQty(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    qDebug() << obj;
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            C5TableWidgetItem *item = ui->tbl->item(r, c);
            if (item) {
                if (item->data(Qt::UserRole).toInt() == obj["f_dish"].toString().toInt()) {
                    item->setText(QString("[%1] %2").arg(obj["f_qty"].toDouble()).arg(dbdish->name(obj["f_dish"].toString().toInt())));
                    return;
                }
            }
        }
    }
}

void DlgViewStopList::handleStopListRemoveOne(const QJsonObject &obj)
{
    sender()->deleteLater();
    if (obj["reply"].toInt() == 0) {
        C5Message::error(obj["msg"].toString());
        return;
    }
    qDebug() << obj;
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            C5TableWidgetItem *item = ui->tbl->item(r, c);
            if (item) {
                if (item->data(Qt::UserRole).toInt() == obj["dish"].toInt()) {
                    item->setText("");
                    item->setData(Qt::UserRole, 0);
                    ui->tbl->setItem(c, r, item);
                    return;
                }
            }
        }
    }
}

void DlgViewStopList::on_btnBack_clicked()
{
    reject();
}

void DlgViewStopList::on_btnChangeQty_clicked()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    int dishid = ui->tbl->item(ml.at(0).row(), ml.at(0).column())->data(Qt::UserRole).toInt();
    if (dishid == 0) {
        return;
    }
    double max = 999;
    if (!DlgPassword::getAmount(dbdish->name(dishid), max, false)) {
        return;
    }
    auto *sh = createSocketHandler(SLOT(handleStopListQty(QJsonObject)));
    sh->bind("cmd", sm_stoplist);
    sh->bind("f_dish", QString::number(dishid));
    sh->bind("f_stopqty", max);
    sh->bind("state", 1);
    sh->send();
}

void DlgViewStopList::on_btnRemoveFromStoplist_clicked()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    int dishid = ui->tbl->item(ml.at(0).row(), ml.at(0).column())->data(Qt::UserRole).toInt();
    if (dishid == 0) {
        return;
    }
    auto *sh = createSocketHandler(SLOT(handleStopListRemoveOne(QJsonObject)));
    sh->bind("cmd", sm_stoplist);
    sh->bind("dish", QString::number(dishid));
    sh->bind("state", sl_remove_one);
    sh->send();
}
