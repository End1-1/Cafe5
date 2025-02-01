#include "c5goodsprice.h"
#include "ui_c5goodsprice.h"
#include "ninterface.h"
#include "jsons.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5lineedit.h"
#include "c5printing.h"
#include "c5printpreview.h"
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

const quint32 col2_retail_price = 3;
const quint32 col2_whosale_price = 4;
const quint32 col2_retail = 5;
const quint32 col2_whosale = 6;

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
    connect(ui->lwNames, &QListWidget::customContextMenuRequested, this, &C5GoodsPriceOrder::lwNamesContextMenu);
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
    ui->tblDoc->setData(row, 3, jdoc["qty"].toDouble());
    ui->tblDoc->setData(row, col1_orretail, jdoc["retail"].toDouble());
    ui->tblDoc->setData(row, col1_orwhosale, jdoc["whosale"].toDouble());
    ui->tblDoc->setCurrentCell(row, 0);
    //setCurrentGroup(jdoc["group"].toInt());
    fHttp->httpQueryFinished(sender());
}

void C5GoodsPriceOrder::editLwNameReponse(const QJsonObject &jdoc)
{
    for (int i = 0; i < ui->lwNames->count(); i++) {
        if (ui->lwNames->item(i)->data(Qt::UserRole).toInt() == jdoc["id"].toInt()) {
            ui->lwNames->item(i)->setText(jdoc["name"].toString());
            break;
        }
    }
    fHttp->httpQueryFinished(sender());
}

void C5GoodsPriceOrder::removeLwName(const QJsonObject &jdoc)
{
    for (int i = 0; i < ui->lwNames->count(); i++) {
        if (ui->lwNames->item(i)->data(Qt::UserRole).toInt() == jdoc["id"].toInt()) {
            delete ui->lwNames->item(i);
            break;
        }
    }
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

void C5GoodsPriceOrder::lwNamesContextMenu(const QPoint &p)
{
    auto *m = new QMenu(this);
    m->addAction(ui->actionEdit_name);
    m->addAction(ui->actionRemovelwName);
    m->popup(ui->lwNames->mapToGlobal(p));
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
    QVector<QJsonValue> vals;
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
        ui->tblDoc->lineEdit(row, col1_whosale)->setDouble(s.toDouble() - (s.toDouble() * 0.3));
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            ui->tblGoods->lineEdit(i, col2_whosale)->setText(s);
        }
    });
    l = ui->tblDoc->createLineEdit(row, col1_whosale);
    l->setValidator(new QDoubleValidator(0, 999999999, 0));
    connect(l, &C5LineEdit::textChanged, [this, row](QString s) {
        setGroupPriceJson(row, 2, s.toDouble());
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            ui->tblGoods->lineEdit(i, col2_retail)->setText(s);
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
    auto *l = ui->tblGoods->createLineEdit(row, col2_retail);
    l->setValidator(new QDoubleValidator(0, 999999999, 0));
    l = ui->tblGoods->createLineEdit(row, col2_whosale);
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
        ui->tblGoods->setData(r, col2_retail_price, j["f_price1"].toDouble());
        ui->tblGoods->setData(r, col2_whosale_price, j["f_price2"].toDouble());
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
            ui->tblDoc->setData(r, 3, j["qty"].toDouble());
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
                        ji["f_price1"] = ui->tblGoods->getDouble(j,  col2_retail_price);
                        ji["f_price2"] = ui->tblGoods->getDouble(j, col2_whosale_price);
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
    }
    setCurrentGroup(ui->tblDoc->getInteger(currentRow, 0));
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
        ui->tblGoods->lineEdit(m.row(), col2_whosale)->setDouble(v);
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
        ui->tblGoods->lineEdit(m.row(), col2_retail)->setDouble(v);
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

void C5GoodsPriceOrder::on_actionEdit_name_triggered()
{
    bool ok;
    QString newName = QInputDialog::getText(this, tr("New name"), "", QLineEdit::Normal, "", &ok).trimmed();
    if (!ok || newName.isEmpty()) {
        return;
    }
    QListWidgetItem *item = ui->lwNames->currentItem();
    if (!item) {
        return;
    }
    fHttp->createHttpQuery("/engine/goods/create-group-discount.php",
    QJsonObject{{"name", newName}, {"id", item->data(Qt::UserRole).toInt()}, {"action", "editLwName"}}, SLOT(
        editLwNameReponse(QJsonObject)));
}

void C5GoodsPriceOrder::on_actionRemovelwName_triggered()
{
    if (C5Message::question(tr("Confirm to remove")) != QDialog::Accepted) {
        return;
    }
    QListWidgetItem *item = ui->lwNames->currentItem();
    if (!item) {
        return;
    }
    fHttp->createHttpQuery("/engine/goods/create-group-discount.php",
    QJsonObject{{"id", item->data(Qt::UserRole).toInt()}, {"action", "removeLwName"}}, SLOT(
        removeLwName(QJsonObject)));
}

void C5GoodsPriceOrder::on_btnPrint_clicked()
{
    if (ui->lwNames->currentItem() == nullptr) {
        return;
    }
    QModelIndexList ml = ui->tblDoc->selectionModel()->selectedIndexes();
    if (ml.isEmpty()) {
        return;
    }
    QSet<int> rows;
    for (const QModelIndex &i : ml) {
        rows.insert(i.row());
    }
    C5Printing p;
    QFont f(qApp->font());
    p.setFont(f);
    p.setSceneParams(2000, 2700, QPageLayout::Portrait);
    p.setFontSize(30);
    p.ctext(tr("Discount document"));
    p.br();
    p.setFontSize(25);
    for (int row : rows) {
        p.setFontSize(25);
        p.setFontBold(true);
        p.ltext(QString("%1 / %2 / %3 - %4 (%5%) ")
                .arg(ui->lwNames->currentItem()->text(),
                     ui->tblDoc->getString(row, 1),
                     ui->tblDoc->lineEdit(row, col1_whosale)->text(),
                     ui->tblDoc->lineEdit(row, col1_retail)->text(),
                     ui->tblDoc->lineEdit(row, 4)->text()), 0);
        p.br();
        p.br();
        ui->tblDoc->setCurrentCell(row, 0);
        QList<qreal> points;
        QStringList vals;
        p.setFontSize(18);
        p.setFontBold(false);
        points << 10 << 400 << 200 << 200 << 200;
        vals << tr("Name") << tr("Barcode") << tr("Retail") << tr("Whosale");
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            if ( ui->tblGoods->lineEdit(i, col2_whosale)->text().toDouble()
                    - ui->tblDoc->lineEdit(row, col1_whosale)->text().toDouble() == 0
                    && ui->tblGoods->lineEdit(i, col2_retail)->text().toDouble()
                    - ui->tblDoc->lineEdit(row, col1_retail)->text().toDouble() == 0) {
                continue;
            }
            vals.clear();
            vals << ui->tblGoods->getString(i, 1)
                 << ui->tblGoods->getString(i, 2)
                 << ui->tblGoods->lineEdit(i, col2_whosale)->text()
                 << ui->tblGoods->lineEdit(i, col2_retail)->text();
            p.tableText(points, vals, p.fLineHeight + 20);
            p.br(p.fLineHeight + 20);
        }
    }
    QList<qreal> points;
    QStringList vals;
    p.setSceneParams(2000, 2700, QPageLayout::Portrait);
    p.setFontSize(25);
    p.setFontBold(true);
    QString docTypeText;
    /*
    p.br();
    p.ctext(ui->leReasonName->text());
    p.br();
    p.br();
    p.setFontSize(20);
    p.setFontBold(false);
    QString storeInName, storeOutName;
    if (ui->leStoreInput->getInteger() > 0) {
        storeInName = ui->leStoreInputName->text();
    }
    if (ui->leStoreOutput->getInteger() > 0) {
        storeOutName = ui->leStoreOutputName->text();
    }
    if (!ui->leComment->isEmpty()) {
        p.ltext(ui->leComment->text(), 50);
        p.br();
    }
    p.br();
    p.setFontBold(true);
    points.clear();
    points << 50 << 200;
    vals << tr("Date");
    if (!storeInName.isEmpty()) {
        vals << tr("Store, input");
        points << 500;
    }
    if (!storeOutName.isEmpty()) {
        vals << tr("Store, output");
        points << 500;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    vals.clear();
    vals << ui->deDate->text();
    if (!storeInName.isEmpty()) {
        vals << ui->leStoreInputName->text();
    }
    if (!storeOutName.isEmpty()) {
        vals << ui->leStoreOutputName->text();
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    points.clear();
    vals.clear();
    p.setFontBold(true);
    points << 50;
    if (ui->lePartner->getInteger() > 0) {
        vals << tr("Supplier");
        points << 1000;
    }
    if (!ui->leInvoiceNumber->isEmpty()) {
        vals << tr("Purchase document");
        points << 800;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    points.clear();
    vals.clear();
    points << 50;
    if (ui->lePartner->getInteger() > 0) {
        vals << ui->lePartnerName->text();
        points << 1000;
    }
    if (!ui->leInvoiceNumber->isEmpty()) {
        vals << ui->leInvoiceNumber->text() + " /    " + ui->deInvoiceDate->text();
        points << 800;
    }
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br();
    p.br();
    p.br();
    if (fDocType == DOC_TYPE_COMPLECTATION) {
        points.clear();
        points << 50 << 100 << 800 << 250;
        vals.clear();
        vals.append(tr("NN"));
        vals.append(tr("Other charges"));
        vals.append(tr("Amount"));
        p.setFontBold(true);
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
        for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
            vals.clear();
            vals.append(QString::number(i + 1));
            vals.append(ui->tblAdd->getString(i, 1));
            vals.append(float_str(ui->tblAdd->lineEdit(i, 2)->getDouble(), 2));
            p.setFontBold(false);
            p.tableText(points, vals, p.fLineHeight + 20);
            p.br(p.fLineHeight + 20);
        }
        p.br();
        points.clear();
        points << 50 << 100 << 200 << 600 << 250 << 250 << 250;
        vals.clear();
        vals.append(tr("NN"));
        vals.append(tr("Code"));
        vals.append(tr("Input material"));
        vals.append(tr("Qty"));
        vals.append(tr("Price"));
        vals.append(tr("Amount"));
        p.setFontBold(true);
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
        vals.clear();
        vals.append("1");
        vals.append(ui->leComplectationCode->text());
        vals.append(ui->leComplectationName->text());
        vals.append(ui->leComplectationQty->text());
        vals.append(float_str(ui->leTotal->getDouble() / ui->leComplectationQty->getDouble(), 2));
        vals.append(ui->leTotal->text());
        p.setFontBold(false);
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
    }
    p.br();
    QMap<int, int> goodsGroupMap;
    QList<QStringList> goodsGroupData;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        QStringList val  = {"", "", "", "0", "", "", "0"};
        if (ui->chLeaveFocusOnBarcode->isChecked()) {
            if (goodsGroupMap.contains(ui->tblGoods->getInteger(i, 3))) {
                val = goodsGroupData[goodsGroupMap[ui->tblGoods->getInteger(i, 3)]];
            } else {
                val[0] = QString::number(goodsGroupData.count() + 1);
            }
        } else {
            val[0] = QString::number(i + 1);
        }
        //val <<
        val[1] = ui->tblGoods->getString(i, 3);
        val[2] = ui->tblGoods->getString(i, 4);
        if (ui->chLeaveFocusOnBarcode->isChecked()) {
            val[3] = float_str(str_float(val[3]) + ui->tblGoods->lineEdit(i, 5)->getDouble(), 3);
        } else {
            val[3] = ui->tblGoods->lineEdit(i, 5)->text();
        }
        val[4] = ui->tblGoods->getString(i, 6);
        val[5] = ui->tblGoods->lineEdit(i, 7)->text();
        if (ui->chLeaveFocusOnBarcode->isChecked()) {
            val[6] = float_str(str_float(val[6]) + ui->tblGoods->lineEdit(i, 8)->getDouble(), 2);
        } else {
            val[6] = ui->tblGoods->lineEdit(i, 8)->text();
        }
        val[5] = float_str(str_float(val[6]) / str_float(val[3]), 2);
        if (ui->chLeaveFocusOnBarcode->isChecked()) {
            if (goodsGroupMap.contains(ui->tblGoods->getInteger(i, 3))) {
                goodsGroupData[goodsGroupMap[ui->tblGoods->getInteger(i, 3)]] = val;
            } else {
                goodsGroupData.append(val);
                goodsGroupMap[ui->tblGoods->getInteger(i, 3)] = goodsGroupData.count() - 1;
            }
        } else {
            goodsGroupData.append(val);
        }
    }
    QString goodsColName = tr("Goods");
    if (fDocType == DOC_TYPE_COMPLECTATION) {
        goodsColName = tr("Output material");
    }
    points.clear();
    points << 50 << 100 << 200 << 600 << 250 << 250 << 250 << 250;
    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << goodsColName
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Total");
    p.setFontBold(true);
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    for (int i = 0; i < goodsGroupData.count(); i++) {
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
            p.br();
        }
        vals = goodsGroupData[i];
        //        vals << QString::number(i + 1);
        //        vals << ui->tblGoods->getString(i, 3);
        //        vals << ui->tblGoods->getString(i, 4);
        //        vals << ui->tblGoods->lineEdit(i, 5)->text();
        //        vals << ui->tblGoods->getString(i, 6);
        //        vals << ui->tblGoods->lineEdit(i, 7)->text();
        //        vals << ui->tblGoods->lineEdit(i, 8)->text();
        p.tableText(points, vals, p.fLineHeight + 20);
        if (p.checkBr(p.fLineHeight + 20)) {
            p.br(p.fLineHeight + 20);
        }
        p.br(p.fLineHeight + 20);
    }
    p.setFontBold(true);
    points.clear();
    points << 950
           << 750
           << 250;
    vals.clear();
    vals << tr("Total amount");
    vals << ui->leTotal->text();
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    switch (fDocType) {
    case DOC_TYPE_STORE_MOVE:
        p.ltext(tr("Passed"), 50);
        p.ltext(tr("Accepted"), 1000);
        p.br(p.fLineHeight + 20);
        p.ltext(ui->lePassedName->text(), 50);
        p.ltext(ui->leAcceptedName->text(), 1000);
        break;
    default:
        p.ltext(tr("Accepted"), 50);
        p.ltext(tr("Passed"), 1000);
        p.br(p.fLineHeight + 20);
        p.ltext(ui->leAcceptedName->text(), 1000);
        p.ltext(ui->lePassedName->text(), 50);
        break;
    }
    p.line(50, p.fTop, 700, p.fTop);
    p.line(1000, p.fTop, 1650, p.fTop);
    p.setFontBold(false);
    */
    C5PrintPreview pp( &p, fDBParams);
    pp.exec();
}
