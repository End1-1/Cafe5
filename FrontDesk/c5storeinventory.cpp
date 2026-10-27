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
#include "dlginventorysemireadylist.h"
#include "dlgsemireadyinout.h"
#include "format_date.h"
#include "ninterface.h"
#include "office_structs.h"
#include "ui_c5storeinventory.h"
#include <QHash>

C5StoreInventory::C5StoreInventory(C5User *user, const QString &title, QIcon icon, QWidget *parent)
    : C5OfficeWidget(parent)
    , ui(new Ui::C5StoreInventory)
{
    ui->setupUi(this);
    fIcon = icon;
    fLabel = tr("Store inventory");
    ui->wStore->selectorCallback = storeItemSelector;
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 300, 0, 300, 80, 80, 80, 80, 80);
    ui->tblGoods->setColumnDecimals(col_qty_historical, 3);
    ui->tblGoods->setColumnDecimals(col_qty_diff, 3);
    ui->wSearch->setVisible(false);
    connect(ui->deDate, &C5DateEdit::dateChanged, this, [this](const QDate &) {
        refreshStockBalances();
    });
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
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printDoc()));
        fToolBar->addAction(QIcon(":/checklists.png"), tr("Autofill"), this, SLOT(autofillDoc()));
        fToolBar->addAction(QIcon(":/diffs.png"), tr("Export diffs"), this, SLOT(exportDiffs()));
    }

    return fToolBar;
}

void C5StoreInventory::setDocument(StoreInventoryDocument sd)
{
    mSemireadyBatches.clear();
    QDate docDate = QDateTime::fromString(sd.date, FORMAT_DATETIME_TO_STR_MYSQL).date();
    if (!docDate.isValid()) {
        docDate = QDate::fromString(sd.date.left(10), FORMAT_DATE_TO_STR_MYSQL);
    }
    if (docDate.isValid()) {
        ui->deDate->setDate(docDate);
    }
    ui->wStore->setCodeAndName(sd.store, sd.store_name);
    ui->leComment->setText(sd.comment());
    fInternalID = sd.uuid;
    ui->tblGoods->setRowCount(sd.items.size());
    for (int i = 0; i < sd.items.size(); i++) {
        const StoreInventoryUser &su = sd.items.at(i);
        ui->tblGoods->setString(i, col_group_name, su.group_name);
        ui->tblGoods->setInteger(i, col_goods_id, su.item_id);
        ui->tblGoods->setString(i, col_goods_name, su.item_name);
        ui->tblGoods->setDouble(i, col_qty_historical, su.qty_sys);
        ui->tblGoods->createLineEdit(i, col_qty_user)->setDouble(su.qty_user);
        ui->tblGoods->setDouble(i, col_qty_diff, su.qty_diff);
        ui->tblGoods->setDouble(i, col_price, su.price);
        ui->tblGoods->setString(i, col_unit, su.unit_name);
        setLockedQty(i, 0);

        connect(ui->tblGoods->lineEdit(i, col_qty_user), &C5LineEdit::textChanged, this, &C5StoreInventory::userQtyTextChanged);
    }
    refreshStockBalances();
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
        double qtySys = ui->tblGoods->getDouble(i, col_qty_historical);

        row["f_qty_sys"] = qtySys;
        row["f_qty_user"] = qtyUser;
        row["f_price"] = ui->tblGoods->getDouble(i, col_price);

        ja.append(row);
    }
    jo["items"] = ja;

    // Отправка на сервер
    NInterface::query1("/engine/v2/common/stock/save-inventory", mUser->mSessionKey, this, jo, [this](const QJsonObject &jdoc) {
        fInternalID = jdoc.value("uid").toString();
        C5Message::info(tr("Saved"));
    });
}

void C5StoreInventory::removeDocument()
{
    if (C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
        return;
    }

    if (fInternalID.isEmpty()) {
        __mainWindow->removeTab(this);
        return;
    }

    NInterface::query1("/engine/v2/common/stock/remove-inventory",
                       mUser->mSessionKey,
                       this,
                       {{"id", fInternalID}},
                       [this](const QJsonObject &) {
                           C5Message::info(tr("Deleted"));
                           __mainWindow->removeTab(this);
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
                               mSemireadyBatches.clear();
                               ui->tblGoods->setUpdatesEnabled(false);
                               ui->tblGoods->setRowCount(0);
                               auto ja = jo.value("data").toArray();
                               ui->tblGoods->setRowCount(ja.size());
                               for (int i = 0; i < ja.size(); i++) {
                                   const QJsonObject &ji = ja.at(i).toObject();
                                   ui->tblGoods->setString(i, col_group_name, ji.value("group_name").toString());
                                   ui->tblGoods->setInteger(i, col_goods_id, ji.value("item_id").toInteger());
                                   ui->tblGoods->setString(i, col_goods_name, ji.value("item_name").toString());
                                   ui->tblGoods->setDouble(i, col_qty_historical,
                                                           ji.value("qty_sys").toDouble(
                                                               str_table_amount(ji.value("qty_sys").toString())));
                                   ui->tblGoods->setDouble(i, col_price,
                                                           ji.value("price").toDouble(
                                                               str_table_amount(ji.value("price").toString())));
                                   ui->tblGoods->setString(i, col_unit, ji.value("unit_name").toString());
                                   auto *l = ui->tblGoods->createLineEdit(i, col_qty_user);
                                   connect(l, &C5LineEdit::textChanged, this, &C5StoreInventory::userQtyTextChanged);
                                   setLockedQty(i, 0);
                               }
                               ui->tblGoods->setUpdatesEnabled(true);
                           });
}

void C5StoreInventory::exportDiffs()
{
    commit_table_edits(ui->tblGoods);
    QList<InventoryDiff> plus, minus;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        C5LineEdit *le = ui->tblGoods->lineEdit(i, col_qty_user);
        if (!le) {
            continue;
        }
        // Корректировка = введённое − остаток на складе (в т.ч. отрицательный: 24−(−594)=618)
        const double qtyUser = le->getDouble();
        const double qtySys = ui->tblGoods->getDouble(i, col_qty_historical);
        const double diff = qtyUser - qtySys;
        ui->tblGoods->setDouble(i, col_qty_diff, diff);

        InventoryDiff id;
        id.goodsId = ui->tblGoods->getInteger(i, col_goods_id);
        id.goodsName = ui->tblGoods->getString(i, col_goods_name);
        id.unitName = ui->tblGoods->getString(i, col_unit);
        id.price = ui->tblGoods->getDouble(i, col_price);
        if (diff < -0.0001) {
            id.qty = qAbs(diff);
            minus.append(id);
        } else if (diff > 0.0001) {
            id.qty = diff;
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
        si->fillFromInventory(minus);
        si->setStore(ui->wStore->value(), ui->wStore->name());
        __mainWindow->addWidget(si);
    }
}

void C5StoreInventory::on_btnAddGoods_clicked()
{
    if (ui->wStore->value() == 0) {
        C5Message::error(tr("Select storage"));
        return;
    }
    const auto r = selectStoreDocGoods();
    if (r.isEmpty()) {
        return;
    }

    GoodsItem g = r.first();

    int row = addGoodsRow();
    ui->tblGoods->setString(row, col_group_name, g.groupName);
    ui->tblGoods->setInteger(row, col_goods_id, g.id);
    ui->tblGoods->setString(row, col_goods_name, g.name);
    if (ui->tblGoods->getString(row, col_unit).isEmpty()) {
        ui->tblGoods->setString(row, col_unit, g.unitName);
    }
    if (qAbs(ui->tblGoods->getDouble(row, col_price)) < 0.0000001) {
        ui->tblGoods->setDouble(row, col_price, g.lastInputPrice);
    }
    ui->tblGoods->lineEdit(row, col_qty_user)->setFocus();
    refreshStockBalances();
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
    connect(lqty, &QLineEdit::textChanged, this, &C5StoreInventory::userQtyTextChanged);

    return row;
}

void C5StoreInventory::countTotal()
{
    double total = 0;
    double qtySum = 0;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        C5LineEdit *lqty = ui->tblGoods->lineEdit(i, col_qty_user);
        if(!lqty) {
            continue;
        }
        const double qty = lqty->getDouble();
        qtySum += qty;
        total += qty * ui->tblGoods->getDouble(i, col_price);
    }

    Q_UNUSED(total);
    Q_UNUSED(qtySum);
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

    if(lockedQty(row) > 0.0001) {
        C5Message::error(tr("Remove this quantity via the semiready list"));
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
        ui->tblGoods->setRowHidden(i, !ui->tblGoods->getString(i, 2).contains(arg1, Qt::CaseInsensitive));
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
    Q_UNUSED(arg1);
    C5LineEdit *le = ui->tblGoods->lineEdit(row, col_qty_user);
    const double qtyUser = le ? le->getDouble() : str_table_amount(arg1);
    ui->tblGoods->setDouble(row, col_qty_diff, qtyUser - ui->tblGoods->getDouble(row, col_qty_historical));
}

int C5StoreInventory::findGoodsRow(int goodsId) const
{
    for (int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        if (ui->tblGoods->getInteger(i, col_goods_id) == goodsId) {
            return i;
        }
    }
    return -1;
}

double C5StoreInventory::lockedQty(int row) const
{
    QTableWidgetItem *item = ui->tblGoods->item(row, col_goods_id);
    if (!item) {
        return 0;
    }
    return item->data(Qt::UserRole).toDouble();
}

void C5StoreInventory::setLockedQty(int row, double qty)
{
    QTableWidgetItem *item = ui->tblGoods->item(row, col_goods_id);
    if (!item) {
        item = new QTableWidgetItem();
        ui->tblGoods->setItem(row, col_goods_id, item);
    }
    item->setData(Qt::UserRole, qty);
}

void C5StoreInventory::refreshRowDiff(int row)
{
    C5LineEdit *le = ui->tblGoods->lineEdit(row, col_qty_user);
    if (!le) {
        return;
    }
    ui->tblGoods->setDouble(row, col_qty_diff,
                            le->getDouble() - ui->tblGoods->getDouble(row, col_qty_historical));
}

void C5StoreInventory::refreshStockBalances()
{
    if (ui->wStore->value() == 0 || ui->tblGoods->rowCount() == 0 || !ui->deDate->date().isValid()) {
        return;
    }
    QJsonArray ids;
    for (int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        const int id = ui->tblGoods->getInteger(i, col_goods_id);
        if (id > 0) {
            ids.append(id);
        }
    }
    if (ids.isEmpty()) {
        return;
    }
    const QString date = ui->deDate->date().toString(FORMAT_DATE_TO_STR_MYSQL) + QStringLiteral(" 23:59:59");
    NInterface::query1("/engine/v2/common/stock/balances",
                       mUser->mSessionKey,
                       this,
                       {{"store", ui->wStore->value()}, {"date", date}, {"items", ids}},
                       [this](const QJsonObject &jo) {
                           QHash<int, double> qty;
                           const QJsonArray rows = jo.value(QStringLiteral("data")).toArray();
                           for (const QJsonValue &v : rows) {
                               const QJsonObject o = v.toObject();
                               qty.insert(o.value(QStringLiteral("item_id")).toInt(),
                                          o.value(QStringLiteral("qty_sys")).toDouble());
                           }
                           for (int i = 0; i < ui->tblGoods->rowCount(); ++i) {
                               const int id = ui->tblGoods->getInteger(i, col_goods_id);
                               if (!qty.contains(id)) {
                                   continue;
                               }
                               ui->tblGoods->setDouble(i, col_qty_historical, qty.value(id));
                               refreshRowDiff(i);
                           }
                       });
}

void C5StoreInventory::applySemireadyBatch(const SemireadyBatch &batch)
{
    for (const SemireadyComponent &c : batch.components) {
        if (c.goodsId <= 0 || c.qty <= 0.0000001) {
            continue;
        }

        int row = findGoodsRow(c.goodsId);
        if (row < 0) {
            row = addGoodsRow();
            ui->tblGoods->setInteger(row, col_goods_id, c.goodsId);
            ui->tblGoods->setString(row, col_goods_name, c.goodsName);
            ui->tblGoods->setDouble(row, col_qty_historical, 0);
            C5LineEdit *le = ui->tblGoods->lineEdit(row, col_qty_user);
            if (!le) {
                continue;
            }
            le->setDouble(c.qty);
            setLockedQty(row, c.qty);
        } else {
            C5LineEdit *le = ui->tblGoods->lineEdit(row, col_qty_user);
            if (!le) {
                continue;
            }
            le->setDouble(le->getDouble() + c.qty);
            setLockedQty(row, lockedQty(row) + c.qty);
        }
        refreshRowDiff(row);
    }

    mSemireadyBatches.append(batch);
    countTotal();
}

void C5StoreInventory::removeSemireadyBatch(const QString &batchId)
{
    int batchIndex = -1;
    for (int i = 0; i < mSemireadyBatches.size(); ++i) {
        if (mSemireadyBatches.at(i).id == batchId) {
            batchIndex = i;
            break;
        }
    }
    if (batchIndex < 0) {
        return;
    }

    const SemireadyBatch batch = mSemireadyBatches.takeAt(batchIndex);
    for (const SemireadyComponent &c : batch.components) {
        int row = findGoodsRow(c.goodsId);
        if (row < 0) {
            continue;
        }

        C5LineEdit *le = ui->tblGoods->lineEdit(row, col_qty_user);
        if (!le) {
            continue;
        }
        const double newQty = le->getDouble() - c.qty;
        const double newLocked = lockedQty(row) - c.qty;
        if (newQty <= 0.0001 && newLocked <= 0.0001) {
            ui->tblGoods->removeRow(row);
            continue;
        }

        le->setDouble(newQty > 0 ? newQty : 0);
        setLockedQty(row, newLocked > 0 ? newLocked : 0);
        refreshRowDiff(row);
    }

    countTotal();
}

void C5StoreInventory::on_btnAddSemiready_clicked()
{
    DlgSemireadyInOut dlg(mUser);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }

    if (dlg.dishId() <= 0 || dlg.dishQty() <= 0.0000001 || dlg.rowCount() <= 0) {
        return;
    }

    SemireadyBatch batch;
    batch.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    batch.dishId = dlg.dishId();
    batch.dishName = dlg.dishName();
    batch.dishQty = dlg.dishQty();

    for (int i = 0; i < dlg.rowCount(); ++i) {
        SemireadyComponent c;
        c.goodsId = dlg.goodsId(i);
        c.goodsName = dlg.goodsName(i);
        c.qty = dlg.goodsQty(i);
        if (c.goodsId <= 0 || c.qty <= 0.0000001) {
            continue;
        }
        batch.components.append(c);
    }

    if (batch.components.isEmpty()) {
        return;
    }

    applySemireadyBatch(batch);
    refreshStockBalances();
}

void C5StoreInventory::on_btnSemireadyList_clicked()
{
    DlgInventorySemireadyList dlg(this);
    dlg.setBatches(mSemireadyBatches);
    connect(&dlg, &DlgInventorySemireadyList::removeBatchRequested, this, [this, &dlg](const QString &batchId) {
        removeSemireadyBatch(batchId);
        dlg.setBatches(mSemireadyBatches);
    });
    dlg.exec();
}