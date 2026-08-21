#include "c5storecomplectation.h"
#include "ui_c5storecomplectation.h"

#include "c5codenameselectorfunctions.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5user.h"
#include "c5utils.h"
#include "format_date.h"
#include "ninterface.h"
#include "office_structs.h"
#include "store_document_status.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QUuid>

C5StoreComplectation::C5StoreComplectation(C5User *user, const QString &title, QIcon icon, QWidget *parent)
    : C5Widget(parent)
    , ui(new Ui::C5StoreComplectation)
{
    mUser = user;
    ui->setupUi(this);
    fLabel = title;
    fIcon = icon;
    ui->wOutputStore->selectorCallback = storeItemSelector;
    ui->wInputStore->selectorCallback = storeItemSelector;
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 400, 80, 80, 80);
    ui->leComplectationQty->setValidator(new QDoubleValidator(0, 1000000, 4));
    ui->leComplectationQty->fDecimalPlaces = 4;
    ui->leComplectationQty->setDouble(1);
    ui->deDate->setEnabled(mUser->check(cp_t1_allow_change_store_doc_date));
}

C5StoreComplectation::~C5StoreComplectation()
{
    delete ui;
}

void C5StoreComplectation::setDocument(const QJsonObject &docJo)
{
    mDocData = JsonParser<StoreInputDocument>::fromJson(docJo);
    mComplectGoodsId = docJo.value(QStringLiteral("complect_goods")).toVariant().toInt();
    if (mComplectGoodsId <= 0) {
        mComplectGoodsId = mDocData.data.value(QStringLiteral("complect_goods")).toVariant().toInt();
    }
    mComplectRowId = docJo.value(QStringLiteral("complect_row_id")).toString(
        mDocData.data.value(QStringLiteral("complect_row_id")).toString());
    const double cq = docJo.value(QStringLiteral("complect_qty")).toVariant().toDouble();
    ui->deDate->setDate(QDateTime::fromString(mDocData.date, FORMAT_DATETIME_TO_STR_MYSQL).date());
    ui->leDocNum->setText(mDocData.user_id);
    ui->wOutputStore->setCodeAndName(mDocData.store_out, mDocData.store_out_name);
    ui->wInputStore->setCodeAndName(mDocData.store_in, mDocData.store_in_name);
    ui->leComment->setText(mDocData.comment());
    ui->leComplectationCode->setInteger(mComplectGoodsId);
    ui->leComplectationName->setText(docJo.value(QStringLiteral("complect_goods_name")).toString());
    ui->leComplectationScancode->setText(docJo.value(QStringLiteral("complect_scancode")).toString());
    ui->lbComplectUnit->setText(docJo.value(QStringLiteral("complect_unit_name")).toString());
    ui->leComplectationQty->setDouble(cq > 0 ? cq : 1.0);
    ui->tblGoods->setRowCount(0);
    for (const auto &su : mDocData.items) {
        const double baseQty = cq > 0.0001 ? (su.qty / cq) : su.qty;
        addMaterial(su.item_id, su.item_name, baseQty, su.qty, su.unit_name);
    }
    countTotal();
    setState();
}

QToolBar *C5StoreComplectation::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        mActionSave = fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDocument()));
        mActionDraft = fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDocument()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New\ndocument"), this, [this]() {
            __mainWindow->addWidget(new C5StoreComplectation(mUser, tr("Store complectation"), QIcon(":/storage.png")));
        });
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        setState();
    }
    return fToolBar;
}

bool C5StoreComplectation::buildDoc(QJsonObject &outDoc)
{
    QString err;
    if (mComplectGoodsId <= 0) {
        err += tr("Complectation goods not selected") + "<br>";
    }
    if (ui->leComplectationQty->getDouble() < 0.0001) {
        err += tr("Complectation qty required") + "<br>";
    }
    if (ui->tblGoods->rowCount() == 0) {
        err += tr("Empty document") + "<br>";
    }
    if (ui->wOutputStore->value() == 0) {
        err += tr("Output store not selected") + "<br>";
    }
    if (ui->wInputStore->value() == 0) {
        err += tr("Input store not selected") + "<br>";
    }
    QJsonArray items;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        const int itemId = ui->tblGoods->getInteger(i, col_goods_id);
        const double qty = ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble();
        if (itemId <= 0 || qty < 0.0001) {
            err += tr("Quantity not valid on row #") + QString::number(i + 1) + "<br>";
            continue;
        }
        items.append(QJsonObject{
            {"id", QUuid::createUuid().toString(QUuid::WithoutBraces)},
            {"item_id", itemId},
            {"qty", qty},
            {"price", 0},
            {"row", i},
        });
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return false;
    }
    if (mDocData.uuid.isEmpty()) {
        mDocData.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    if (mComplectRowId.isEmpty()) {
        mComplectRowId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    outDoc = QJsonObject{
        {"doc_uuid", mDocData.uuid},
        {"doc_user_id", ui->leDocNum->text()},
        {"doc_date", ui->deDate->toMySQLDate(false)},
        {"doc_status", mDocData.status},
        {"doc_type", DOC_TYPE_STORE_COMPLECTATION},
        {"doc_store_out", ui->wOutputStore->value()},
        {"doc_store_in", ui->wInputStore->value()},
        {"doc_sum", 0},
        {"doc_create_user", mUser->id()},
        {"doc_version", mDocData.version},
        {"complect_goods", mComplectGoodsId},
        {"complect_qty", ui->leComplectationQty->getDouble()},
        {"complect_row_id", mComplectRowId},
        {"items", items},
        {"doc_data",
         QJsonObject{{"comment", ui->leComment->text()},
                     {"create_user", mUser->fullName()},
                     {"create_date", QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)},
                     {"kind", "complect"},
                     {"complect_goods", mComplectGoodsId},
                     {"complect_qty", ui->leComplectationQty->getDouble()},
                     {"complect_row_id", mComplectRowId}}},
    };
    return true;
}

void C5StoreComplectation::setState()
{
    const bool editable = (mDocData.status != STORE_DOC_STATUS_POSTED);
    ui->wOutputStore->setEnabled(editable);
    ui->wInputStore->setEnabled(editable);
    ui->btnSelectComplect->setEnabled(editable);
    ui->leComplectationQty->setEnabled(editable);
    ui->btnAddGoods->setEnabled(editable);
    ui->btnRemoveGoods->setEnabled(editable);
    if (mActionSave) {
        mActionSave->setEnabled(editable);
    }
}

void C5StoreComplectation::countTotal()
{
    double qty = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (auto *l = ui->tblGoods->lineEdit(i, col_goods_qty)) {
            qty += l->getDouble();
        } else {
            qty += ui->tblGoods->getDouble(i, col_goods_qty);
        }
    }
    ui->leTotalQty->setDouble(qty);
}

void C5StoreComplectation::scaleMaterials()
{
    const double cq = ui->leComplectationQty->getDouble();
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        const double base = ui->tblGoods->getDouble(i, col_base_qty);
        if (auto *l = ui->tblGoods->lineEdit(i, col_goods_qty)) {
            l->setDouble(base * cq);
        } else {
            ui->tblGoods->setData(i, col_goods_qty, base * cq);
        }
    }
    countTotal();
}

int C5StoreComplectation::addMaterial(int goodsId, const QString &name, double baseQty, double qty, const QString &unit)
{
    const int row = ui->tblGoods->rowCount();
    ui->tblGoods->setRowCount(row + 1);
    ui->tblGoods->setData(row, col_goods_id, goodsId);
    ui->tblGoods->setData(row, col_goods_name, name);
    ui->tblGoods->setData(row, col_base_qty, baseQty);
    auto *lqty = ui->tblGoods->createLineEdit(row, col_goods_qty);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 4));
    lqty->fDecimalPlaces = 4;
    lqty->setDouble(qty);
    connect(lqty, &C5LineEdit::textEdited, this, &C5StoreComplectation::tblQtyChanged);
    ui->tblGoods->setData(row, col_goods_unit, unit);
    return row;
}

void C5StoreComplectation::loadRecipe(int complectId)
{
    NInterface::query1("/engine/v2/officen/complect/get",
                       mUser->mSessionKey,
                       this,
                       {{"complect_id", complectId}},
                       [this](const QJsonObject &jo) {
                           ui->tblGoods->setRowCount(0);
                           const QJsonArray items = jo.value(QStringLiteral("complect_items")).toArray();
                           for (const QJsonValue &v : items) {
                               const QJsonObject o = v.toObject();
                               const double baseQty = o.value(QStringLiteral("f_qty")).toVariant().toDouble();
                               addMaterial(o.value(QStringLiteral("f_id")).toVariant().toInt(),
                                           o.value(QStringLiteral("f_name")).toString(),
                                           baseQty,
                                           baseQty * ui->leComplectationQty->getDouble(),
                                           o.value(QStringLiteral("f_unit_name")).toString());
                           }
                           countTotal();
                       });
}

void C5StoreComplectation::applySaveResult(const QJsonObject &jo, int status)
{
    mDocData.uuid = jo.value(QStringLiteral("id")).toString(mDocData.uuid);
    mComplectRowId = jo.value(QStringLiteral("complect_row_id")).toString(mComplectRowId);
    const int ver = jo.value(QStringLiteral("version")).toInt();
    if (ver > 0) {
        mDocData.version = ver;
    } else {
        mDocData.version++;
    }
    mDocData.status = status;
    setState();
    C5Message::info(tr("Saved"));
}

void C5StoreComplectation::saveDocument()
{
    mDocData.status = STORE_DOC_STATUS_POSTED;
    QJsonObject jdoc;
    if (!buildDoc(jdoc)) {
        return;
    }
    NInterface::query1("/engine/v2/common/store-move/complect",
                       mUser->mSessionKey,
                       this,
                       {{"doc", jdoc}},
                       [this](const QJsonObject &jo) {
                           applySaveResult(jo, STORE_DOC_STATUS_POSTED);
                       });
}

void C5StoreComplectation::draftDocument()
{
    mDocData.status = STORE_DOC_STATUS_DRAFT;
    QJsonObject jdoc;
    if (!buildDoc(jdoc)) {
        return;
    }
    NInterface::query1("/engine/v2/common/store-move/complect",
                       mUser->mSessionKey,
                       this,
                       {{"doc", jdoc}},
                       [this](const QJsonObject &jo) {
                           applySaveResult(jo, STORE_DOC_STATUS_DRAFT);
                       });
}

void C5StoreComplectation::removeDocument()
{
    if (mDocData.uuid.isEmpty()) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
        return;
    }
    NInterface::query1("/engine/v2/common/store-move/remove",
                       mUser->mSessionKey,
                       this,
                       {{"id", mDocData.uuid}},
                       [this](const QJsonObject &) {
                           C5Message::info(tr("Deleted"));
                           mDocData = StoreInputDocument();
                           mComplectRowId.clear();
                           mComplectGoodsId = 0;
                           ui->tblGoods->setRowCount(0);
                           ui->leComplectationCode->clear();
                           ui->leComplectationName->clear();
                           ui->leComplectationScancode->clear();
                           ui->leComplectationQty->setDouble(1);
                           countTotal();
                           setState();
                       });
}

void C5StoreComplectation::tblQtyChanged(const QString &)
{
    int row = -1, col = -1;
    if (!ui->tblGoods->findWidget(static_cast<QWidget *>(sender()), row, col)) {
        return;
    }
    const double cq = ui->leComplectationQty->getDouble();
    const double qty = ui->tblGoods->lineEdit(row, col_goods_qty)->getDouble();
    ui->tblGoods->setData(row, col_base_qty, cq > 0.0001 ? (qty / cq) : qty);
    countTotal();
}

void C5StoreComplectation::on_btnSelectComplect_clicked()
{
    const auto r = selectItem<GoodsItem>(false, false);
    if (r.isEmpty()) {
        return;
    }
    const GoodsItem g = r.first();
    mComplectGoodsId = g.id;
    ui->leComplectationCode->setInteger(g.id);
    ui->leComplectationName->setText(g.name);
    ui->leComplectationScancode->setText(g.barcode);
    ui->lbComplectUnit->setText(g.unitName);
    if (ui->leComplectationQty->getDouble() < 0.0001) {
        ui->leComplectationQty->setDouble(1);
    }
    loadRecipe(g.id);
}

void C5StoreComplectation::on_btnAddGoods_clicked()
{
    const auto r = selectItem<GoodsItem>(false, false);
    if (r.isEmpty()) {
        return;
    }
    const GoodsItem g = r.first();
    const int row = addMaterial(g.id, g.name, 0, 0, g.unitName);
    ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
}

void C5StoreComplectation::on_btnRemoveGoods_clicked()
{
    const int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    ui->tblGoods->removeRow(row);
    countTotal();
}

void C5StoreComplectation::on_leComplectationQty_textEdited(const QString &)
{
    scaleMaterials();
}
