#include "c5storemovement.h"
#include "ui_c5storemovement.h"

#include "c5codenameselectorfunctions.h"
#include "c5config.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5user.h"
#include "c5utils.h"
#include "dlginventorysemireadylist.h"
#include "dlgsemireadyinout.h"
#include "format_date.h"
#include "ninterface.h"
#include "office_structs.h"
#include "store_document_status.h"

#include <QUuid>

C5StoreMovement::C5StoreMovement(C5User *user, const QString &title, QIcon icon, QWidget *parent)
    : C5Widget(parent)
    , ui(new Ui::C5StoreMovement)
{
    mUser = user;
    ui->setupUi(this);
    fLabel = title;
    fIcon = icon;
    ui->wOutputStore->selectorCallback = storeItemSelector;
    ui->wInputStore->selectorCallback = storeItemSelector;
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 350, 80, 80, 80, 100);
    ui->deDate->setEnabled(mUser->check(cp_t1_allow_change_store_doc_date));
}

C5StoreMovement::~C5StoreMovement()
{
    delete ui;
}

void C5StoreMovement::setDocument(StoreInputDocument doc)
{
    mSemireadyBatches.clear();
    mDocData = doc;
    ui->deDate->setDate(QDateTime::fromString(doc.date, FORMAT_DATETIME_TO_STR_MYSQL).date());
    ui->leDocNum->setText(doc.user_id);
    ui->wOutputStore->setCodeAndName(doc.store_out, doc.store_out_name);
    ui->wInputStore->setCodeAndName(doc.store_in, doc.store_in_name);
    ui->leComment->setText(doc.comment());
    ui->tblGoods->setRowCount(0);
    for (const auto &su : doc.items) {
        addGoods(su.item_id, su.item_name, su.qty, su.unit_name, su.price);
    }
    countTotal();
    setState();
}

QToolBar *C5StoreMovement::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        mActionSave = fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDocument()));
        mActionDraft = fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDocument()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New\ndocument"), this, [this]() {
            __mainWindow->addWidget(new C5StoreMovement(mUser, tr("Store movement"), QIcon(":/storage.png")));
        });
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        setState();
    }
    return fToolBar;
}

bool C5StoreMovement::buildDoc()
{
    QString err;
    if (ui->tblGoods->rowCount() == 0) {
        err += tr("Empty document") + "<br>";
    }
    if (ui->wOutputStore->value() == 0) {
        err += tr("Output store not selected") + "<br>";
    }
    if (ui->wInputStore->value() == 0) {
        err += tr("Input store not selected") + "<br>";
    }
    if (ui->wOutputStore->value() > 0 && ui->wOutputStore->value() == ui->wInputStore->value()) {
        err += tr("Stores must be different") + "<br>";
    }
    if (mDocData.uuid.isEmpty()) {
        mDocData.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    mDocData.user_id = ui->leDocNum->text();
    mDocData.date = ui->deDate->toMySQLDate(false);
    mDocData.type = DOC_TYPE_STORE_MOVE;
    mDocData.create_user = mUser->id();
    mDocData.store_out = ui->wOutputStore->value();
    mDocData.store_in = ui->wInputStore->value();
    mDocData.data = {{"comment", ui->leComment->text()},
                     {"create_user", mUser->fullName()},
                     {"create_date", QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)},
                     {"kind", "move"}};
    mDocData.sum = ui->leTotal->getDouble();
    mDocData.items.clear();
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        StoreUser st;
        st.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
        st.item_id = ui->tblGoods->getInteger(i, col_goods_id);
        st.qty = ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble();
        st.price = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        st.row = i;
        mDocData.items.append(st);
        if (st.item_id <= 0 || st.qty < 0.001) {
            err += tr("Quantity not valid on row #") + QString::number(i + 1) + "<br>";
        }
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return false;
    }
    return true;
}

void C5StoreMovement::setState()
{
    const bool draft = (mDocData.status == STORE_DOC_STATUS_DRAFT) || mDocData.uuid.isEmpty();
    if (mActionSave) {
        mActionSave->setEnabled(draft || mDocData.status == STORE_DOC_STATUS_DRAFT);
    }
    if (mActionDraft) {
        mActionDraft->setEnabled(true);
    }
    const bool editable = (mDocData.status != STORE_DOC_STATUS_POSTED);
    ui->wOutputStore->setEnabled(editable);
    ui->wInputStore->setEnabled(editable);
    ui->btnAddGoods->setEnabled(editable);
    ui->btnAddSemiready->setEnabled(editable);
    ui->btnRemoveGoods->setEnabled(editable);
    ui->btnSemireadyList->setEnabled(editable);
}

void C5StoreMovement::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, col_total)->getDouble();
    }
    ui->leTotal->setDouble(total);
}

int C5StoreMovement::addGoodsRow()
{
    const int row = ui->tblGoods->rowCount();
    ui->tblGoods->setRowCount(row + 1);
    ui->tblGoods->setItem(row, col_goods_id, new QTableWidgetItem());
    ui->tblGoods->setItem(row, col_goods_name, new QTableWidgetItem());
    ui->tblGoods->setItem(row, col_goods_unit, new QTableWidgetItem());
    auto *lqty = ui->tblGoods->createLineEdit(row, col_goods_qty);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 4));
    lqty->fDecimalPlaces = 4;
    connect(lqty, &C5LineEdit::textEdited, this, &C5StoreMovement::tblQtyChanged);
    auto *lprice = ui->tblGoods->createLineEdit(row, col_price);
    lprice->setValidator(new QDoubleValidator(0, 100000000, 3));
    lprice->fDecimalPlaces = 3;
    connect(lprice, &C5LineEdit::textEdited, this, &C5StoreMovement::tblPriceChanged);
    ui->tblGoods->createLineEdit(row, col_total)->setReadOnly(true);
    return row;
}

int C5StoreMovement::addGoods(int goods, const QString &name, double qty, const QString &unit, double price)
{
    const int row = addGoodsRow();
    ui->tblGoods->setData(row, col_goods_id, goods);
    ui->tblGoods->setData(row, col_goods_name, name);
    ui->tblGoods->setData(row, col_goods_unit, unit);
    if (qty > 0) {
        ui->tblGoods->lineEdit(row, col_goods_qty)->setDouble(qty);
    }
    if (price > 0) {
        ui->tblGoods->lineEdit(row, col_price)->setDouble(price);
        ui->tblGoods->lineEdit(row, col_total)->setDouble(qty * price);
    }
    return row;
}

void C5StoreMovement::saveDocument()
{
    if (!buildDoc()) {
        return;
    }
    mDocData.status = STORE_DOC_STATUS_POSTED;
    NInterface::query1("/engine/v2/common/store-move/transfer",
                       mUser->mSessionKey,
                       this,
                       {{"doc", mDocData.toJson()}},
                       [this](const QJsonObject &jo) {
                           mDocData.uuid = jo.value(QStringLiteral("id")).toString(mDocData.uuid);
                           if (jo.contains(QStringLiteral("version"))) {
                               mDocData.version = jo.value(QStringLiteral("version")).toInt();
                           } else {
                               mDocData.version++;
                           }
                           mDocData.status = STORE_DOC_STATUS_POSTED;
                           setState();
                           C5Message::info(tr("Saved"));
                       });
}

void C5StoreMovement::draftDocument()
{
    if (!buildDoc()) {
        return;
    }
    mDocData.status = STORE_DOC_STATUS_DRAFT;
    NInterface::query1("/engine/v2/common/store-move/transfer",
                       mUser->mSessionKey,
                       this,
                       {{"doc", mDocData.toJson()}},
                       [this](const QJsonObject &jo) {
                           mDocData.uuid = jo.value(QStringLiteral("id")).toString(mDocData.uuid);
                           if (jo.contains(QStringLiteral("version"))) {
                               mDocData.version = jo.value(QStringLiteral("version")).toInt();
                           } else {
                               mDocData.version++;
                           }
                           mDocData.status = STORE_DOC_STATUS_DRAFT;
                           setState();
                           C5Message::info(tr("Saved"));
                       });
}

void C5StoreMovement::removeDocument()
{
    if (C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
        return;
    }

    if (mDocData.uuid.isEmpty()) {
        __mainWindow->removeTab(this);
        return;
    }

    NInterface::query1("/engine/v2/common/store-move/remove",
                       mUser->mSessionKey,
                       this,
                       {{"id", mDocData.uuid}},
                       [this](const QJsonObject &) {
                           C5Message::info(tr("Deleted"));
                           __mainWindow->removeTab(this);
                       });
}

void C5StoreMovement::tblQtyChanged(const QString &)
{
    int row = -1, col = -1;
    if (!ui->tblGoods->findWidget(static_cast<QWidget *>(sender()), row, col)) {
        return;
    }
    const double qty = ui->tblGoods->lineEdit(row, col_goods_qty)->getDouble();
    const double price = ui->tblGoods->lineEdit(row, col_price)->getDouble();
    ui->tblGoods->lineEdit(row, col_total)->setDouble(qty * price);
    countTotal();
}

void C5StoreMovement::tblPriceChanged(const QString &)
{
    tblQtyChanged(QString());
}

void C5StoreMovement::on_btnAddGoods_clicked()
{
    const auto r = selectStoreDocGoods();
    if (r.isEmpty()) {
        return;
    }
    const GoodsItem g = r.first();
    const int row = addGoods(g.id, g.name, 0, g.unitName, 0);
    ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(g.lastInputPrice, 2));
    ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
}

void C5StoreMovement::on_btnRemoveGoods_clicked()
{
    const int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    if (lockedQty(row) > 0.0001) {
        C5Message::error(tr("Remove this quantity via the semiready list"));
        return;
    }
    ui->tblGoods->removeRow(row);
    countTotal();
}

int C5StoreMovement::findGoodsRow(int goodsId) const
{
    for (int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        if (ui->tblGoods->getInteger(i, col_goods_id) == goodsId) {
            return i;
        }
    }
    return -1;
}

double C5StoreMovement::lockedQty(int row) const
{
    QTableWidgetItem *item = ui->tblGoods->item(row, col_goods_id);
    if (!item) {
        return 0;
    }
    return item->data(Qt::UserRole).toDouble();
}

void C5StoreMovement::setLockedQty(int row, double qty)
{
    QTableWidgetItem *item = ui->tblGoods->item(row, col_goods_id);
    if (!item) {
        item = new QTableWidgetItem();
        ui->tblGoods->setItem(row, col_goods_id, item);
    }
    item->setData(Qt::UserRole, qty);
}

void C5StoreMovement::refreshRowTotal(int row)
{
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
    if (!lqty || !lprice || !ltotal) {
        return;
    }
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
}

void C5StoreMovement::applySemireadyBatch(const SemireadyBatch &batch)
{
    for (const SemireadyComponent &c : batch.components) {
        if (c.goodsId <= 0 || c.qty <= 0.0000001) {
            continue;
        }

        int row = findGoodsRow(c.goodsId);
        if (row < 0) {
            row = addGoods(c.goodsId, c.goodsName, c.qty, QString(), 0);
            setLockedQty(row, c.qty);
        } else {
            C5LineEdit *le = ui->tblGoods->lineEdit(row, col_goods_qty);
            if (!le) {
                continue;
            }
            le->setDouble(le->getDouble() + c.qty);
            setLockedQty(row, lockedQty(row) + c.qty);
            refreshRowTotal(row);
        }
    }

    mSemireadyBatches.append(batch);
    countTotal();
}

void C5StoreMovement::removeSemireadyBatch(const QString &batchId)
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

        C5LineEdit *le = ui->tblGoods->lineEdit(row, col_goods_qty);
        const double newQty = le->getDouble() - c.qty;
        const double newLocked = lockedQty(row) - c.qty;
        if (newQty <= 0.0001 && newLocked <= 0.0001) {
            ui->tblGoods->removeRow(row);
            continue;
        }

        le->setDouble(newQty > 0 ? newQty : 0);
        setLockedQty(row, newLocked > 0 ? newLocked : 0);
        refreshRowTotal(row);
    }

    countTotal();
}

void C5StoreMovement::on_btnAddSemiready_clicked()
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
}

void C5StoreMovement::on_btnSemireadyList_clicked()
{
    DlgInventorySemireadyList dlg(this);
    dlg.setBatches(mSemireadyBatches);
    connect(&dlg, &DlgInventorySemireadyList::removeBatchRequested, this, [this, &dlg](const QString &batchId) {
        removeSemireadyBatch(batchId);
        dlg.setBatches(mSemireadyBatches);
    });
    dlg.exec();
}
