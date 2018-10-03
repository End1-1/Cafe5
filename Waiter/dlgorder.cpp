#include "dlgorder.h"
#include "ui_dlgorder.h"
#include "c5user.h"
#include "c5witerconf.h"
#include "c5menu.h"
#include "c5order.h"
#include "c5dishtabledelegate.h"
#include "c5ordertabledelegate.h"
#include "dlgpassword.h"
#include <QCloseEvent>

#define PART2_COL_WIDTH 150
#define PART2_ROW_HEIGHT 40
#define PART3_ROW_HEIGHT 80
#define PART4_ROW_HEIGHT 80

DlgOrder::DlgOrder(QWidget *parent) :
    C5Dialog(parent),
    ui(new Ui::DlgOrder)
{
    ui->setupUi(this);
    fOrder = new C5Order();
}

DlgOrder::~DlgOrder()
{
    delete ui;
    delete fOrder;
}

void DlgOrder::openTable(int table, C5User *user)
{
    DlgOrder *d = new DlgOrder(C5Config::fParentWidget);
    d->fMenuName = C5WaiterConf::fDefaultMenu;
    d->showFullScreen();
    qApp->processEvents();
    d->ui->tblPart2->horizontalHeader()->setDefaultSectionSize(PART2_COL_WIDTH);
    int colCount = d->ui->tblPart2->width() / PART2_COL_WIDTH;
    d->ui->tblPart2->setColumnCount(colCount);
    d->ui->tblPart2->verticalHeader()->setDefaultSectionSize(PART2_ROW_HEIGHT);
    d->ui->tblDishes->setItemDelegate(new C5DishTableDelegate());
    d->ui->tblDishes->horizontalHeader()->setDefaultSectionSize(PART2_COL_WIDTH);
    d->ui->tblDishes->verticalHeader()->setDefaultSectionSize(PART3_ROW_HEIGHT);
    colCount = d->ui->tblDishes->width() / PART2_COL_WIDTH;
    d->ui->tblDishes->setColumnCount(colCount);
    d->ui->tblOrder->setItemDelegate(new C5OrderTableDelegate());
    d->ui->tblOrder->setColumnWidth(0, d->ui->tblOrder->width() - 2);
    d->ui->tblOrder->verticalHeader()->setDefaultSectionSize(PART4_ROW_HEIGHT);
    d->buildMenu(d->fMenuName, "", "");
    d->fUser = user;
    d->load(table);
    d->exec();
    delete d;
}

void DlgOrder::accept()
{
    C5SocketHandler *sh = createSocketHandler(SLOT(saveAndQuit(QJsonObject)));
    fOrder->setHeaderValue("unlocktable", 1);
    fOrder->save(sh);
}

void DlgOrder::reject()
{
    accept();
}

void DlgOrder::load(int table)
{
    C5SocketHandler *sh = createSocketHandler(SLOT(handleOpenTable(QJsonObject)));
    sh->bind("cmd", sm_opentable);
    sh->bind("table", table);
    sh->bind("host", hostinfo);
    sh->send();
}

void DlgOrder::buildMenu(const QString &menu, QString part1, QString part2)
{
    Q_UNUSED(part1)
    Q_UNUSED(part2)
    QStringList p1 = C5Menu::fMenu[menu].keys();
    int col = 0, row = 0;
    ui->tblPart1->clear();
    foreach (QString s, p1) {
        ui->tblPart1->setItem(row, col++, new QTableWidgetItem(s));
        if (col == 2) {
            col = 0;
            row ++;
        }
    }

    if (part1.isEmpty()) {
        part1 = ui->tblPart1->item(0, 0)->text();
    }

    ui->tblPart2->clear();
    ui->tblPart2->setRowCount(0);
    QStringList p2 = C5Menu::fMenu[menu][part1].keys();
    int colCount = ui->tblPart2->columnCount();
    col = 0;
    row = 0;
    foreach (QString s, p2) {
        if (row > ui->tblPart2->rowCount() - 1) {
            ui->tblPart2->setRowCount(row + 1);
        }
        ui->tblPart2->setItem(row, col++, new QTableWidgetItem(s));
        if (col == colCount) {
            col = 0;
            row ++;
        }
    }

    ui->tblDishes->clear();
    ui->tblDishes->setRowCount(0);
    if (part2.isEmpty()) {
        return;
    }
    colCount = ui->tblDishes->columnCount();
    col = 0;
    row = 0;
    QList<QJsonObject> dishes = C5Menu::fMenu[menu][part1][part2];
    for (int i = 0; i < dishes.count(); i++) {
        QJsonObject &o = dishes[i];
        QTableWidgetItem *item = new QTableWidgetItem(o["f_name"].toString());
        item->setData(Qt::UserRole, o);
        if (row > ui->tblDishes->rowCount() - 1) {
            ui->tblDishes->setRowCount(row + 1);
        }
        ui->tblDishes->setItem(row, col++, item);
        if (col == colCount) {
            col = 0;
            row ++;
        }
    }
}

void DlgOrder::addDishToOrder(const QJsonObject &obj)
{
    QJsonObject o = obj;
<<<<<<< HEAD
    if (obj["f_id"].toString().toInt() == 0) {
        o["f_id"] = "0";
        o["f_header"] = fOrder->headerValue("f_id");
        o["f_state"] = QString::number(DISH_STATE_OK);
        o["f_service"] = "0";
        o["f_discount"] = "0";
        o["f_total"] = o["f_price"];
        o["f_qty1"] = "1";
        o["f_qty2"] = "0";
        o["f_comment"] = "";
    }
=======
    o["f_id"] = "0";
    o["f_state"] = QString::number(DISH_STATE_OK);
    o["f_service"] = "0";
    o["f_discount"] = "0";
    o["f_total"] = o["f_price"];
    o["f_qty1"] = "1";
    o["f_qty2"] = "0";
    o["f_comment"] = "";
>>>>>>> 838f31771d5f7dd82bf2f9d4a1b63c78fc2269eb
    int row = ui->tblOrder->rowCount();
    ui->tblOrder->setRowCount(row + 1);
    ui->tblOrder->setItem(row, 0, new QTableWidgetItem());
    ui->tblOrder->item(row, 0)->setData(Qt::UserRole, o);
    ui->tblOrder->setCurrentCell(ui->tblOrder->rowCount() - 1, 0);
    fOrder->addItem(o);
}

void DlgOrder::loadOrder(const QJsonObject &obj)
{
<<<<<<< HEAD
=======
    fOrder->fItems = obj["body"].toArray();
>>>>>>> 838f31771d5f7dd82bf2f9d4a1b63c78fc2269eb
    fOrder->fHeader = obj["header"].toArray().at(0).toObject();
    if (fOrder->headerValue("f_id").toInt() == 0) {
        fOrder->setHeaderValue("f_staff", fUser->fId);
        fOrder->setHeaderValue("f_staffname", fUser->fFull);
        fOrder->setHeaderValue("f_state", ORDER_STATE_OPEN);
        fOrder->setHeaderValue("f_prefix", "");
<<<<<<< HEAD
        fOrder->setHeaderValue("f_comment", "");
        fOrder->setHeaderValue("f_hall", obj["table"].toArray().at(0)["f_hall"].toString());
    }
    ui->tblOrder->clear();
    ui->tblOrder->setRowCount(0);
    for (int i = 0, count = obj["body"].toArray().count(); i < count; i++) {
        addDishToOrder(obj["body"].toArray().at(i).toObject());
    }
=======
        fOrder->setHeaderValue("f_hall", obj["table"].toArray().at(0)["f_hall"].toString());
    }
>>>>>>> 838f31771d5f7dd82bf2f9d4a1b63c78fc2269eb
}

void DlgOrder::changeQty(double qty)
{
    QModelIndexList ml = ui->tblOrder->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    int index = ml.at(0).row();
    QJsonObject o = fOrder->fItems.at(index).toObject();
    if (qty > 0) {
        if (o["f_qty1"].toString().toDouble() + qty > 99) {
            return;
        }
        o["f_qty1"] = float_str(o["f_qty1"].toString().toDouble() + qty, 1);
    } else {
        if (o["f_qty1"].toString().toDouble() + qty < 0.4) {
            return;
        }
        if (o["f_qty2"].toString().toDouble() < o["f_qty1"].toString().toDouble() + qty) {
            o["f_qty1"] = float_str(o["f_qty1"].toString().toDouble() + qty, 1);
        }
    }
    fOrder->fItems[index] = o;
    ui->tblOrder->item(index, 0)->setData(Qt::UserRole, o);
    ui->tblOrder->viewport()->update();
}

void DlgOrder::handleOpenTable(const QJsonObject &obj)
{
    sender()->deleteLater();
    switch (obj["reply"].toInt()) {
    case 0:
    case 2: {
        C5Message::error(obj["msg"].toString());
        QDialog::accept();
        return;
    }
    case 1: {
        loadOrder(obj);
        break;
    }
    }
}

void DlgOrder::saveAndQuit(const QJsonObject &obj)
{
    qDebug() << obj;
    QDialog::accept();
}

void DlgOrder::handleError(int err, const QString &msg)
{
    switch (err) {

    }
    C5Message::error(msg);
}

void DlgOrder::on_tblPart1_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (item->text().isEmpty()) {
        return;
    }
    fPart1Name = item->text();
    buildMenu(fMenuName, fPart1Name, "");
}

void DlgOrder::on_tblPart2_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }

    QString part2 = item->text();
    if (part2.isEmpty()) {
        return;
    }
    buildMenu(fMenuName, fPart1Name, part2);
}

void DlgOrder::on_tblDishes_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (item->text().isEmpty()) {
        return;
    }
    const QJsonObject &o = item->data(Qt::UserRole).toJsonObject();
    addDishToOrder(o);
}

void DlgOrder::on_btnPlus1_clicked()
{
    changeQty(1);
}

void DlgOrder::on_btnMin1_clicked()
{
    changeQty(-1);
}

void DlgOrder::on_btnPlus05_clicked()
{
    changeQty(0.5);
}

void DlgOrder::on_btnMin05_clicked()
{
    changeQty(-0.5);
}

void DlgOrder::on_btnCustom_clicked()
{
    QModelIndexList ml = ui->tblOrder->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    int index = ml.at(0).row();
    QJsonObject o = fOrder->fItems.at(index).toObject();
    int qty;
    if (!DlgPassword::getQty(o["f_qty1"].toString(), qty)) {
        return;
    }
    if (qty > o["f_qty2"].toString().toDouble()) {
        o["f_qty1"] = QString::number(qty);
    }
    fOrder->fItems[index] = o;
    ui->tblOrder->item(index, 0)->setData(Qt::UserRole, o);
    ui->tblOrder->viewport()->update();
}
