#include "c5storeinventory.h"
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QShortcut>
#include <QUuid>
#include "c5codenameselectorfunctions.h"
#include "c5config.h"
#include "c5htmlprint.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5storeinput.h"
#include "c5storeoutput.h"
#include "c5user.h"
#include "c5utils.h"
#include "format_date.h"
#include "office_structs.h"
#include "ui_c5storeinventory.h"

C5StoreInventory::C5StoreInventory(C5User *user, const QString &title, QIcon icon, QWidget *parent)
    : C5OfficeWidget(parent)
    , ui(new Ui::C5StoreInventory)
{
    ui->setupUi(this);
    fIcon = icon;
    fLabel = tr("Store inventory");
    ui->wStore->selectorCallback = storeItemSelector;
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 300, 0, 300, 80, 80, 80, 80, 80);
    ui->wSearch->setVisible(false);
}

C5StoreInventory::~C5StoreInventory()
{
    delete ui;
}

QToolBar* C5StoreInventory::toolBar()
{
    if(!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDoc()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printDoc()));
        fToolBar->addAction(QIcon(":/checklists.png"), tr("Autofill"), this, SLOT(autofillDoc()));
        fToolBar->addAction(QIcon(":/diffs.png"), tr("Export diffs"), this, SLOT(exportDiffs()));
    }

    return fToolBar;
}

void C5StoreInventory::setDocument(StoreInventoryDocument sd)
{
    ui->deDate->setDate(QDateTime::fromString(sd.date, FORMAT_DATETIME_TO_STR_MYSQL).date());
    ui->wStore->setCodeAndName(sd.store, sd.store_name);
    ui->leComment->setText(sd.comment());
    fInternalID = sd.uuid;
    ui->tblGoods->setRowCount(sd.items.size());
    for (int i = 0; i < sd.items.size(); i++) {
        const StoreInventoryUser &su = sd.items.at(i);
        ui->tblGoods->setString(i, col_group_name, su.group_name);
        ui->tblGoods->setInteger(i, col_goods_id, su.item_id);
        ui->tblGoods->setString(i, col_goods_name, su.item_name);
        ui->tblGoods->setString(i, col_qty_historical, float_str(su.qty_sys, 3));
        ui->tblGoods->createLineEdit(i, col_qty_user)->setDouble(su.qty_user);
        ui->tblGoods->setString(i, col_qty_diff, float_str(su.qty_diff, 3));
        ui->tblGoods->setString(i, col_price, float_str(su.price, 2));
        ui->tblGoods->setString(i, col_unit, su.unit_name);

        connect(ui->tblGoods->lineEdit(i, col_qty_user), &C5LineEdit::textChanged, this, &C5StoreInventory::userQtyTextChanged);
    }
}

void C5StoreInventory::saveDoc()
{
    QString err;

    if(!docCheck(err)) {
        C5Message::error(err);
        return;
    }

    QJsonObject jo;
    // Данные заголовка документа (таблица store_inventory_document)
    jo["f_id"] = fInternalID;
    jo["f_date"] = ui->deDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    jo["f_store"] = ui->wStore->value();
    jo["f_data"] = {{"f_comment", ui->leComment->text()}};

    // Данные строк документа (таблица store_inventory_user)
    QJsonArray ja;
    for (int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        QJsonObject row;
        row["f_item"] = ui->tblGoods->getInteger(i, col_goods_id);

        // Получаем значения из LineEdit (колонка пользователя) и обычных ячеек (система)
        double qtyUser = ui->tblGoods->lineEdit(i, col_qty_user)->getDouble();
        double qtySys = str_float(ui->tblGoods->getString(i, col_qty_historical));

        row["f_qty_sys"] = qtySys;
        row["f_qty_user"] = qtyUser;

        // Если у тебя в таблице есть цена (например, скрытая или в доп. колонке)
        // Если цены нет в таблице, сервер сам подтянет f_lastinputprice
        row["f_price"] = str_float(ui->tblGoods->getString(i, col_price)); // Или возьми из данных, если добавил колонку цены

        ja.append(row);
    }
    jo["items"] = ja;

    // Отправка на сервер
    NInterface::query1("/engine/v2/common/stock/save-inventory", mUser->mSessionKey, this, jo, [this](const QJsonObject &jdoc) {
        fInternalID = jdoc.value("uid").toString();
        C5Message::info(tr("Saved"));
    });
}

QString C5StoreInventory::makeInventoryTable(C5TableWidget *tbl)
{
    QString h;
    QTextStream s(&h);
    s << "<table>";
    s << "<tr>"
      << "<th>NN</th>"
      << "<th>Material code</th>"
      << "<th>Goods</th>"
      << "<th class='right'>Qty</th>"
      << "<th>Unit</th>"
      << "<th class='right'>Price</th>"
      << "<th class='right'>Total</th>"
      << "</tr>";

    for(int i = 0; i < tbl->rowCount(); ++i) {
        s << "<tr>";
        s << "<td class='center'>" << i + 1 << "</td>";
        s << "<td>" << tbl->getString(i, 1) << "</td>";
        s << "<td>" << tbl->getString(i, 2) << "</td>";
        s << "<td class='right'>" << tbl->lineEdit(i, 3)->text() << "</td>";
        s << "<td>" << tbl->getString(i, 4) << "</td>";
        s << "<td class='right'>" << tbl->lineEdit(i, 5)->text() << "</td>";
        s << "<td class='right'>" << tbl->lineEdit(i, 6)->text() << "</td>";
        s << "</tr>";
    }

    s << "</table>";
    return h;
}

void C5StoreInventory::printDoc()
{
    QString html = loadTemplate("store_inventory_a4.html");

    if(html.isEmpty()) {
        C5Message::error("Template not found");
        return;
    }

    QMap<QString, QString> vars;
    vars["title"] =
        tr("Store inventorization") + " N" + ui->leDocNum->text();
    vars["date"] = ui->deDate->text();
    vars["goods_table"] = makeInventoryTable(ui->tblGoods);
    // динамические колонки
    vars["store_header"] =
        ui->wStore->name().isEmpty() ? "" : "<th>Store</th>";
    vars["store_value"] =
        ui->wStore->name().isEmpty() ? "" : "<td>" + ui->wStore->name() + "</td>";
    vars["comment_header"] =
        ui->leComment->isEmpty() ? "" : "<th>Comment</th>";
    vars["comment_value"] =
        ui->leComment->isEmpty() ? "" : "<td>" + ui->leComment->text() + "</td>";
    html = applyTemplate(html, vars);
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setFullPage(false);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, [&](QPrinter * p) { doc.print(p); });
    preview.exec();
}

void C5StoreInventory::autofillDoc()
{
    if (ui->wStore->value() == 0) {
        C5Message::error(tr("Select storage"));
        return;
    }
    if (C5Message::question(tr("This action will clear current result")))
        NInterface::query1("/engine/v2/common/stock/historical-store",
                           mUser->mSessionKey,
                           this,
                           {{"store", ui->wStore->value()}, {"date", ui->deDate->date().toString(FORMAT_DATE_TO_STR_MYSQL) + " 23:59:59"}},
                           [this](const QJsonObject &jo) {
                               ui->tblGoods->setUpdatesEnabled(false);
                               ui->tblGoods->setRowCount(0);
                               auto ja = jo.value("data").toArray();
                               ui->tblGoods->setRowCount(ja.size());
                               for (int i = 0; i < ja.size(); i++) {
                                   const QJsonObject &ji = ja.at(i).toObject();
                                   ui->tblGoods->setString(i, col_group_name, ji.value("group_name").toString());
                                   ui->tblGoods->setInteger(i, col_goods_id, ji.value("item_id").toInteger());
                                   ui->tblGoods->setString(i, col_goods_name, ji.value("item_name").toString());
                                   ui->tblGoods->setString(i, col_qty_historical, ji.value("qty_sys").toString());
                                   ui->tblGoods->setString(i, col_price, ji.value("price").toString());
                                   ui->tblGoods->setString(i, col_unit, ji.value("unit_name").toString());
                                   auto *l = ui->tblGoods->createLineEdit(i, col_qty_user);
                                   connect(l, &C5LineEdit::textChanged, this, &C5StoreInventory::userQtyTextChanged);
                               }
                               ui->tblGoods->setUpdatesEnabled(true);
                           });
}

void C5StoreInventory::exportDiffs()
{
    QList<InventoryDiff> plus, minus;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        double diff = str_float(ui->tblGoods->getString(i, col_qty_diff));
        InventoryDiff id;
        id.goodsId = ui->tblGoods->getInteger(i, col_goods_id);
        id.goodsName = ui->tblGoods->getString(i, col_goods_name);
        id.unitName = ui->tblGoods->getString(i, col_unit);
        id.price = str_float(ui->tblGoods->getString(i, col_price));
        id.qty = diff;
        if (diff < -0.0001) {
            minus.append(id);
        } else if (diff > 0.0001) {
            plus.append(id);
        }
    }
    if (plus.size() > 0) {
        auto *si = new C5StoreInput(mUser, tr("Store input"), QIcon());
        si->fillFromInventory(plus);
        si->setStore(ui->wStore->value(), ui->wStore->name());
        __mainWindow->addWidget(si);
    }
    if (minus.size() > 0) {
        auto *si = new C5StoreOutput(mUser, tr("Store output"), QIcon());
        si->fillFromInventory(plus);
        si->setStore(ui->wStore->value(), ui->wStore->name());
        __mainWindow->addWidget(si);
    }
}

void C5StoreInventory::on_btnAddGoods_clicked()
{
    const auto r = selectItem<GoodsItem>(false, false);
    if (r.isEmpty()) {
        return;
    }

    GoodsItem g = r.first();

    int row = addGoodsRow();
    ui->tblGoods->setString(row, col_group_name, g.groupName);
    ui->tblGoods->setInteger(row, col_goods_id, g.id);
    ui->tblGoods->setString(row, col_goods_name, g.name);
    ui->tblGoods->lineEdit(row, col_qty_user)->setFocus();
}

int C5StoreInventory::addGoodsRow()
{
    int row = ui->tblGoods->rowCount();
    ui->tblGoods->setRowCount(row + 1);
    ui->tblGoods->setItem(row, col_group_name, new QTableWidgetItem());
    ui->tblGoods->setItem(row, col_goods_id, new QTableWidgetItem());
    ui->tblGoods->setItem(row, col_goods_name, new QTableWidgetItem());
    C5LineEdit *lqty = ui->tblGoods->createLineEdit(row, col_qty_user);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 3));
    ui->tblGoods->setItem(row, 4, new QTableWidgetItem());
    connect(lqty, &QLineEdit::textChanged, this, &C5StoreInventory::userQtyTextChanged);

    return row;
}

void C5StoreInventory::countTotal()
{
    double total = 0;
    double qtySum = 0;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, 6)->getDouble();
        qtySum += ui->tblGoods->lineEdit(i, 3)->getDouble();
    }
}

bool C5StoreInventory::docCheck(QString &err)
{
    if(ui->wStore->value() == 0) {
        err += tr("Input store is not defined") + "<br>";
    }

    if(ui->tblGoods->rowCount() == 0) {
        err += tr("Cannot save an emtpy document") + "<br>";
    }

    return err.isEmpty();
}

void C5StoreInventory::on_btnRemoveGoods_clicked()
{
    int row = ui->tblGoods->currentRow();

    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblGoods->item(row, 2)->text()) != QDialog::Accepted) {
        return;
    }

    ui->tblGoods->removeRow(row);
    countTotal();
}

void C5StoreInventory::on_leSearch_textChanged(const QString &arg1)
{
    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        ui->tblGoods->setRowHidden(i, !ui->tblGoods->getString(i, 2).contains(arg1));
    }
}

void C5StoreInventory::on_btnCloseSearch_clicked()
{
    ui->wSearch->setVisible(false);
}

void C5StoreInventory::on_leFind_textChanged(const QString &arg1)
{
    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        bool rh = (!ui->tblGoods->getString(i, 1).contains(arg1, Qt::CaseInsensitive)
                   && !ui->tblGoods->getString(i, 2).contains(arg1, Qt::CaseInsensitive)) ;
        ui->tblGoods->setRowHidden(i, rh);
    }
}

void C5StoreInventory::userQtyTextChanged(const QString &arg1)
{
    int row, col;
    if (!ui->tblGoods->findWidget(qobject_cast<QWidget *>(sender()), row, col)) {
        return;
    }
    ui->tblGoods->setDouble(row, col_qty_diff, str_float(arg1) - str_float(ui->tblGoods->getString(row, col_qty_historical)));
}
