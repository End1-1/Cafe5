#include "c5goodsprice.h"
#include "ui_c5goodsprice.h"
#include "ninterface.h"
#include "jsons.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5lineedit.h"
#include <QInputDialog>
#include <QMenu>
#include <QJsonArray>
#include <c5daterange.h>

const quint32 col1_saleqty = 2;
const quint32 col1_storeqty = 3;
const quint32 col1_percent = 4;
const quint32 col1_orretail = 5;
const quint32 col1_orwhosale = 6;
const quint32 col1_retail = 7;
const quint32 col1_whosale = 8;

const quint32 col2_retail = 3;
const quint32 col2_whosale = 4;

C5GoodsPriceOrder::C5GoodsPriceOrder(const QStringList &dbParams)
    : C5Widget(dbParams)
    , ui(new Ui::C5GoodsPriceOrder)
{
    ui->setupUi(this);
    fIcon = ":/pricing.png";
    fLabel = tr("Group discount");
    fHttp->createHttpQuery("/engine/goods/create-group-discount.php", QJsonObject{{"action", "getAll"}}, SLOT(
        getAllResponse(QJsonObject)));
    ui->tblGoods->setVisible(false);
    connect(ui->tblGoods, &C5TableWidget::customContextMenuRequested, this, &C5GoodsPriceOrder::tblGoodsContextMenu);
    fDate1 = QDate::currentDate();
    fDate2 = QDate::currentDate();
    ui->lbDateRange->setText(QString("%1 - %2").arg(fDate1.toString(FORMAT_DATE_TO_STR),
                             fDate2.toString(FORMAT_DATE_TO_STR)));
}

C5GoodsPriceOrder::~C5GoodsPriceOrder()
{
    delete ui;
}

void C5GoodsPriceOrder::createGropuDiscountResponse(const QJsonObject &jdoc)
{
    QListWidgetItem *item = new QListWidgetItem(ui->lwNames);
    item->setText(jdoc["name"].toString());
    item->setData(Qt::UserRole, jdoc["id"].toInt());
    ui->lwNames->addItem(item);
    ui->lwNames->setCurrentItem(item);
    fHttp->httpQueryFinished(sender());
}

void C5GoodsPriceOrder::getAllResponse(const QJsonObject &jdoc)
{
    QJsonArray ja = jdoc["all"].toArray();
    for (int i = 0; i < ja.size(); i++) {
        const QJsonObject &j = ja.at(i).toObject();
        auto *item = new QListWidgetItem(ui->lwNames);
        item->setText(j["f_name"].toString());
        item->setData(Qt::UserRole, j["f_id"].toInt());
        item->setData(Qt::UserRole + 1, j["f_body"].toString());
        ui->lwNames->addItem(item);
    }
    if (ui->lwNames->count() > 0) {
        ui->lwNames->setCurrentRow(0);
        if (ui->tblDoc->rowCount() > 0) {
            setCurrentGroup(ui->tblDoc->getInteger(0, 0));
            ui->tblDoc->setCurrentCell(0, 0);
        }
    }
    fHttp->httpQueryFinished(sender());
}

void C5GoodsPriceOrder::addGroupResponse(const QJsonObject &jdoc)
{
    QListWidgetItem *item = nullptr;
    for (int i = 0; i < ui->lwNames->count(); i++) {
        item = ui->lwNames->item(i);
        if (item->data(Qt::UserRole).toInt() == jdoc["itemid"].toInt()) {
            break;
        }
    }
    if (!item) {
        return;
    }
    QString jstr = item->data(Qt::UserRole + 1).toString();
    QJsonObject jo = __strjson(jstr);
    QJsonArray jgroup = jo["groups"].toArray();
    for (int i = 0; i < jgroup.size(); i++) {
        const QJsonObject &j = jgroup.at(i).toObject();
        if (j["f_id"].toInt() == jdoc["group"].toInt()) {
            C5Message::error(tr("Group already added"));
            return;
        }
    }
    jgroup.append(QJsonObject{
        {"f_id", jdoc["group"].toInt()},
        {"retail", jdoc["retail"].toDouble()},
        {"whosale", jdoc["whosale"].toDouble()},
        {"f_name", jdoc["groupname"].toString()},
        {"items", jdoc["items"].toArray()}});
    jo["groups"] = jgroup;
    item->setData(Qt::UserRole + 1, __jsonstr(jo));
    int row = newRow();
    ui->tblDoc->setData(row, 0, jdoc["group"].toInt());
    ui->tblDoc->setData(row, 1, jdoc["groupname"].toString());
    ui->tblDoc->setData(row, col1_orretail, jdoc["retail"].toDouble());
    ui->tblDoc->setData(row, col1_orwhosale, jdoc["whosale"].toDouble());
    ui->tblDoc->setCurrentCell(row, 0);
    //setCurrentGroup(jdoc["group"].toInt());
    fHttp->httpQueryFinished(sender());
}

void C5GoodsPriceOrder::saveResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    C5Message::info(tr("Saved"));
}

void C5GoodsPriceOrder::discountResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
    C5Message::info(tr("Saved"));
}

void C5GoodsPriceOrder::refreshSaleStoreResponse(const QJsonObject &jdoc)
{
    for (int i = 0; i < ui->tblDoc->rowCount(); i++) {
        ui->tblDoc->setDouble(i, col1_saleqty, 0);
        ui->tblDoc->setDouble(i, col1_storeqty, 0);
    }
    QJsonArray jst = jdoc["store"].toArray();
    QJsonArray jsa = jdoc["sale"].toArray();
    for (int i = 0; i < jsa.size(); i++) {
        QJsonArray ja = jsa.at(i).toArray();
        for (int j = 0; j < ui->tblDoc->rowCount(); j++) {
            if (ui->tblDoc->getInteger(j, 0) == ja[0].toInt()) {
                ui->tblDoc->setDouble(j, col1_saleqty, ja[1].toDouble());
            }
        }
    }
    for (int i = 0; i < jst.size(); i++) {
        QJsonArray ja = jst.at(i).toArray();
        for (int j = 0; j < ui->tblDoc->rowCount(); j++) {
            if (ui->tblDoc->getInteger(j, 0) == ja[0].toInt()) {
                ui->tblDoc->setDouble(j, col1_storeqty, ja[1].toDouble());
            }
        }
    }
    fHttp->httpQueryFinished(sender());
}

void C5GoodsPriceOrder::tblGoodsContextMenu(const QPoint &p)
{
    auto *m = new QMenu(this);
    m->addAction(ui->actionChangeGoodsPrice);
    m->addAction(ui->actionSet_whosale_price_of_selected_goods);
    m->show();
    m->popup(ui->tblGoods->mapToGlobal(p));
}

void C5GoodsPriceOrder::on_btnAddNew_clicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Name"), tr("Name"), QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) {
        return;
    }
    fHttp->createHttpQuery("/engine/goods/create-group-discount.php",
    QJsonObject{{"name", name}, {"action", "createGroupDiscount"}}, SLOT(createGropuDiscountResponse(QJsonObject)));
}

void C5GoodsPriceOrder::on_btnNewGroup_clicked()
{
    int ci = ui->lwNames->currentRow();
    if (ci < 0) {
        return;
    }
    auto *item = ui->lwNames->item(ci);
    int itemid = item->data(Qt::UserRole).toInt();
    QList<QVariant> vals;
    if (!C5Selector::getValueOfColumn(fDBParams, cache_goods_group, vals, 2)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }
    fHttp->createHttpQuery("/engine/goods/create-group-discount.php",
    QJsonObject{{"action", "getGroupItems"},
        {"item", itemid},
        {"group", vals.at(1).toInt()},
        {"groupname", vals.at(2).toString() }},
    SLOT(addGroupResponse(QJsonObject)));
}

int C5GoodsPriceOrder::newRow()
{
    int row = ui->tblDoc->rowCount();
    ui->tblDoc->addEmptyRow();
    ui->tblDoc->setData(row, 0, 0);
    ui->tblDoc->setData(row, 1, "");
    auto *l = ui->tblDoc->createLineEdit(row, col1_retail);
    l->setValidator(new QDoubleValidator(0, 999999999, 0));
    connect(l, &C5LineEdit::textChanged, [this, row](QString s) {
        setGroupPriceJson(row, 1, s.toDouble());
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            ui->tblGoods->lineEdit(i, 3)->setText(s);
        }
    });
    l = ui->tblDoc->createLineEdit(row, col1_whosale);
    l->setValidator(new QDoubleValidator(0, 999999999, 0));
    connect(l, &C5LineEdit::textChanged, [this, row](QString s) {
        setGroupPriceJson(row, 2, s.toDouble());
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            ui->tblGoods->lineEdit(i, 4)->setText(s);
        }
    });
    l = ui->tblDoc->createLineEdit(row, col1_percent);
    l->setValidator(new QDoubleValidator(0, 2, 0));
    connect(l, &C5LineEdit::textChanged, [this, row](QString s) {
        ui->tblDoc->lineEdit(row, col1_retail)
        ->setDouble(ui->tblDoc->getDouble(row, col1_orretail)  - (ui->tblDoc->getDouble(row,
                    col1_orretail) * (s.toDouble() / 100)));
        ui->tblDoc->lineEdit(row, col1_whosale)
        ->setDouble(ui->tblDoc->getDouble(row, col1_orwhosale)  - (ui->tblDoc->getDouble(row,
                    col1_orwhosale) * (s.toDouble() / 100)));
        // setGroupPriceJson(row, 2, s.toDouble());
        // for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        //     ui->tblGoods->lineEdit(i, 4)->setText(s);
        // }
    });
    return row;
}

int C5GoodsPriceOrder::newGoodsRow()
{
    int row = ui->tblGoods->rowCount();
    ui->tblGoods->addEmptyRow();
    ui->tblGoods->setData(row, 0, 0);
    ui->tblGoods->setData(row, 1, "");
    ui->tblGoods->setData(row, 2, "");
    auto *l = ui->tblGoods->createLineEdit(row, 3);
    l->setValidator(new QDoubleValidator(0, 999999999, 0));
    l = ui->tblGoods->createLineEdit(row, 4);
    l->setValidator(new QDoubleValidator(0, 999999999, 0));
    return row;
}

void C5GoodsPriceOrder::setCurrentGroup(int group)
{
    int ci = ui->lwNames->currentRow();
    if (ci < 0) {
        return;
    }
    auto *item = ui->lwNames->item(ci);
    QString jstr = item->data(Qt::UserRole + 1).toString();
    QJsonObject jo = __strjson(jstr);
    QJsonArray jgroup = jo["groups"].toArray();
    QJsonObject gr;
    for (int i = 0; i < jgroup.size(); i++) {
        const QJsonObject &j = jgroup.at(i).toObject();
        if (j["f_id"].toInt() == group) {
            gr = j;
        }
    }
    if (gr.isEmpty()) {
        return;
    }
    ui->tblGoods->setRowCount(0);
    QJsonArray ja = gr["items"].toArray();
    for (int i = 0; i < ja.size(); i++) {
        const QJsonObject &j = ja.at(i).toObject();
        int r = newGoodsRow();
        ui->tblGoods->setData(r, 0, j["f_id"].toInt());
        ui->tblGoods->setData(r, 1, j["f_name"].toString());
        ui->tblGoods->setData(r, 2, j["f_scancode"].toString());
        ui->tblGoods->lineEdit(r, col2_retail)->setDouble(j["f_price1disc"].toDouble());
        ui->tblGoods->lineEdit(r, col2_whosale)->setDouble(j["f_price2disc"].toDouble());
    }
}

void C5GoodsPriceOrder::getSaledQty()
{
    QString goodsGroups;
    for (int i = 0; i < ui->tblDoc->rowCount(); i++) {
        if (!goodsGroups.isEmpty()) {
            goodsGroups += ",";
        }
        goodsGroups += QString::number(ui->tblDoc->getInteger(i, 0));
    }
    if (goodsGroups.isEmpty()) {
        C5Message::info(tr("Select group of goods"));
        return;
    }
    fHttp->createHttpQuery("/engine/goods/create-group-discount.php",
    QJsonObject{{"action", "refreshSaleStore"},
        {"groups", goodsGroups},
        {"date1", fDate1.toString("yyyy-MM-dd")},
        {"date2", fDate2.toString("yyyy-MM-dd")}},
    SLOT(refreshSaleStoreResponse(QJsonObject)));
}

void C5GoodsPriceOrder::setGroupPriceJson(int row, int col, double price)
{
    QListWidgetItem *item = ui->lwNames->currentItem();
    if (!item) {
        return;
    }
    QString field = col == 1 ? "f_price1disc" : "f_price2disc";
    QJsonObject jo = __strjson(item->data(Qt::UserRole + 1).toString());
    QJsonArray ja = jo["groups"].toArray();
    for (int i = 0; i < ja.size(); i++) {
        QJsonObject j = ja.at(i).toObject();
        if (ui->tblDoc->getInteger(row, 0) == j["f_id"].toInt()) {
            j["f_percent"] = ui->tblDoc->lineEdit(row, col1_percent)->getDouble();
            j["f_price1disc"] = ui->tblDoc->lineEdit(row, col1_retail)->getDouble();
            j["f_price2disc"] = ui->tblDoc->lineEdit(row, col1_whosale)->getDouble();;
            ja[i] = j;
            break;
        }
    }
    jo["groups"] = ja;
    item->setData(Qt::UserRole + 1, __jsonstr(jo));
}

void C5GoodsPriceOrder::on_btnSave_clicked()
{
    QListWidgetItem *item = ui->lwNames->currentItem();
    if (!item) {
        return;
    }
    ui->tblDoc->setCurrentCell(-1, -1);
    fHttp->createHttpQuery("/engine/goods/create-group-discount.php",
    QJsonObject{{"action", "save"},
        {"itemid", item->data(Qt::UserRole).toInt()},
        {"name", item->text()},
        {"body", item->data(Qt::UserRole + 1).toString() }},
    SLOT(saveResponse(QJsonObject)));
}

void C5GoodsPriceOrder::on_lwNames_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    ui->tblDoc->setRowCount(0);
    ui->tblGoods->setRowCount(0);
    if (current != nullptr) {
        QJsonObject jo = __strjson(current->data(Qt::UserRole + 1).toString());
        QJsonArray jg = jo["groups"].toArray();
        for (int i = 0; i < jg.size(); i++) {
            const QJsonObject &j = jg.at(i).toObject();
            int r = newRow();
            ui->tblDoc->setData(r, 0, j["f_id"].toInt());
            ui->tblDoc->setData(r, 1, j["f_name"].toString());
            ui->tblDoc->lineEdit(r, col1_percent)->setDouble(j["f_percent"].toDouble());
            ui->tblDoc->setDouble(r, col1_orretail, j["retail"].toDouble());
            ui->tblDoc->setDouble(r, col1_orwhosale, j["whosale"].toDouble());
            ui->tblDoc->lineEdit(r, col1_retail)->setDouble(j["f_price1disc"].toDouble());
            ui->tblDoc->lineEdit(r, col1_whosale)->setDouble(j["f_price2disc"].toDouble());
        }
    }
}

void C5GoodsPriceOrder::on_btnShowGoods_clicked()
{
    ui->tblGoods->setVisible(!ui->tblGoods->isVisible());
}
void C5GoodsPriceOrder::on_tblDoc_currentCellChanged(int currentRow, int currentColumn, int previousRow,
        int previousColumn)
{
    Q_UNUSED(currentColumn);
    if (previousRow > -1) {
        int prevgroup = ui->tblDoc->item(previousRow, 0)->data(Qt::EditRole).toInt();
        auto *item = ui->lwNames->currentItem();
        if (item) {
            QString jsonstr = item->data(Qt::UserRole + 1).toString();
            QJsonObject jo = __strjson(jsonstr);
            QJsonArray jgroup = jo["groups"].toArray();
            for (int i = 0; i < jgroup.size(); i++) {
                QJsonObject jg = jgroup.at(i).toObject();
                if (jg["f_id"].toInt() == prevgroup) {
                    QJsonArray jitems;
                    for (int j = 0; j < ui->tblGoods->rowCount(); j++) {
                        QJsonObject ji;
                        ji["f_id"] = ui->tblGoods->getInteger(j, 0);
                        ji["f_name"] = ui->tblGoods->getString(j, 1);
                        ji["f_scancode"] = ui->tblGoods->getString(j, 2);
                        ji["f_price1disc"] = ui->tblGoods->lineEdit(j, col2_retail)->getDouble();
                        ji["f_price2disc"] = ui->tblGoods->lineEdit(j, col2_whosale)->getDouble();
                        jitems.append(ji);
                    }
                    jg["items"] = jitems;
                    jgroup[i] = jg;
                    jo["groups"] = jgroup;
                    item->setData(Qt::UserRole + 1, __jsonstr(jo));
                    break;
                }
            }
        }
        setCurrentGroup(ui->tblDoc->getInteger(currentRow, 0));
    }
}

void C5GoodsPriceOrder::on_btnRollback_clicked()
{
    if (C5Message::question(tr("Confirm")) != QDialog::Accepted) {
        return;
    }
    QListWidgetItem *item = ui->lwNames->currentItem();
    if (!item) {
        return;
    }
    ui->tblDoc->setCurrentCell(-1, -1);
    fHttp->createHttpQuery("/engine/goods/create-group-discount.php",
    QJsonObject{{"action", "rollback"},
        {"itemid", item->data(Qt::UserRole).toInt()},
        {"name", item->text()},
        {"body", item->data(Qt::UserRole + 1).toString() }},
    SLOT(discountResponse(QJsonObject)));
}

void C5GoodsPriceOrder::on_tblDoc_customContextMenuRequested(const QPoint &pos)
{
    auto *m = new QMenu(this);
    m->addAction(ui->actionRemove);
    m->show();
    m->popup(ui->tblDoc->mapToGlobal(pos));
}

void C5GoodsPriceOrder::on_actionRemove_triggered()
{
    int row = ui->tblDoc->currentRow();
    if (row < 0) {
        return;
    }
    int groupid = ui->tblDoc->getInteger(row, 0);
    ui->tblDoc->removeRow(row);
    auto *item = ui->lwNames->currentItem();
    QJsonObject jo = __strjson(item->data(Qt::UserRole + 1).toString());
    QJsonArray jgroup = jo["groups"].toArray();
    for (int i = 0; i < jgroup.size(); i++) {
        const QJsonObject &jg = jgroup.at(i).toObject();
        if (jg["f_id"].toInt() == groupid) {
            jgroup.removeAt(i);
            break;
        }
    }
    jo["groups"] = jgroup;
    item->setData(Qt::UserRole + 1, __jsonstr(jo));
}

void C5GoodsPriceOrder::on_btnApply_clicked()
{
    if (C5Message::question(tr("Confirm")) != QDialog::Accepted) {
        return;
    }
    QListWidgetItem *item = ui->lwNames->currentItem();
    if (!item) {
        return;
    }
    ui->tblDoc->setCurrentCell(-1, -1);
    fHttp->createHttpQuery("/engine/goods/create-group-discount.php",
    QJsonObject{{"action", "discount"},
        {"itemid", item->data(Qt::UserRole).toInt()},
        {"name", item->text()},
        {"body", item->data(Qt::UserRole + 1).toString() }},
    SLOT(discountResponse(QJsonObject)));
}

void C5GoodsPriceOrder::on_actionChangeGoodsPrice_triggered()
{
    QModelIndexList ml = ui->tblGoods->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    bool ok;
    double v = QInputDialog::getDouble(this, tr("Price"), tr("Retail"), 0, 0, 999999999, 0, &ok);
    if (!ok) {
        return;
    }
    for (const QModelIndex &m : ml) {
        ui->tblGoods->lineEdit(m.row(), 3)->setDouble(v);
    }
}

void C5GoodsPriceOrder::on_actionSet_whosale_price_of_selected_goods_triggered()
{
    QModelIndexList ml = ui->tblGoods->selectionModel()->selectedIndexes();
    if (ml.count() == 0) {
        return;
    }
    bool ok;
    double v = QInputDialog::getDouble(this, tr("Price"), tr("Whosale"), 0, 0, 999999999, 0, &ok);
    if (!ok) {
        return;
    }
    for (const QModelIndex &m : ml) {
        ui->tblGoods->lineEdit(m.row(), 4)->setDouble(v);
    }
}

void C5GoodsPriceOrder::on_btnChangeRange_clicked()
{
    QDate d1 = fDate1, d2 = fDate2;
    if (!C5DateRange::dateRange(d1, d2))  {
        return;
    }
    fDate1 = d1;
    fDate2 = d2;
    ui->lbDateRange->setText(QString("%1 - %2").arg(fDate1.toString(FORMAT_DATE_TO_STR),
                             fDate2.toString(FORMAT_DATE_TO_STR)));
    getSaledQty();
}

void C5GoodsPriceOrder::on_btnRefresh_clicked()
{
    getSaledQty();
}