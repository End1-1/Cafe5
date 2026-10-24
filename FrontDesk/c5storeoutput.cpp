#include "c5storeoutput.h"
#include <QClipboard>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHash>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMenu>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QShortcut>
#include <QSqlQuery>
#include "appwebsocket.h"
#include "c5codenameselector.h"
#include "c5database.h"
#include "c5codenameselectorfunctions.h"
#include "c5config.h"
#include "c5dateedit.h"
#include "c5editor.h"
#include "c5htmlprint.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5user.h"
#include "c5utils.h"
#include "store_document_status.h"
#include "calculator.h"
#include "ce5goods.h"
#include "format_date.h"
#include "ui_c5storeoutput.h"
#include "dlginventorysemireadylist.h"
#include "dlgsemireadyinout.h"
#include <QUuid>
#include <xlsxdocument.h>

namespace {

QString normalizeStoreBarcode(QString code)
{
    code = code.trimmed();
    if (code.startsWith(QLatin1Char(';'))) {
        code.remove(0, 1);
    }
    if (code.endsWith(QLatin1Char('?'))) {
        code.chop(1);
    }
    return code.trimmed();
}

bool lookupStoreGoodsByBarcode(const QString &barcode, GoodsItem &g)
{
    if (barcode.isEmpty()) {
        return false;
    }

    C5Database db;
    db[":barcode"] = barcode;
    db.exec("select g.f_id, g.f_name, u.f_name as f_unit_name, g.f_lastinputprice, "
            "if(length(coalesce(g.f_adg, '')) > 0, g.f_adg, gr.f_adgcode) as f_adgt "
            "from c_goods g "
            "left join c_units u on u.f_id = g.f_unit "
            "left join c_groups gr on gr.f_id = g.f_group "
            "left join c_goods_multiscancode m on m.f_goods = g.f_id "
            "where g.f_scancode = :barcode or m.f_id = :barcode "
            "limit 1");
    if (!db.nextRow()) {
        return false;
    }

    g = GoodsItem();
    g.id = db.getInt("f_id");
    g.name = db.getString("f_name");
    g.unitName = db.getString("f_unit_name");
    g.lastInputPrice = db.getDouble("f_lastinputprice");
    g.adgt = db.getString("f_adgt");
    g.barcode = barcode;
    return g.id > 0;
}

} // namespace

C5StoreOutput::C5StoreOutput(C5User *user, const QString &title, QIcon icon, QWidget *parent)
    : C5Widget(parent)
    , ui(new Ui::C5StoreOutput)
{
    mUser = user;
    ui->setupUi(this);
    fLabel = title;
    fIcon = icon;
    ui->wOutputStore->selectorCallback = storeItemSelector;
    ui->wSearchInDocs->setVisible(false);
    QMap<int, int> colwidths = {{col_rec_in_id, 0},
                                {col_goods_id, 0},
                                {col_goods_name, 350},
                                {col_goods_qty, 80},
                                {col_goods_unit, 80},
                                {col_price, 80},
                                {col_total, 80},
                                {col_valid_date, 80},
                                {col_comment, 300},
                                {col_remain, 80},
                                {col_adgt, 50}};

    for (auto [col, width] : colwidths.asKeyValueRange()) {
        ui->tblGoods->setColumnWidth(col, width);
    }

    ui->btnNewGoods->setEnabled(mUser->check(cp_t6_goods));
    ui->btnEditGoods->setEnabled(mUser->check(cp_t6_goods));
    ui->leScancode->setVisible(!C5Config::noScanCodeStore());
    fFocusNextChild = false;
    fCanChangeFocus = true;

    ui->deDate->setEnabled(mUser->check(cp_t1_allow_change_store_doc_date));

    if (__c5config.getRegValue("storedoc_storeinput").toBool()) {
        ui->wOutputStore->setCodeAndName(__c5config.getRegValue("storedoc_storeinput_id").toInt(),
                                         __c5config.getRegValue("storedoc_storeinput_name").toString());
    }

    adjustSize();

    connect(AppWebSocket::instance, &AppWebSocket::bMessageReceived, this, [this](const QJsonObject &jdoc) {
        if (jdoc.value("requestId").toString() != mWebSocketRequestId) {
            return;
        }
        if (!jdoc.contains("result")) {
            return;
        }

        QJsonArray arr = jdoc.value("result").toArray();
        GoodsItem g;
        if (!arr.isEmpty()) {
            g = JsonParser<GoodsItem>::fromJson(arr.at(0).toObject());
        } else if (!lookupStoreGoodsByBarcode(mScanBarcode, g)) {
            if (!mScanBarcode.isEmpty()) {
                C5Message::error(tr("Goods not found") + ": " + mScanBarcode);
            }
            return;
        }

        appendScannedGoods(g);
    });
}

C5StoreOutput::~C5StoreOutput()
{
    __mainWindow->removeBroadcastListener(this);
    delete ui;
}

void C5StoreOutput::setDocument(StoreInputDocument doc)
{
    mSemireadyBatches.clear();
    mDocData = doc;
    ui->deDate->setDate(QDateTime::fromString(doc.date, FORMAT_DATETIME_TO_STR_MYSQL).date());
    ui->leDocNum->setText(doc.user_id);
    ui->wOutputStore->setCodeAndName(doc.store_out, doc.store_out_name);
    ui->leComment->setText(doc.comment());
    for (int i = 0; i < doc.items.size(); i++) {
        auto const su = doc.items.at(i);
        int r = addGoods(su.item_id, su.item_name, su.qty, su.unit_name, su.price, su.price * su.qty, su.comment, su.adgt);
        ui->tblGoods->setString(r, col_rec_in_id, su.uuid);
        auto *d = ui->tblGoods->getWidget<C5DateEdit>(r, col_valid_date);
        d->setDate(QDateTime::fromString(su.expire_date, FORMAT_DATETIME_TO_STR_MYSQL).date());
    }
    setState();
}

QToolBar *C5StoreOutput::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        mActionSave = fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDocument()));
        mActionDraft = fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDocument()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New\ndocument"), this, [this]() {
            __mainWindow->addWidget(new C5StoreOutput(mUser, tr("Store output"), QIcon(":/storage.png")));
        });
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printDoc()));
        fToolBar->addAction(QIcon(":/excel.png"),
                            tr("Export\nto Excel"),
                            this,
                            SLOT(exportToExcel()));
        fToolBar->addAction(QIcon(":/barcode.png"),
                            tr("Print\nbarcode"),
                            this,
                            SLOT(printBarcode()));
        fToolBar->addAction(QIcon(":/goods_store.png"),
                            tr("Duplicate as output"),
                            this,
                            SLOT(duplicateOutput()));
        fToolBar->addAction(QIcon(":/goods_store.png"),
                            tr("Duplicate as input"),
                            this,
                            SLOT(duplicateAsInput()));
    }

    return fToolBar;
}

bool C5StoreOutput::removeDoc(QString id, bool showmessage)
{
    if (showmessage) {
        if (C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
            return false;
        }
    }


    return false;
}

bool C5StoreOutput::allowChangeDatabase()
{
    return false;
}

void C5StoreOutput::appendScannedGoods(const GoodsItem &g)
{
    if (g.id <= 0) {
        return;
    }

    int row = addGoods(g.id, g.name, mScanQty, g.unitName, 0, 0, "", g.adgt);
    ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(g.lastInputPrice, 2));
    if (mScanQty > 0.0001) {
        ui->leScancode->setFocus();
    } else {
        ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
    }
}

void C5StoreOutput::addByScancode(const QString &code, const QString &qty, QString price)
{
    Q_UNUSED(price);
    const QString barcode = normalizeStoreBarcode(code);
    ui->leScancode->clear();
    if (barcode.isEmpty()) {
        return;
    }

    mScanBarcode = barcode;
    mScanQty = str_float(qty);

    auto addFromDb = [this]() {
        GoodsItem g;
        if (!lookupStoreGoodsByBarcode(mScanBarcode, g)) {
            C5Message::error(tr("Goods not found") + ": " + mScanBarcode);
            return;
        }
        appendScannedGoods(g);
    };

    if (!AppWebSocket::instance || !AppWebSocket::instance->isConnected()) {
        addFromDb();
        return;
    }

    mWebSocketRequestId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QJsonObject jo;
    jo["command"] = SelectorName<GoodsItem>::value;
    jo["lower_name"] = "";
    jo["barcode"] = barcode;
    jo["requestId"] = mWebSocketRequestId;
    if (!AppWebSocket::instance->sendBinaryMessage(QJsonDocument(jo).toJson())) {
        addFromDb();
    }
}

double C5StoreOutput::total()
{
    return ui->leTotal->getDouble();
}

void C5StoreOutput::setStore(int id, const QString &name)
{
    ui->wOutputStore->setCodeAndName(id, name);
}

void C5StoreOutput::hotKey(const QString &key)
{
    if (key.toLower() == "ctrl+f") {
        ui->wSearchInDocs->setVisible(true);
    } else {
        C5Widget::hotKey(key);
    }
}

bool C5StoreOutput::openDraft(const QString &id, QString &err)
{
    return true;
}

void C5StoreOutput::nextChild()
{
    if (fCanChangeFocus) {
        // focusNextChild();
    } else {
        fCanChangeFocus = true;
    }
}

bool C5StoreOutput::buildDoc(bool requireValidQty)
{
    QString err;
    if (ui->tblGoods->rowCount() == 0) {
        err += tr("Empty document") + "<br>";
    }
    if (ui->wOutputStore->value() == 0) {
        err += tr("Input store not selected") + "<br>";
    }

    if (mDocData.uuid.isEmpty()) {
        mDocData.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
    }
    mDocData.user_id = ui->leDocNum->text();
    mDocData.date = ui->deDate->toMySQLDate(false);
    mDocData.type = DOC_TYPE_STORE_OUTPUT;
    mDocData.create_user = mUser->id();
    mDocData.store_out = ui->wOutputStore->value();
    mDocData.data = {{"comment", ui->leComment->text()},
                     {"create_user", mUser->fullName()},
                     {"create_date", QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)}};
    mDocData.sum = ui->leTotal->getDouble();
    mDocData.items.clear();
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        StoreUser st;
        st.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
        st.item_id = ui->tblGoods->getInteger(i, col_goods_id);
        st.qty = ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble();
        st.price = ui->tblGoods->lineEdit(i, col_price)->getDouble();
        st.expire_date = ui->tblGoods->getWidget<C5DateEdit>(i, col_valid_date)->toMySQLDate(false);
        st.comment = ui->tblGoods->lineEdit(i, col_comment)->text();
        st.row = i;
        mDocData.items.append(st);
        // 4 decimal places: 0.0005 is valid; only reject true zeros when posting
        if (requireValidQty && st.qty < 0.0001) {
            err += tr("Quantity not valid on row #") + QString::number(i + 1) + "<br>";
        }
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return false;
    }
    return true;
}

void C5StoreOutput::setState()
{
    mActionSave->setEnabled(mDocData.status == STORE_DOC_STATUS_DRAFT);
    ui->wtoolbar->setEnabled(mActionSave->isEnabled());
}

void C5StoreOutput::countTotal()
{
    double total = 0, totalQty = 0.0;

    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, col_total)->getDouble();
        totalQty += ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble();
    }

    ui->leTotal->setDouble(total);
    ui->leTotalQty->setDouble(totalQty);
}

bool C5StoreOutput::docCheck(QString &err, int state)
{
    rowsCheck(err);

    if (ui->wOutputStore->value() == 0) {
        err += tr("Input store is not defined") + "<br>";
    }

    return err.isEmpty();
}

void C5StoreOutput::rowsCheck(QString &err)
{
    if (ui->tblGoods->rowCount() == 0) {
        err += tr("Empty document") + "<br>";
        return;
    }

    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble() < 0.0001) {
            err += tr("Row #") + QString::number(i + 1) + tr(" empty qty") + "<br>";
        }
    }
}

int C5StoreOutput::addGoodsRow()
{
    int row = ui->tblGoods->rowCount();
    ui->tblGoods->setRowCount(row + 1);
    ui->tblGoods->setItem(row, col_rec_in_id, new QTableWidgetItem());
    ui->tblGoods->setItem(row, col_goods_id, new QTableWidgetItem());
    ui->tblGoods->setItem(row, col_goods_name, new QTableWidgetItem());
    ui->tblGoods->setItem(row, col_adgt, new QTableWidgetItem());
    ui->tblGoods->setItem(row, col_goods_qty, new QTableWidgetItem());
    C5LineEdit *lqty = ui->tblGoods->createLineEdit(row, col_goods_qty);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 4));
    lqty->fDecimalPlaces = 4;
    lqty->addEventKeys("+-*");
    connect(lqty, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(lqty, SIGNAL(returnPressed()), this, SLOT(focusNextChildren()));
    ui->tblGoods->setItem(row, col_goods_unit, new QTableWidgetItem());
    C5LineEdit *lprice = ui->tblGoods->createLineEdit(row, col_price);
    lprice->setValidator(new QDoubleValidator(0, 100000000, 2));
    lprice->fDecimalPlaces = 3;
    lprice->addEventKeys("+-*");
    connect(lprice, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(lprice, SIGNAL(returnPressed()), this, SLOT(focusNextChildren()));
    C5LineEdit *ltotal = ui->tblGoods->createLineEdit(row, col_total);
    ltotal->setValidator(new QDoubleValidator(0, 100000000, 2));
    ltotal->addEventKeys("+-*");
    connect(ltotal, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(ltotal, SIGNAL(returnPressed()), this, SLOT(focusNextChildren()));
    ui->tblGoods->createWidget<C5DateEdit>(row, col_valid_date);
    ui->tblGoods->createLineEdit(row, col_comment);
    connect(lqty, SIGNAL(textEdited(QString)), this, SLOT(tblQtyChanged(QString)));
    connect(lprice, SIGNAL(textEdited(QString)), this, SLOT(tblPriceChanged(QString)));
    connect(ltotal, SIGNAL(textEdited(QString)), this, SLOT(tblTotalChanged(QString)));
    return row;
}

int C5StoreOutput::addGoods(int goods,
                           const QString &name,
                           double qty,
                           const QString &unit,
                           double price,
                           double total,
                           const QString &comment,
                           const QString &adgt)
{
    int row = addGoodsRow();
    ui->tblGoods->setData(row, col_goods_id, goods);
    ui->tblGoods->setData(row, col_goods_name, name);
    ui->tblGoods->setData(row, col_goods_unit, unit);

    if (qty > 0) {
        ui->tblGoods->lineEdit(row, col_goods_qty)->setDouble(qty);
    }

    if (price > 0) {
        ui->tblGoods->lineEdit(row, col_price)->setDouble(price);
        ui->tblGoods->lineEdit(row, col_total)->setDouble(total);
    }

    ui->tblGoods->lineEdit(row, col_comment)->setText(comment);
    ui->tblGoods->setString(row, col_adgt, adgt);
    ui->tblGoods->setCurrentCell(row, 0);
    return row;
}

void C5StoreOutput::fillFromInventory(const QList<InventoryDiff> &surpluses)
{
    mSemireadyBatches.clear();
    ui->tblGoods->setRowCount(0);

    for (const auto &s : surpluses) {
        // Добавляем строку товара
        // addGoods вернет индекс созданной строки
        int row = addGoods(s.goodsId, s.goodsName, s.qty, s.unitName, s.price, s.qty * s.price, "Inventory deficit", "");

        // Дополнительно можно подкрасить эти строки или поставить фокус
        ui->tblGoods->lineEdit(row, col_comment)->setReadOnly(true);
    }

    countTotal(); // Пересчитываем итоги документа
    ui->leComment->setText(tr("Imported from inventory deficit ") + QDate::currentDate().toString(FORMAT_DATE_TO_STR));
}

void C5StoreOutput::setDocEnabled(bool v)
{
    ui->deDate->setEnabled(v && mUser->check(cp_t1_allow_change_store_doc_date));
    ui->wOutputStore->setEnabled(v);
    ui->wtoolbar->setEnabled(v);

    for (int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for (int c = 0; c < ui->tblGoods->columnCount(); c++) {
            C5LineEdit *l = dynamic_cast<C5LineEdit *>(ui->tblGoods->cellWidget(r, c));

            if (l) {
                l->setEnabled(v);
            }
        }
    }

    if (fToolBar) {
        fToolBar->actions().at(1)->setEnabled(!v);
        fToolBar->actions().at(0)->setEnabled(v);
    }
}

QString C5StoreOutput::makeGoodsTableHtml(const QStringList &headers,
                                         const QList<QStringList> &rows,
                                         const QSet<int> &rightCols)
{
    QString h;
    QTextStream s(&h);
    s << "<table>";
    s << "<thead><tr>";

    for (int i = 0; i < headers.size(); ++i) {
        s << "<th>" << htmlEscape(headers[i]) << "</th>";
    }

    s << "</tr></thead>";
    s << "<tbody>";

    for (const auto &r : rows) {
        s << "<tr>";

        for (int c = 0; c < headers.size(); ++c) {
            const QString cell = (c < r.size() ? r[c] : "");
            const bool isRight = rightCols.contains(c);
            s << "<td" << (isRight ? " class='right'" : "") << ">" << htmlEscape(cell) << "</td>";
        }

        s << "</tr>";
    }

    s << "</tbody></table>";
    return h;
}

QString C5StoreOutput::makeOtherChargesHtml(C5TableWidget *tbl, const QStringList &hdr)
{
    QString h;
    QTextStream s(&h);
    s << "<div class='mt10'><table>";
    s << "<thead><tr>";

    for (auto &x : hdr)
        s << "<th>" << htmlEscape(x) << "</th>";

    s << "</tr></thead><tbody>";

    for (int i = 0; i < tbl->rowCount(); ++i) {
        s << "<tr>";
        s << "<td class='center'>" << (i + 1) << "</td>";
        s << "<td>" << htmlEscape(tbl->getString(i, 1)) << "</td>";
        s << "<td class='right'>" << htmlEscape(float_str(tbl->lineEdit(i, 2)->getDouble(), 2))
          << "</td>";
        s << "</tr>";
    }

    s << "</tbody></table></div>";
    return h;
}

QString C5StoreOutput::makeComplectationInputHtml(const C5LineEdit *code,
                                                 const C5LineEdit *name,
                                                 const C5LineEdit *qty,
                                                 double total,
                                                 double qtyVal,
                                                 const QStringList &hdr)
{
    const double unitPrice = (qtyVal != 0.0 ? (total / qtyVal) : 0.0);
    QString h;
    QTextStream s(&h);
    s << "<div class='mt10'><table>";
    s << "<thead><tr>";

    for (auto &x : hdr)
        s << "<th>" << htmlEscape(x) << "</th>";

    s << "</tr></thead><tbody>";
    s << "<tr>";
    s << "<td class='center'>1</td>";
    s << "<td>" << htmlEscape(code->text()) << "</td>";
    s << "<td>" << htmlEscape(name->text()) << "</td>";
    s << "<td class='right'>" << htmlEscape(qty->text()) << "</td>";
    s << "<td class='right'>" << htmlEscape(float_str(unitPrice, 2)) << "</td>";
    s << "<td class='right'>" << htmlEscape(float_str(total, 2)) << "</td>";
    s << "</tr>";
    s << "</tbody></table></div>";
    return h;
}

void C5StoreOutput::saveDocument()
{
    if (!buildDoc(true)) {
        return;
    }
    mDocData.status = STORE_DOC_STATUS_POSTED;
    QJsonObject jdoc = mDocData.toJson();
    NInterface::query1("/engine/v2/common/store-move/output", mUser->mSessionKey, this, {{"doc", jdoc}}, [this](const QJsonObject) {
        mDocData.version++;
        setState();
        C5Message::info(tr("Saved"));
    });
}

void C5StoreOutput::draftDocument()
{
    if (!buildDoc(false)) {
        return;
    }
    mDocData.status = STORE_DOC_STATUS_DRAFT;
    QJsonObject jdoc = mDocData.toJson();
    NInterface::query1("/engine/v2/common/store-move/output", mUser->mSessionKey, this, {{"doc", jdoc}}, [this](const QJsonObject) {
        mDocData.version++;
        mActionSave->setEnabled(true);
        mActionDraft->setEnabled(true);
        C5Message::info(tr("Saved"));
    });
}

void C5StoreOutput::focusNextChildren()
{
    focusNextChild();
}

void C5StoreOutput::changeCurrencyResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
}

void C5StoreOutput::slotCheckQtyResponse(const QJsonObject &jdoc)
{
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->getInteger(i, col_goods_id) == jdoc["goodsid"].toInt()) {
            ui->tblGoods->setDouble(i, col_remain, jdoc["remain"].toDouble());
        }
    }
}

void C5StoreOutput::getInput()
{
    const auto r = selectStoreDocGoods();
    if (r.isEmpty()) {
        return;
    }

    GoodsItem g = r.first();
    int row = addGoods(g.id, g.name, 0, g.unitName, 0, 0, "", g.adgt);

    ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(g.lastInputPrice, 2));
    ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
}

void C5StoreOutput::removeDocument()
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

void C5StoreOutput::tblAddChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countTotal();
}

void C5StoreOutput::tblQtyChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;

    if (!ui->tblGoods->findWidget(static_cast<QWidget *>(sender()), row, col)) {
        return;
    }

    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void C5StoreOutput::tblPriceChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;

    if (!ui->tblGoods->findWidget(static_cast<QWidget *>(sender()), row, col)) {
        return;
    }

    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
    double d1 = lqty->getDouble();
    double d2 = lprice->getDouble();
    ltotal->setDouble(d1 * d2);
    countTotal();
}

void C5StoreOutput::tblTotalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;

    if (!ui->tblGoods->findWidget(static_cast<QWidget *>(sender()), row, col)) {
        return;
    }

    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);

    if (lqty->getDouble() > 0.0001) {
        lprice->setDouble(ltotal->getDouble() / lqty->getDouble());
    }

    countTotal();
}

void C5StoreOutput::on_btnAddGoods_clicked()
{
    getInput();
}

void C5StoreOutput::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QJsonObject data;

    if (e->getJsonObject(data)) {
        QJsonObject j = data["goods"].toObject();

        if (j["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot add goods without code"));
            return;
        }

        int row = addGoodsRow();
        ui->tblGoods->setData(row, col_goods_id, j["f_id"].toInt());
        ui->tblGoods->setData(row, col_goods_name, j["f_name"].toString() + " " + j["f_scancode"].toString());
        ui->tblGoods->setData(row, col_goods_unit, data["f_unitname"].toString());
        ui->tblGoods->setData(row, col_adgt, j["f_adg"].toString());
        ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
    }

    delete e;
}

void C5StoreOutput::on_leScancode_returnPressed()
{
    QString qty = ui->chLeaveFocusOnBarcode->isChecked() ? "1" : "";
    addByScancode(ui->leScancode->text(), qty, "");

    if (ui->chLeaveFocusOnBarcode->isChecked()) {
        ui->leScancode->setFocus();
    }
}


void C5StoreOutput::on_btnEditGoods_clicked()
{
    int row = ui->tblGoods->currentRow();

    if (row < 0) {
        return;
    }

    if (ui->tblGoods->getInteger(row, col_goods_id) == 0) {
        return;
    }

    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    ep->setId(ui->tblGoods->getInteger(row, col_goods_id));
    QJsonObject data;

    if (e->getJsonObject(data)) {
        QJsonObject j = data["goods"].toObject();

        if (j["f_id"].toInt() == 0) {
            C5Message::error(tr("Cannot change goods without code"));
            return;
        }

        ui->tblGoods->setData(row, col_goods_id, j["f_id"].toInt());
        ui->tblGoods->setData(row, col_goods_name, j["f_name"].toString() + " " + j["f_scancode"].toString());
        ui->tblGoods->setData(row, col_goods_unit, data["f_unitname"].toString());
        ui->tblGoods->setData(row, col_adgt, j["f_adg"].toString());
        ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
    }

    delete e;
}

void C5StoreOutput::on_btnCalculator_clicked()
{
    double v;
    Calculator::get(v, mUser);
}

void C5StoreOutput::on_btnRememberStoreIn_clicked(bool checked)
{
    __c5config.setRegValue("storedoc_storeinput", checked);
}

void C5StoreOutput::on_btnCopyUUID_clicked()
{
    qApp->clipboard()->setText(mDocData.uuid);
}

void C5StoreOutput::on_leSearchInDoc_textChanged(const QString &arg1)
{
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        bool hidden = !arg1.isEmpty();

        if (!arg1.isEmpty()) {
            hidden = !(ui->tblGoods->getString(i, col_goods_name).contains(arg1, Qt::CaseInsensitive)
                       || ui->tblGoods->getString(i, col_goods_id).contains(arg1, Qt::CaseInsensitive));
        }

        ui->tblGoods->setRowHidden(i, hidden);
    }
}

void C5StoreOutput::on_btnCloseSearch_clicked()
{
    ui->wSearchInDocs->setVisible(false);
    on_leSearchInDoc_textChanged("");
}

void C5StoreOutput::on_btnCopyLastAdd_clicked()
{
    countTotal();
}

void C5StoreOutput::on_btnSaveComment_clicked() {}

void C5StoreOutput::on_btnRemoveGoods_clicked()
{
    int r = ui->tblGoods->currentRow();
    if (r < 0) {
        return;
    }
    if (lockedQty(r) > 0.0001) {
        C5Message::error(tr("Remove this quantity via the semiready list"));
        return;
    }
    ui->tblGoods->removeRow(r);
    countTotal();
}

int C5StoreOutput::findGoodsRow(int goodsId) const
{
    for (int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        if (ui->tblGoods->getInteger(i, col_goods_id) == goodsId) {
            return i;
        }
    }
    return -1;
}

double C5StoreOutput::lockedQty(int row) const
{
    QTableWidgetItem *item = ui->tblGoods->item(row, col_goods_id);
    if (!item) {
        return 0;
    }
    return item->data(Qt::UserRole).toDouble();
}

void C5StoreOutput::setLockedQty(int row, double qty)
{
    QTableWidgetItem *item = ui->tblGoods->item(row, col_goods_id);
    if (!item) {
        item = new QTableWidgetItem();
        ui->tblGoods->setItem(row, col_goods_id, item);
    }
    item->setData(Qt::UserRole, qty);
}

void C5StoreOutput::refreshRowTotal(int row)
{
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
    if (!lqty || !lprice || !ltotal) {
        return;
    }
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
}

void C5StoreOutput::applySemireadyBatch(const SemireadyBatch &batch)
{
    for (const SemireadyComponent &c : batch.components) {
        if (c.goodsId <= 0 || c.qty <= 0.0000001) {
            continue;
        }

        int row = findGoodsRow(c.goodsId);
        if (row < 0) {
            row = addGoods(c.goodsId, c.goodsName, c.qty, QString(), 0, 0, QString(), QString());
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

void C5StoreOutput::removeSemireadyBatch(const QString &batchId)
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

void C5StoreOutput::on_btnAddSemiready_clicked()
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

void C5StoreOutput::on_btnSemireadyList_clicked()
{
    DlgInventorySemireadyList dlg(this);
    dlg.setBatches(mSemireadyBatches);
    connect(&dlg, &DlgInventorySemireadyList::removeBatchRequested, this, [this, &dlg](const QString &batchId) {
        removeSemireadyBatch(batchId);
        dlg.setBatches(mSemireadyBatches);
    });
    dlg.exec();
}
