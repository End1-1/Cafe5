#include "dlgviewstoplist.h"
#include "ui_dlgviewstoplist.h"
#include "datadriver.h"
#include "dlgpassword.h"
#include "c5config.h"

DlgViewStopList::DlgViewStopList() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgViewStopList)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    ui->tbl->setColumnCount(width() / 200);
    fHttp->createHttpQuery("/engine/waiter/stoplist.php", QJsonObject{{"action", "get"}}, SLOT(getStopListResponse(
                QJsonObject)));
}

DlgViewStopList::~DlgViewStopList()
{
    delete ui;
}

void DlgViewStopList::getStopListResponse(const QJsonObject &jdoc)
{
    QJsonArray ja = jdoc["list"].toArray();
    int r = 0, c = 0;
    for (int i = 0; i < ja.count(); i++) {
        if (r > ui->tbl->rowCount() - 1) {
            ui->tbl->addEmptyRow();
        }
        QJsonObject o = ja.at(i).toObject();
        C5TableWidgetItem *item = new C5TableWidgetItem(QString("[%1] %2")
            .arg(o["f_qty"].toDouble())
            .arg(dbdish->name(
                     o["f_dish"].toInt())));
        item->setData(Qt::UserRole, o["f_dish"].toInt());
        ui->tbl->setItem(r, c, item);
        if (++c > ui->tbl->columnCount() - 1) {
            c = 0;
            r++;
        }
    }
    fHttp->httpQueryFinished(sender());
}

void DlgViewStopList::handleStopListQty(const QJsonObject &obj)
{
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            C5TableWidgetItem *item = ui->tbl->item(r, c);
            if (item) {
                if (item->data(Qt::UserRole).toInt() == obj["f_dish"].toString().toInt()) {
                    item->setText(QString("[%1] %2").arg(obj["f_qty"].toDouble()).arg(dbdish->name(obj["f_dish"].toString().toInt())));
                    goto label;
                }
            }
        }
    }
label:
    fHttp->httpQueryFinished(sender());
}

void DlgViewStopList::handleStopListRemoveOne(const QJsonObject &obj)
{
    fHttp->httpQueryFinished(sender());
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        for (int c = 0; c < ui->tbl->columnCount(); c++) {
            C5TableWidgetItem *item = ui->tbl->item(r, c);
            if (item) {
                if (item->data(Qt::UserRole).toInt() == obj["f_dish"].toInt()) {
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
    fHttp->createHttpQuery("/engine/waiter/stoplist.php",
    QJsonObject{{"action", "set"}, {"f_dish", dishid}, {"f_qty", max}},
    SLOT(handleStopListQty(QJsonObject)));
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
    fHttp->createHttpQuery("/engine/waiter/stoplist.php",
    QJsonObject{{"action", "removedish"}, {"f_dish", dishid}},
    SLOT(handleStopListRemoveOne(QJsonObject)));
}
