#include "c5storeinput.h"
#include "dlgstoreinputpayment.h"
#include "dlgstoreinputpricewarn.h"
#include <QCoreApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
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
#include <QProcess>
#include <QProgressDialog>
#include <QShortcut>
#include <QSignalBlocker>
#include <QSqlQuery>
#include <QTemporaryFile>
#include <QTextStream>
#include <QTimer>
#include <QUrl>
#include <memory>
#include "appwebsocket.h"
#include "c5codenameselector.h"
#include "c5codenameselectorfunctions.h"
#include "c5config.h"
#include "c5dateedit.h"
#include "c5database.h"
#include "c5editor.h"
#include "c5htmlprint.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5user.h"
#include "c5utils.h"
#include "dlgprintbarcodelabels.h"
#include "calculator.h"
#include "dict_payment_type.h"
#include "store_document_status.h"
#include "ce5goods.h"
#include "ce5partner.h"
#include "format_date.h"
#include "ninterface.h"
#include "c5structtableview.h"
#include "c5storeoutput.h"
#include "c5storemovement.h"
#include "c5storecomplectation.h"
#include "dlgstoreinputxmlgoodsmatch.h"
#include "dlgstoreinputxmlimportoptions.h"
#include "dlgstoreinputxmlinvoicepick.h"
#include "dlgstoreinputxmlpartnerupdate.h"
#include "storeinputxmlimport.h"
#include "struct_goods_item.h"
#include "ui_c5storeinput.h"
#include <QDialog>
#include <QEvent>
#include <QEventLoop>
#include <QMouseEvent>
#include <xlsxdocument.h>

C5StoreInput::C5StoreInput(C5User *user, const QString &title, QIcon icon, QWidget *parent)
    : C5Widget(parent)
    , ui(new Ui::C5StoreInput)
{
    mUser = user;
    ui->setupUi(this);
    fLabel = title;
    fIcon = icon;
    ui->wInputStore->selectorCallback = storeItemSelector;
    ui->wPartner->selectorCallback = partnerItemSelector;
    ui->wSearchInDocs->setVisible(false);
    ui->leParialPaymentAmount->installEventFilter(this);
    ui->leParialPaymentAmount->setText(float_str(0.0, 2));
    connect(ui->leParialPaymentAmount, &QLineEdit::editingFinished, this, &C5StoreInput::onPaidAmountEditingFinished);
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
    ui->tblAdd->setColumnWidths(ui->tblAdd->columnCount(), 0, 300, 80);
    ui->tblRelatedOutput->setColumnWidth(col_related_id, 0);
    ui->tblRelatedOutput->setColumnWidth(col_related_doc_type, 0);
    ui->tblRelatedOutput->setColumnWidth(col_related_type, 100);
    ui->tblRelatedOutput->setColumnWidth(col_related_number, 110);
    ui->tblRelatedOutput->setColumnWidth(col_related_status, 90);
    ui->tblRelatedOutput->setColumnWidth(col_related_date, 110);
    ui->tblRelatedOutput->setColumnWidth(col_related_store_out, 140);
    ui->tblRelatedOutput->setColumnWidth(col_related_store_in, 140);
    ui->tblRelatedOutput->setColumnWidth(col_related_sum, 90);
    mRelatedOutputTabIndex = ui->tw->indexOf(ui->t5);
    ui->tw->setTabText(mRelatedOutputTabIndex, tr("Write-off"));
    ui->deDate->setEnabled(mUser->check(cp_t1_allow_change_store_doc_date));

    if (__c5config.getRegValue("storedoc_storeinput").toBool()) {
        ui->wInputStore
            ->setCodeAndName(__c5config.getRegValue("storedoc_storeinput_id").toInt(),
                             __c5config.getRegValue("storedoc_storeinput_name").toString());
    }
    ui->btnPinDate->setChecked(__c5config.getRegValue("storedoc_pin_date", false).toBool());
    if (ui->btnPinDate->isChecked()) {
        const QDate pinnedDate = __c5config.getRegValue("storedoc_pinned_date").toDate();
        if (pinnedDate.isValid()) {
            ui->deDate->setDate(pinnedDate);
        }
    }

    initPaymentCombo();
    adjustSize();

    connect(AppWebSocket::instance, &AppWebSocket::bMessageReceived, this, [this](const QJsonObject &jdoc) {
        if (jdoc.value("requestId").toString() != mWebSocketRequestId) {
            return;
        }
        if (!jdoc.contains("result"))
            return;

        QJsonArray arr = jdoc.value("result").toArray();
        if (arr.size() > 0) {
            auto const &jo = arr.at(0).toObject();
            GoodsItem g = JsonParser<GoodsItem>::fromJson(jo);
            int row = addGoods(g.id, g.name, 0, g.unitName, 0, 0, "", g.adgt);

            ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(g.lastInputPrice, 2));
            ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
        }
    });
    captureInitialState();
}

C5StoreInput::~C5StoreInput()
{
    __mainWindow->removeBroadcastListener(this);
    delete ui;
}

void C5StoreInput::setDocument(StoreInputDocument doc)
{
    mDocData = doc;
    mDocumentPersisted = !doc.uuid.isEmpty();
    ui->deDate->setDate(QDateTime::fromString(doc.date, FORMAT_DATETIME_TO_STR_MYSQL).date());
    ui->leDocNum->setText(doc.user_id);
    ui->wInputStore->setCodeAndName(doc.store_in, doc.store_in_name);
    ui->wPartner->setCodeAndName(doc.partner, doc.partnerName());
    ui->leComment->setText(doc.comment());
    if (doc.cashbox_id > 0 && doc.payment_type_id > 0) {
        StorePaymentPreset p;
        p.cashboxId = doc.cashbox_id;
        p.cashboxName = doc.cashbox_name;
        p.paymentTypeId = doc.payment_type_id;
        const char *const nm = payment_names.value(doc.payment_type_id);
        p.paymentTypeName = nm ? QCoreApplication::translate("PaymentType", nm) : doc.payment_type_id_name;
        p.currencyId = doc.currency_id > 0 ? doc.currency_id : 1;
        p.currencyName = doc.currency_name.isEmpty() ? tr("Armenian dram") : doc.currency_name;
        addOrSelectPaymentPreset(p);
    } else {
        setPaymentUnpaid();
        if (doc.currency_id > 0) {
            mCurrentPayment.currencyId = doc.currency_id;
            mCurrentPayment.currencyName = doc.currency_name.isEmpty() ? tr("Armenian dram") : doc.currency_name;
        }
    }
    double totalQty = 0;
    for (int i = 0; i < doc.items.size(); i++) {
        auto const su = doc.items.at(i);
        int r = addGoods(su.item_id, su.item_name, su.qty, su.unit_name, su.price, su.price * su.qty, su.comment, su.adgt);
        totalQty += su.qty;
        ui->tblGoods->setString(r, col_rec_in_id, su.uuid);
        auto *d = ui->tblGoods->getWidget<C5DateEdit>(r, col_valid_date);
        d->setDate(QDateTime::fromString(su.expire_date, FORMAT_DATETIME_TO_STR_MYSQL).date());
    }
    ui->leTotal->setData(doc.sum);
    ui->leTotalQty->setDouble(totalQty);
    mPaidAmountUserEdited = true;
    setPaidAmountUi(doc.paid_amount, true);
    setState();
    captureInitialState();
    mRelatedOutputsLoaded = false;
    ui->tblRelatedOutput->setRowCount(0);
}

QToolBar *C5StoreInput::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        mActionSave = fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDocument()));
        mActionDraft = fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDocument()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New\ndocument"), this, [this]() {
            startNewDocument();
        });
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printDoc()));
        fToolBar->addAction(QIcon(":/excel.png"),
                            tr("Export\nto Excel"),
                            this,
                            SLOT(exportToExcel()));
        fToolBar->addAction(QIcon(":/xml.png"),
                            tr("Import\nfrom XML"),
                            this,
                            SLOT(importFromXml()));
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
        fToolBar->addAction(QIcon(":/setting.png"),
                            tr("Price\nwarning"),
                            this,
                            SLOT(onPriceWarnSettings()));
    }

    return fToolBar;
}

bool C5StoreInput::removeDoc(QString id, bool showmessage)
{
    if (showmessage) {
        if (C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
            return false;
        }
    }


    return false;
}

bool C5StoreInput::allowChangeDatabase()
{
    return false;
}

void C5StoreInput::captureInitialState()
{
    mInitialDate = ui->deDate->date();
    mInitialStoreId = ui->wInputStore->value();
    mInitialPartnerId = ui->wPartner->value();
    mInitialCurrencyId = mCurrentPayment.currencyId;
    mInitialCashboxId = mCurrentPayment.cashboxId;
    mInitialPaymentTypeId = mCurrentPayment.paymentTypeId;
    mInitialComment = ui->leComment->text();
    mInitialDocNum = ui->leDocNum->text();
}

bool C5StoreInput::hasUnsavedChanges() const
{
    if (mDocumentPersisted) {
        return false;
    }

    if (ui->tblGoods->rowCount() > 0 || ui->tblAdd->rowCount() > 0) {
        return true;
    }

    if (ui->leComment->text().trimmed() != mInitialComment.trimmed()) {
        return true;
    }

    if (ui->leDocNum->text().trimmed() != mInitialDocNum.trimmed()) {
        return true;
    }

    if (ui->deDate->date() != mInitialDate) {
        return true;
    }

    if (ui->wInputStore->value() != mInitialStoreId) {
        return true;
    }

    if (ui->wPartner->value() != mInitialPartnerId) {
        return true;
    }

    if (mCurrentPayment.currencyId != mInitialCurrencyId) {
        return true;
    }

    if (mCurrentPayment.cashboxId != mInitialCashboxId) {
        return true;
    }

    return mCurrentPayment.paymentTypeId != mInitialPaymentTypeId;
}

double C5StoreInput::goodsRowPrice(int row) const
{
    // Placeholder shows last purchase price as a hint only — never use it as the document price.
    return ui->tblGoods->lineEdit(row, col_price)->getDouble();
}

int C5StoreInput::priceWarnMode() const
{
    return __c5config.getRegValue(QStringLiteral("storedoc_price_warn_mode"), StoreInputPriceWarnOnSave).toInt();
}

int C5StoreInput::priceWarnPercent() const
{
    return qBound(1, __c5config.getRegValue(QStringLiteral("storedoc_price_warn_percent"), 15).toInt(), 100);
}

bool C5StoreInput::priceAllowZero() const
{
    return __c5config.getRegValue(QStringLiteral("storedoc_allow_zero_price"), false).toBool();
}

bool C5StoreInput::priceDeviationExceeded(int row, double price) const
{
    const double oldPrice = str_float(ui->tblGoods->lineEdit(row, col_price)->placeholderText());
    if (oldPrice <= 0.001) {
        return false;
    }
    const double threshold = oldPrice * (priceWarnPercent() / 100.0);
    return qAbs(price - oldPrice) > threshold;
}

bool C5StoreInput::acceptPriceDeviation(int row, double price) const
{
    if (mPriceWarnSuppressed) {
        return true;
    }
    if (!priceDeviationExceeded(row, price)) {
        return true;
    }
    const double oldPrice = str_float(ui->tblGoods->lineEdit(row, col_price)->placeholderText());
    const QString msg = tr("Price difference for %1 is more than %2%. Current: %3, Old: %4. Accept?")
                            .arg(ui->tblGoods->getString(row, col_goods_name))
                            .arg(priceWarnPercent())
                            .arg(float_str(price, 2))
                            .arg(float_str(oldPrice, 2));
    // Nested event loop: block concurrent save/draft while the question is open.
    auto *self = const_cast<C5StoreInput *>(this);
    self->mPriceAskOpen = true;
    const int answer = C5Message::question(msg);
    self->mPriceAskOpen = false;
    return answer == QDialog::Accepted;
}

bool C5StoreInput::confirmPricesForSave()
{
    if (mPriceWarnSuppressed || priceWarnMode() != StoreInputPriceWarnOnSave) {
        return true;
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        if (!acceptPriceDeviation(i, goodsRowPrice(i))) {
            return false;
        }
    }
    return true;
}

void C5StoreInput::syncGoodsSearchCachePrices() const
{
    QHash<int, double> prices;
    for (const StoreUser &item : mDocData.items) {
        if (item.item_id > 0 && item.price > 0.001) {
            prices[item.item_id] = item.price;
        }
    }
    C5StructTableView::updateGoodsLastInputPrices(prices);
}

int C5StoreInput::unsavedCloseChoice() const
{
    return C5Message::question(tr("Document has unsaved changes. Save before closing?"),
                               tr("Save"),
                               tr("Cancel"),
                               tr("Close without saving"));
}

bool C5StoreInput::saveDraftBlocking()
{
    return saveBlocking(STORE_DOC_STATUS_DRAFT);
}

bool C5StoreInput::savePostedBlocking()
{
    return saveBlocking(STORE_DOC_STATUS_POSTED);
}

bool C5StoreInput::saveBlocking(int status)
{
    if (mSaveBusy || mPriceAskOpen) {
        return false;
    }
    mFocusPriceWarnRow = -1;
    mSaveBusy = true;
    struct BusyGuard {
        bool &flag;
        ~BusyGuard() { flag = false; }
    } busyGuard{mSaveBusy};

    if (!confirmPricesForSave()) {
        return false;
    }
    if (!buildDoc()) {
        return false;
    }

    mDocData.status = status;
    const QJsonObject jdoc = mDocData.toJson();
    bool ok = false;
    QEventLoop loop;

    NInterface::query("/engine/v2/common/store-move/input",
                      mUser->mSessionKey,
                      this,
                      {{"doc", jdoc}},
                      [&](const QJsonObject &jo) {
                          mDocData.version = jo.value(QStringLiteral("version")).toInt(mDocData.version + 1);
                          if (jo.contains(QStringLiteral("paid_amount"))) {
                              mDocData.paid_amount = jo.value(QStringLiteral("paid_amount")).toDouble();
                              setPaidAmountUi(mDocData.paid_amount, true);
                          }
                          mDocumentPersisted = true;
                          syncGoodsSearchCachePrices();
                          setState();
                          ok = true;
                          if (mRelatedOutputsLoaded) {
                              loadRelatedOutputs();
                          }
                          loop.quit();
                      },
                      [&](const QJsonObject &jerr) {
                          QString msg = jerr.value(QStringLiteral("message")).toString();
                          if (msg.isEmpty()) {
                              msg = jerr.value(QStringLiteral("errorMessage")).toString();
                          }
                          if (!msg.isEmpty()) {
                              C5Message::error(msg);
                          }
                          loop.quit();
                          return true;
                      },
                      true,
                      60000,
                      false);

    loop.exec();
    return ok;
}

bool C5StoreInput::confirmTabClose()
{
    if (mClosingForNewDocument) {
        return true;
    }

    if (!hasUnsavedChanges()) {
        return true;
    }

    const int choice = unsavedCloseChoice();

    if (choice == QDialog::Rejected) {
        return false;
    }

    if (choice == 2) {
        return true;
    }

    if (mSaveBusy || mPriceAskOpen) {
        return false;
    }
    mFocusPriceWarnRow = -1;
    mSaveBusy = true;
    struct BusyGuard {
        bool &flag;
        ~BusyGuard() { flag = false; }
    } busyGuard{mSaveBusy};

    if (!confirmPricesForSave()) {
        return false;
    }
    if (!buildDoc()) {
        return false;
    }

    mDocData.status = STORE_DOC_STATUS_DRAFT;
    const QJsonObject jdoc = mDocData.toJson();
    NInterface::query1("/engine/v2/common/store-move/input",
                       mUser->mSessionKey,
                       this,
                       {{"doc", jdoc}},
                       [this](const QJsonObject &) {
                           mDocData.version++;
                           mDocumentPersisted = true;
                           syncGoodsSearchCachePrices();
                           setState();
                           __mainWindow->removeTab(this);
                       });
    return false;
}

void C5StoreInput::startNewDocument()
{
    if (hasUnsavedChanges()) {
        const int choice = unsavedCloseChoice();
        if (choice == QDialog::Rejected) {
            return;
        }
        if (choice != 2) {
            if (!saveDraftBlocking()) {
                return;
            }
        }
    }

    C5User *user = mUser;
    QTimer::singleShot(0, __mainWindow, [user]() {
        __mainWindow->addWidget(new C5StoreInput(user, QObject::tr("Store input"), QIcon(":/storage.png")));
    });
    mClosingForNewDocument = true;
    __mainWindow->removeTab(this);
}

bool C5StoreInput::confirmApplicationClose()
{
    if (!hasUnsavedChanges()) {
        return true;
    }

    const int choice = unsavedCloseChoice();

    if (choice == QDialog::Rejected) {
        return false;
    }

    if (choice == 2) {
        return true;
    }

    return saveDraftBlocking();
}


double C5StoreInput::total()
{
    return ui->leTotal->getDouble();
}

void C5StoreInput::setStore(int id, const QString &name)
{
    ui->wInputStore->setCodeAndName(id, name);
}

void C5StoreInput::hotKey(const QString &key)
{
    if (key.toLower() == "ctrl+f") {
        ui->wSearchInDocs->setVisible(true);
    } else {
        C5Widget::hotKey(key);
    }
}

bool C5StoreInput::openDraft(const QString &id, QString &err)
{
    return true;
}

void C5StoreInput::nextChild()
{
    if (fCanChangeFocus) {
        // focusNextChild();
    } else {
        fCanChangeFocus = true;
    }
}

bool C5StoreInput::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->leParialPaymentAmount && event->type() == QEvent::MouseButtonDblClick) {
        if (mDocData.status == STORE_DOC_STATUS_POSTED) {
            editPostedPaidAmount();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

bool C5StoreInput::buildDoc()
{
    QString err;
    if (ui->tblGoods->rowCount() == 0) {
        err += tr("Empty document") + "<br>";
    }
    if (ui->wInputStore->value() == 0) {
        err += tr("Input store not selected") + "<br>";
    }
    if (ui->wPartner->value() == 0) {
        err += tr("Partner not selected") + "<br>";
    }
    if (mCurrentPayment.currencyId <= 0) {
        mCurrentPayment.currencyId = 1;
        mCurrentPayment.currencyName = tr("Armenian dram");
    }
    if (mDocData.uuid.isEmpty()) {
        mDocData.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
    }
    mDocData.user_id = ui->leDocNum->text();
    mDocData.date = ui->deDate->toMySQLDate(false);
    mDocData.type = DOC_TYPE_STORE_INPUT;
    mDocData.create_user = mUser->id();
    mDocData.cashbox_id = mCurrentPayment.cashboxId;
    mDocData.payment_type_id = mCurrentPayment.paymentTypeId;
    mDocData.currency_id = mCurrentPayment.currencyId;
    mDocData.store_in = ui->wInputStore->value();
    mDocData.partner = ui->wPartner->value();
    mDocData.sum = ui->leTotal->getDouble();
    mDocData.paid_amount = paidAmountFromUi();
    if (mDocData.paid_amount < -0.001) {
        err += tr("Paid amount is not valid") + "<br>";
    }
    if (mDocData.paid_amount - mDocData.sum > 0.001) {
        err += tr("Paid amount cannot exceed document total") + "<br>";
    }
    if (mDocData.paid_amount > 0.001) {
        if (mCurrentPayment.cashboxId <= 0) {
            err += tr("Cashbox not selected") + "<br>";
        }
        if (mCurrentPayment.paymentTypeId <= 0) {
            err += tr("Payment type not specified") + "<br>";
        }
    }
    mDocData.data = {{"comment", ui->leComment->text()},
                     {"create_user", mUser->fullName()},
                     {"create_date", QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)},
                     {"paid_amount", mDocData.paid_amount},
                     {"cashbox_id", mCurrentPayment.cashboxId},
                     {"cashbox_name", mCurrentPayment.cashboxName},
                     {"payment_type_id", mCurrentPayment.paymentTypeId},
                     {"payment_type_name", mCurrentPayment.paymentTypeName},
                     {"currency_id", mCurrentPayment.currencyId},
                     {"currency_name", mCurrentPayment.currencyName}};
    mDocData.items.clear();
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        StoreUser st;
        st.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
        st.item_id = ui->tblGoods->getInteger(i, col_goods_id);
        st.qty = ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble();
        st.price = goodsRowPrice(i);
        st.expire_date = ui->tblGoods->getWidget<C5DateEdit>(i, col_valid_date)->toMySQLDate(false);
        st.comment = ui->tblGoods->lineEdit(i, col_comment)->text();
        st.row = i;
        mDocData.items.append(st);

        if (st.qty < 0.001) {
            err += tr("Quantity not valid on row #") + QString::number(i + 1) + "\n";
        }
        if (!priceAllowZero() && st.price <= 0.001) {
            err += tr("Price not valid on row #") + QString::number(i + 1) + "\n";
        }
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return false;
    }
    return true;
}

void C5StoreInput::setState()
{
    if (mActionSave) {
        mActionSave->setEnabled(mDocData.status == STORE_DOC_STATUS_DRAFT);
        ui->wtoolbar->setEnabled(mActionSave->isEnabled());
    }
    updatePaidAmountEditableState();
}

void C5StoreInput::countTotal()
{
    double total = 0, totalQty = 0.0;

    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, col_total)->getDouble();
        totalQty += ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble();
    }

    for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
        total += ui->tblAdd->lineEdit(i, 2)->getDouble();
    }

    ui->leTotal->setDouble(total);
    ui->leTotalQty->setDouble(totalQty);
    if (mDocData.status == STORE_DOC_STATUS_DRAFT) {
        syncPaidAmountWithPayment();
    }
}

bool C5StoreInput::docCheck(QString &err, int state)
{
    rowsCheck(err);

    if (ui->wInputStore->value() == 0) {
        err += tr("Input store is not defined") + "<br>";
    }

    if (state == 1 && ui->wPartner->value() == 0) {
        err += tr("Partner not specified");
    }

    return err.isEmpty();
}

void C5StoreInput::rowsCheck(QString &err)
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

int C5StoreInput::addGoodsRow()
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
    connect(lprice, &QLineEdit::editingFinished, this, &C5StoreInput::onPriceEditingFinished);
    connect(ltotal, SIGNAL(textEdited(QString)), this, SLOT(tblTotalChanged(QString)));
    return row;
}

int C5StoreInput::addGoods(int goods,
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

void C5StoreInput::setDocEnabled(bool v)
{
    ui->deDate->setEnabled(v && mUser->check(cp_t1_allow_change_store_doc_date));
    ui->wInputStore->setEnabled(v);
    ui->wtoolbar->setEnabled(v);

    for (int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for (int c = 0; c < ui->tblGoods->columnCount(); c++) {
            C5LineEdit *l = dynamic_cast<C5LineEdit *>(ui->tblGoods->cellWidget(r, c));

            if (l) {
                l->setEnabled(v);
            }
        }
    }

    for (int r = 0; r < ui->tblAdd->rowCount(); r++) {
        for (int c = 1; c < ui->tblAdd->columnCount(); c++) {
            C5LineEdit *l = dynamic_cast<C5LineEdit *>(ui->tblAdd->cellWidget(r, c));

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

QString C5StoreInput::makeGoodsTableHtml(const QStringList &headers,
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

QString C5StoreInput::makeOtherChargesHtml(C5TableWidget *tbl, const QStringList &hdr)
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

QString C5StoreInput::makeComplectationInputHtml(const C5LineEdit *code,
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

void C5StoreInput::saveDocument()
{
    if (mSaveBusy || mPriceAskOpen) {
        return;
    }
    mFocusPriceWarnRow = -1;
    mSaveBusy = true;
    struct BusyGuard {
        bool &flag;
        ~BusyGuard() { flag = false; }
    } busyGuard{mSaveBusy};

    if (!confirmPricesForSave()) {
        return;
    }
    if (!buildDoc()) {
        return;
    }
    mDocData.status = STORE_DOC_STATUS_POSTED;
    QJsonObject jdoc = mDocData.toJson();
    NInterface::query1("/engine/v2/common/store-move/input", mUser->mSessionKey, this, {{"doc", jdoc}}, [this](const QJsonObject jo) {
        mDocData.version = jo.value(QStringLiteral("version")).toInt(mDocData.version + 1);
        if (jo.contains(QStringLiteral("paid_amount"))) {
            mDocData.paid_amount = jo.value(QStringLiteral("paid_amount")).toDouble();
            setPaidAmountUi(mDocData.paid_amount, true);
        }
        mDocumentPersisted = true;
        syncGoodsSearchCachePrices();
        setState();
        if (mRelatedOutputsLoaded) {
            loadRelatedOutputs();
        }
        C5Message::info(tr("Saved"));
    });
}

void C5StoreInput::draftDocument()
{
    if (mSaveBusy || mPriceAskOpen) {
        return;
    }
    mFocusPriceWarnRow = -1;
    mSaveBusy = true;
    struct BusyGuard {
        bool &flag;
        ~BusyGuard() { flag = false; }
    } busyGuard{mSaveBusy};

    if (!confirmPricesForSave()) {
        return;
    }
    if (!buildDoc()) {
        return;
    }
    mDocData.status = STORE_DOC_STATUS_DRAFT;
    QJsonObject jdoc = mDocData.toJson();
    NInterface::query1("/engine/v2/common/store-move/input", mUser->mSessionKey, this, {{"doc", jdoc}}, [this](const QJsonObject) {
        mDocData.version++;
        mDocumentPersisted = true;
        syncGoodsSearchCachePrices();
        mActionSave->setEnabled(true);
        mActionDraft->setEnabled(true);
        ui->wtoolbar->setEnabled(true);
        if (mRelatedOutputsLoaded) {
            loadRelatedOutputs();
        }
        C5Message::info(tr("Saved"));
    });
}

double C5StoreInput::additionalCost()
{
    double c = 0;

    for (int i = 0; i < ui->tblAdd->rowCount(); i++) {
        c += ui->tblAdd->lineEdit(i, 2)->getDouble();
    }

    return c;
}

double C5StoreInput::additionalCostForEveryGoods()
{
    if (ui->leTotalQty->getDouble() > 0) {
        return additionalCost() / ui->leTotalQty->getDouble();
    }

    return 0;
}

void C5StoreInput::focusNextChildren()
{
    focusNextChild();
}

void C5StoreInput::changeCurrencyResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
}

void C5StoreInput::slotCheckQtyResponse(const QJsonObject &jdoc)
{
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->getInteger(i, col_goods_id) == jdoc["goodsid"].toInt()) {
            ui->tblGoods->setDouble(i, col_remain, jdoc["remain"].toDouble());
        }
    }
}

void C5StoreInput::getInput()
{
    const auto r = selectItem<GoodsItem>(false, false);
    if (r.isEmpty()) {
        return;
    }

    GoodsItem g = r.first();
    int row = addGoods(g.id, g.name, 0, g.unitName, 0, 0, "", g.adgt);

    ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(g.lastInputPrice, 2));
    ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
}

void C5StoreInput::removeDocument()
{
    if (C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
        return;
    }

    // Not saved on server yet — just close the tab.
    if (!mDocumentPersisted || mDocData.uuid.isEmpty()) {
        mClosingForNewDocument = true;
        __mainWindow->removeTab(this);
        return;
    }

    NInterface::query1("/engine/v2/common/store-move/remove",
                       mUser->mSessionKey,
                       this,
                       {{"id", mDocData.uuid}},
                       [this](const QJsonObject &) {
                           C5Message::info(tr("Deleted"));
                           mClosingForNewDocument = true;
                           __mainWindow->removeTab(this);
                       });
}

void C5StoreInput::tblAddChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    countTotal();
}

void C5StoreInput::tblQtyChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;

    if (!ui->tblGoods->findWidget(static_cast<QWidget *>(sender()), row, col)) {
        return;
    }

    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
    ltotal->setDouble(lqty->getDouble() * goodsRowPrice(row));
    countTotal();
}

void C5StoreInput::tblPriceChanged(const QString &arg1)
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

void C5StoreInput::tblTotalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;

    if (!ui->tblGoods->findWidget(static_cast<QWidget *>(sender()), row, col)) {
        return;
    }

    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);

    if (lqty->getDouble() > 0.001) {
        lprice->setDouble(ltotal->getDouble() / lqty->getDouble());
    }

    countTotal();
}

void C5StoreInput::on_btnAddGoods_clicked()
{
    getInput();
}

void C5StoreInput::on_btnNewPartner_clicked()
{
    CE5Partner *ep = new CE5Partner();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QList<QMap<QString, QVariant> > data;

    if (e->getResult(data)) {
        // ui->lePartner->setValue(data.at(0)["f_id"].toString());
    }

    delete e;
}

void C5StoreInput::on_btnNewGoods_clicked()
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

void C5StoreInput::on_leScancode_returnPressed()
{
    QString qty = ui->chLeaveFocusOnBarcode->isChecked() ? "1" : "";
    if (ui->chLeaveFocusOnBarcode->isChecked()) {
        ui->leScancode->setFocus();
    }

    QString text = ui->leScancode->text().trimmed();
    mWebSocketRequestId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    ui->leScancode->clear();
    QJsonObject jo;
    jo["command"] = SelectorName<GoodsItem>::value;
    jo["lower_name"] = "";
    jo["barcode"] = text;
    jo["requestId"] = mWebSocketRequestId;
    qDebug() << "Sending request" << jo;
    if(!AppWebSocket::instance) {
        C5Message::error(tr("WebSocket is not initialized. Restart the application."));
        return;
    }
    if(!AppWebSocket::instance->isConnected()) {
        C5Message::error(tr("No connection to the server. Check the network and try again."));
        return;
    }
    if(!AppWebSocket::instance->sendBinaryMessage(QJsonDocument(jo).toJson())) {
        C5Message::error(tr("Failed to send the barcode request. Check the connection to the server."));
    }
}

void C5StoreInput::on_btnAddAdd_clicked()
{
    int row = ui->tblAdd->rowCount();
    ui->tblAdd->setRowCount(row + 1);
    C5LineEdit *l = ui->tblAdd->createLineEdit(row, 1);
    l->setFocus();
    l = ui->tblAdd->createLineEdit(row, 2);
    l->setValidator(new QDoubleValidator());
    connect(l, SIGNAL(textEdited(QString)), this, SLOT(tblAddChanged(QString)));
    countTotal();
}

void C5StoreInput::on_btnRemoveAdd_clicked()
{
    int row = ui->tblAdd->currentRow();

    if (row < 0) {
        return;
    }

    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblAdd->lineEdit(row, 2)->text()) != QDialog::Accepted) {
        return;
    }

    ui->tblAdd->removeRow(row);
    countTotal();
}

void C5StoreInput::on_btnEditGoods_clicked()
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

void C5StoreInput::on_btnCalculator_clicked()
{
    double v;
    Calculator::get(v, mUser);
}

void C5StoreInput::on_btnRememberStoreIn_clicked(bool checked)
{
    __c5config.setRegValue("storedoc_storeinput", checked);
}

void C5StoreInput::on_btnCopyUUID_clicked()
{
    qApp->clipboard()->setText(mDocData.uuid);
}

void C5StoreInput::on_leSearchInDoc_textChanged(const QString &arg1)
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

void C5StoreInput::on_btnCloseSearch_clicked()
{
    ui->wSearchInDocs->setVisible(false);
    on_leSearchInDoc_textChanged("");
}

void C5StoreInput::on_btnChangePartner_clicked() {}

void C5StoreInput::on_btnFixPartner_clicked(bool checked)
{
    __c5config.setRegValue("storedoc_fixpartner", checked ? ui->wPartner->value() : 0);
}

void C5StoreInput::on_btnCopyLastAdd_clicked()
{
    countTotal();
}

void C5StoreInput::on_btnSaveComment_clicked() {}

void C5StoreInput::on_btnRemoveGoods_clicked()
{
    int r = ui->tblGoods->currentRow();
    ui->tblGoods->removeRow(r);
}

void C5StoreInput::fillFromInventory(const QList<InventoryDiff> &surpluses)
{
    // Очищаем таблицу перед импортом
    ui->tblGoods->setRowCount(0);

    for (const auto &s : surpluses) {
        // Добавляем строку товара
        // addGoods вернет индекс созданной строки
        int row = addGoods(s.goodsId, s.goodsName, s.qty, s.unitName, s.price, s.qty * s.price, "Inventory surplus", "");

        // Дополнительно можно подкрасить эти строки или поставить фокус
        ui->tblGoods->lineEdit(row, col_comment)->setReadOnly(true);
    }

    countTotal(); // Пересчитываем итоги документа
    ui->leComment->setText(tr("Imported from inventory surplus ") + QDate::currentDate().toString(FORMAT_DATE_TO_STR));
}

void C5StoreInput::on_btnPinDate_clicked(bool checked)
{
    __c5config.setRegValue("storedoc_pin_date", checked);
    if (checked) {
        __c5config.setRegValue("storedoc_pinned_date", ui->deDate->date());
    }
}

void C5StoreInput::on_tw_currentChanged(int index)
{
    if (index == mRelatedOutputTabIndex) {
        loadRelatedOutputs();
    }
}

void C5StoreInput::on_btnRefreshRelatedOutput_clicked()
{
    loadRelatedOutputs();
}

void C5StoreInput::loadRelatedOutputs()
{
    ui->tblRelatedOutput->setRowCount(0);

    if (!mDocumentPersisted || mDocData.uuid.isEmpty()) {
        return;
    }

    NInterface::query1("/engine/v2/common/store-move/related-outputs",
                       mUser->mSessionKey,
                       this,
                       {{"id", mDocData.uuid}},
                       [this](const QJsonObject &jo) {
                           const QJsonArray rows = jo.value("rows").toArray();
                           ui->tblRelatedOutput->setRowCount(rows.size());
                           for (int i = 0; i < rows.size(); ++i) {
                               const QJsonObject row = rows.at(i).toObject();
                               ui->tblRelatedOutput->setString(i, col_related_id, row.value("f_id").toString());
                               ui->tblRelatedOutput->setString(i, col_related_type, row.value("f_type_name").toString());
                               QString docNum = row.value("f_doc_number").toString();
                               if (docNum.isEmpty()) {
                                   docNum = row.value("f_user_id").toString();
                               }
                               ui->tblRelatedOutput->setString(i, col_related_number, docNum);
                               ui->tblRelatedOutput->setString(i, col_related_status, row.value("f_status_name").toString());
                               ui->tblRelatedOutput->setString(i, col_related_date, row.value("f_doc_date").toString());
                               ui->tblRelatedOutput->setString(i, col_related_store_out, row.value("f_store_out_name").toString());
                               ui->tblRelatedOutput->setString(i, col_related_store_in, row.value("f_store_in_name").toString());
                               ui->tblRelatedOutput->setDouble(i, col_related_sum, row.value("f_sum").toDouble());
                               ui->tblRelatedOutput->setInteger(i, col_related_doc_type, row.value("f_doc_type").toInt());
                           }
                           mRelatedOutputsLoaded = true;
                       });
}

void C5StoreInput::openRelatedDocument(const QString &docId, int docType)
{
    NInterface::query1("/engine/v2/common/store-move/open",
                       mUser->mSessionKey,
                       this,
                       {{"id", docId}},
                       [this, docType](const QJsonObject &jo) {
                           Q_UNUSED(docType);
                           const QJsonObject docJo = jo.value("doc").toObject();
                           StoreInputDocument sid = JsonParser<StoreInputDocument>::fromJson(docJo);
                           switch (sid.type) {
                           case DOC_TYPE_STORE_MOVE: {
                               auto *sw = new C5StoreMovement(mUser, tr("Store movement"), QIcon(":/storage.png"));
                               __mainWindow->addWidget(sw);
                               sw->setDocument(sid);
                               break;
                           }
                           case DOC_TYPE_STORE_COMPLECTATION: {
                               auto *sw = new C5StoreComplectation(mUser, tr("Store complectation"), QIcon(":/storage.png"));
                               __mainWindow->addWidget(sw);
                               sw->setDocument(docJo);
                               break;
                           }
                           case DOC_TYPE_STORE_INPUT: {
                               auto *sw = new C5StoreInput(mUser, tr("Store input"), QIcon(":/storage.png"));
                               __mainWindow->addWidget(sw);
                               sw->setDocument(sid);
                               break;
                           }
                           case DOC_TYPE_STORE_OUTPUT: {
                               auto *sw = new C5StoreOutput(mUser, tr("Store output"), QIcon(":/storage.png"));
                               __mainWindow->addWidget(sw);
                               sw->setDocument(sid);
                               break;
                           }
                           default:
                               C5Message::error(tr("Unsupported document type"));
                               break;
                           }
                       });
}

void C5StoreInput::on_tblRelatedOutput_cellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row < 0) {
        return;
    }

    const QString docId = ui->tblRelatedOutput->getString(row, col_related_id);
    if (docId.isEmpty()) {
        return;
    }

    const int docType = ui->tblRelatedOutput->getInteger(row, col_related_doc_type);
    openRelatedDocument(docId, docType);
}

void C5StoreInput::printBarcode()
{
    if (ui->tblGoods->rowCount() == 0) {
        C5Message::info(tr("Nothing to print"));
        return;
    }

    const int currencyId = mCurrentPayment.currencyId > 0
            ? mCurrentPayment.currencyId
            : __c5config.getValue(param_default_currency).toInt();

    QList<BarcodeLabelItem> items;
    C5Database db;
    for (int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        const int goodsId = ui->tblGoods->getInteger(i, col_goods_id);
        if (goodsId <= 0) {
            continue;
        }
        const double qty = ui->tblGoods->lineEdit(i, col_goods_qty)
                ? ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble()
                : 0.0;
        const int labelQty = qMax(1, static_cast<int>(qty));

        db[":f_id"] = goodsId;
        db[":f_currency"] = currencyId;
        db.exec("select g.f_name, g.f_scancode, gpr.f_price1 "
                "from c_goods g "
                "left join c_goods_prices gpr on gpr.f_goods=g.f_id and gpr.f_currency=:f_currency "
                "where g.f_id=:f_id");
        if (!db.nextRow()) {
            continue;
        }
        BarcodeLabelItem it;
        it.name = db.getString("f_name");
        it.barcode = db.getString("f_scancode");
        it.qty = labelQty;
        it.price = float_str(db.getDouble("f_price1"), 2);
        items.append(it);
    }

    DlgPrintBarcodeLabels::printLabels(this, items);
}

void C5StoreInput::importFromXml()
{
    if (mDocData.status != STORE_DOC_STATUS_DRAFT) {
        C5Message::error(tr("Import is available only for draft documents"));
        return;
    }

    const QString path = QFileDialog::getOpenFileName(this, tr("Import from XML"), QString(), tr("XML files (*.xml)"));
    if (path.isEmpty()) {
        return;
    }

    QString error;
    const QVector<StoreInputXmlInvoice> invoices = StoreInputXmlImport::parseFile(path, error);
    if (!error.isEmpty()) {
        C5Message::error(error);
        return;
    }
    if (invoices.isEmpty()) {
        C5Message::error(tr("No invoices found in XML file"));
        return;
    }

    QVector<StoreInputXmlInvoice> selected;
    if (invoices.size() == 1) {
        selected = invoices;
    } else {
        DlgStoreInputXmlInvoicePick pickDlg(invoices, this);
        if (pickDlg.exec() != QDialog::Accepted) {
            return;
        }
        selected = pickDlg.selectedInvoices();
    }
    if (selected.isEmpty()) {
        return;
    }

    StoreInputXmlImportOptions options;
    if (ui->wInputStore->value() > 0) {
        options.storeId = ui->wInputStore->value();
        options.storeName = ui->wInputStore->name();
    }
    if (!DlgStoreInputXmlImportOptions::edit(options, this)) {
        return;
    }

    if (!options.saveImmediately()) {
        setStore(options.storeId, options.storeName);
    }

    // One overall progress dialog; suppress per-request NLoadingDlg for the whole run.
    std::unique_ptr<NInterfaceProgressSuppressor> webSuppress;
    std::unique_ptr<QProgressDialog> progress;
    if (options.saveImmediately()) {
        webSuppress = std::make_unique<NInterfaceProgressSuppressor>();
        progress = std::make_unique<QProgressDialog>(tr("Importing invoices…"), tr("Cancel"), 0, selected.size(), this);
        progress->setWindowModality(Qt::ApplicationModal);
        progress->setMinimumDuration(0);
        progress->setValue(0);
        progress->setLabelText(tr("Preparing…"));
        progress->show();
        QCoreApplication::processEvents();
    }

    QStringList deleteErrors;
    QVector<StoreInputXmlInvoice> toImport;
    toImport.reserve(selected.size());

    for (int selIdx = 0; selIdx < selected.size(); ++selIdx) {
        const StoreInputXmlInvoice &invoice = selected.at(selIdx);
        if (progress) {
            if (progress->wasCanceled()) {
                return;
            }
            progress->setLabelText(tr("Checking %1 (%2 / %3)")
                                       .arg(invoice.docNumber)
                                       .arg(selIdx + 1)
                                       .arg(selected.size()));
            progress->setValue(selIdx);
            QCoreApplication::processEvents();
        }

        QJsonArray existingDocs;
        QString dupError;
        if (!StoreInputXmlImport::findExistingByInvoiceNumber(mUser,
                                                             this,
                                                             invoice.invoiceSeries,
                                                             invoice.invoiceNumber,
                                                             existingDocs,
                                                             dupError)) {
            C5Message::error(dupError);
            return;
        }
        if (existingDocs.isEmpty()) {
            toImport.append(invoice);
            continue;
        }

        if (options.duplicates == StoreInputXmlImportOptions::DupAbort) {
            C5Message::info(tr("Import cancelled: invoice %1 already exists in the database.")
                                .arg(invoice.docNumber));
            return;
        }
        if (options.duplicates == StoreInputXmlImportOptions::DupSkip) {
            continue;
        }

        // DupOverwrite — delete all found documents for this Series+Number
        bool allDeleted = true;
        for (const QJsonValue &v : existingDocs) {
            const QJsonObject doc = v.toObject();
            const QString docId = doc.value(QStringLiteral("f_id")).toVariant().toString();
            QString removeError;
            if (docId.isEmpty() || !StoreInputXmlImport::removeDocument(mUser, this, docId, removeError)) {
                allDeleted = false;
                if (removeError.isEmpty() && docId.isEmpty()) {
                    removeError = tr("Empty document id");
                }
                deleteErrors.append(
                    QStringLiteral("%1  id=%2  date=%3  status=%4  error: %5")
                        .arg(invoice.docNumber,
                             docId,
                             doc.value(QStringLiteral("f_doc_date")).toString(),
                             doc.value(QStringLiteral("f_status_name")).toString(),
                             removeError.isEmpty() ? tr("Unknown error") : removeError));
            }
        }
        if (!allDeleted) {
            continue;
        }
        toImport.append(invoice);
    }

    if (progress) {
        progress->setValue(selected.size());
        QCoreApplication::processEvents();
    }

    if (!deleteErrors.isEmpty()) {
        QTemporaryFile tmp(QDir::temp().filePath(QStringLiteral("store_input_xml_errors_XXXXXX.txt")));
        tmp.setAutoRemove(false);
        if (tmp.open()) {
            QTextStream out(&tmp);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out.setCodec("UTF-8");
#endif
            out << tr("Store input XML import — delete errors") << QLatin1Char('\n');
            out << QString(60, QLatin1Char('-')) << QLatin1Char('\n');
            for (const QString &line : deleteErrors) {
                out << line << QLatin1Char('\n');
            }
            const QString errPath = tmp.fileName();
            tmp.close();
            if (!QProcess::startDetached(QStringLiteral("notepad.exe"), {errPath})) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(errPath));
            }
        } else {
            C5Message::error(tr("Failed to write delete errors:\n%1").arg(deleteErrors.join(QLatin1Char('\n'))));
        }
    }

    if (toImport.isEmpty()) {
        return;
    }

    if (options.saveImmediately()) {
        progress->setMaximum(toImport.size());
        progress->setValue(0);

        QStringList importErrors;
        int savedCount = 0;
        for (int i = 0; i < toImport.size(); ++i) {
            if (progress->wasCanceled()) {
                break;
            }
            const StoreInputXmlInvoice &invoice = toImport.at(i);
            progress->setLabelText(tr("Importing %1 (%2 / %3)")
                                       .arg(invoice.docNumber)
                                       .arg(i + 1)
                                       .arg(toImport.size()));
            progress->setValue(i);
            QCoreApplication::processEvents();

            auto *sw = new C5StoreInput(mUser, tr("Store input"), QIcon(":/storage.png"), this);
            sw->hide();
            sw->setPriceWarnSuppressed(true);
            sw->setStore(options.storeId, options.storeName);
            if (!sw->importInvoiceData(invoice, false, options)) {
                importErrors.append(tr("%1 — import failed").arg(invoice.docNumber));
                delete sw;
                continue;
            }
            const bool saved = (options.saveMode == StoreInputXmlImportOptions::SaveImmediatePosted)
                                   ? sw->savePostedBlocking()
                                   : sw->saveDraftBlocking();
            if (!saved) {
                importErrors.append(tr("%1 — save failed").arg(invoice.docNumber));
                delete sw;
                continue;
            }
            ++savedCount;
            delete sw;
        }
        progress->setValue(toImport.size());
        progress->setLabelText(tr("Done"));
        QCoreApplication::processEvents();

        // Release suppress before showing error UI / notepad
        webSuppress.reset();
        progress.reset();

        if (!importErrors.isEmpty()) {
            QTemporaryFile tmp(QDir::temp().filePath(QStringLiteral("store_input_xml_import_errors_XXXXXX.txt")));
            tmp.setAutoRemove(false);
            if (tmp.open()) {
                QTextStream out(&tmp);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                out.setCodec("UTF-8");
#endif
                out << tr("Store input XML import — errors") << QLatin1Char('\n');
                out << tr("Saved: %1 / %2").arg(savedCount).arg(toImport.size()) << QLatin1Char('\n');
                out << QString(60, QLatin1Char('-')) << QLatin1Char('\n');
                for (const QString &line : importErrors) {
                    out << line << QLatin1Char('\n');
                }
                const QString errPath = tmp.fileName();
                tmp.close();
                if (!QProcess::startDetached(QStringLiteral("notepad.exe"), {errPath})) {
                    QDesktopServices::openUrl(QUrl::fromLocalFile(errPath));
                }
            } else {
                C5Message::error(tr("Saved: %1 / %2\n%3")
                                     .arg(savedCount)
                                     .arg(toImport.size())
                                     .arg(importErrors.join(QLatin1Char('\n'))));
            }
        }
        return;
    }

    if (!importInvoiceData(toImport.first(), true, options)) {
        return;
    }

    for (int i = 1; i < toImport.size(); ++i) {
        auto *sw = new C5StoreInput(mUser, tr("Store input"), QIcon(":/storage.png"));
        __mainWindow->addWidget(sw);
        sw->setStore(options.storeId, options.storeName);
        if (!sw->importInvoiceData(toImport.at(i), false, options)) {
            C5Message::error(tr("Failed to import invoice %1 into a new document").arg(toImport.at(i).docNumber));
        }
    }
}

bool C5StoreInput::importInvoiceData(const StoreInputXmlInvoice &invoice,
                                     bool askReplaceGoods,
                                     const StoreInputXmlImportOptions &options)
{
    const bool prevPriceWarnSuppressed = mPriceWarnSuppressed;
    mPriceWarnSuppressed = true;
    struct PriceWarnRestore {
        C5StoreInput *self;
        bool prev;
        ~PriceWarnRestore() { self->setPriceWarnSuppressed(prev); }
    } priceWarnRestore{this, prevPriceWarnSuppressed};

    if (mDocData.status != STORE_DOC_STATUS_DRAFT) {
        C5Message::error(tr("Import is available only for draft documents"));
        return false;
    }

    if (askReplaceGoods && ui->tblGoods->rowCount() > 0) {
        const int choice = C5Message::question(tr("Document already contains goods lines. Replace existing lines?"),
                                               tr("Replace"),
                                               tr("Cancel"),
                                               tr("Add"));
        if (choice == QDialog::Rejected) {
            return false;
        }
        if (choice == QDialog::Accepted) {
            ui->tblGoods->setRowCount(0);
        }
    }

    if (options.storeId > 0) {
        setStore(options.storeId, options.storeName);
    }

    auto partnerFieldDiffers = [](const QString &left, const QString &right) {
        return StoreInputXmlImport::normalizeName(left).compare(StoreInputXmlImport::normalizeName(right),
                                                                  Qt::CaseInsensitive)
            != 0;
    };

    QString error;
    QJsonObject partner;
    if (!StoreInputXmlImport::findPartnerByTin(mUser, this, invoice.tin, partner, error)) {
        if (!error.contains(QStringLiteral("Partner not found"), Qt::CaseInsensitive)) {
            C5Message::error(error);
            return false;
        }
        if (options.partnerMissing == StoreInputXmlImportOptions::PartnerDoNotCreate) {
            C5Message::error(tr("Partner with TIN %1 was not found. Import aborted for invoice %2.")
                                 .arg(invoice.tin, invoice.docNumber));
            return false;
        }
        error.clear();
        if (!StoreInputXmlImport::createPartner(mUser, this, invoice.tin, invoice.supplierName, invoice.supplierAddress, partner, error)) {
            C5Message::error(error);
            return false;
        }
    } else {
        const bool taxNameDiffers = partnerFieldDiffers(partner.value(QStringLiteral("f_taxname")).toString(), invoice.supplierName);
        const bool addressDiffers = partnerFieldDiffers(partner.value(QStringLiteral("f_address")).toString(), invoice.supplierAddress);
        if (taxNameDiffers || addressDiffers) {
            DlgStoreInputXmlPartnerUpdate partnerDlg(partner, invoice.supplierName, invoice.supplierAddress, this);
            if (partnerDlg.exec() == QDialog::Accepted) {
                partner = partnerDlg.updatedPartner();
                error.clear();
                if (!StoreInputXmlImport::updatePartner(mUser, this, partner, error)) {
                    C5Message::error(error);
                    return false;
                }
            }
        }
    }

    QVector<StoreInputXmlGoodsMappingRow> matchedRows;
    QVector<StoreInputXmlGoodLine> unmatchedLines;
    matchedRows.reserve(invoice.goods.size());
    unmatchedLines.reserve(invoice.goods.size());

    for (const StoreInputXmlGoodLine &line : invoice.goods) {
        GoodsItem goodsItem;
        QString goodsError;
        if (StoreInputXmlImport::findGoodsByExactName(mUser, this, line.description, goodsItem, goodsError)) {
            StoreInputXmlGoodsMappingRow mapped;
            mapped.source = line;
            mapped.goodsId = goodsItem.id;
            mapped.goodsName = goodsItem.name;
            mapped.goodsUnit = goodsItem.unitName;
            mapped.goodsAdgt = goodsItem.adgt;
            matchedRows.append(mapped);
        } else if (!goodsError.isEmpty()) {
            C5Message::error(goodsError);
            return false;
        } else {
            unmatchedLines.append(line);
        }
    }

    if (!unmatchedLines.isEmpty()) {
        const int supplierId = partner.value(QStringLiteral("f_id")).toInt();
        if (options.goodsMissing == StoreInputXmlImportOptions::GoodsAutoCreate) {
            if (!mUser->check(cp_t6_goods)) {
                C5Message::error(mUser->error());
                return false;
            }
            for (const StoreInputXmlGoodLine &line : unmatchedLines) {
                GoodsItem goodsItem;
                QString lookupError;
                if (!StoreInputXmlImport::createGoodsFromXml(mUser, this, supplierId, line, goodsItem, lookupError)) {
                    C5Message::error(lookupError);
                    return false;
                }
                StoreInputXmlGoodsMappingRow mapped;
                mapped.source = line;
                mapped.goodsId = goodsItem.id;
                mapped.goodsName = goodsItem.name;
                mapped.goodsUnit = goodsItem.unitName;
                mapped.goodsAdgt = goodsItem.adgt;
                mapped.createNew = true;
                matchedRows.append(mapped);
            }
        } else {
            DlgStoreInputXmlGoodsMatch goodsDlg(unmatchedLines, this);
            if (goodsDlg.exec() != QDialog::Accepted) {
                return false;
            }
            const QVector<StoreInputXmlGoodsMappingRow> manualRows = goodsDlg.rows();
            for (const StoreInputXmlGoodsMappingRow &row : manualRows) {
                StoreInputXmlGoodsMappingRow mapped = row;
                GoodsItem goodsItem;
                QString lookupError;
                if (mapped.createNew) {
                    if (!mUser->check(cp_t6_goods)) {
                        C5Message::error(mUser->error());
                        return false;
                    }
                    if (!StoreInputXmlImport::createGoodsFromXml(mUser, this, supplierId, mapped.source, goodsItem, lookupError)) {
                        C5Message::error(lookupError);
                        return false;
                    }
                    mapped.goodsId = goodsItem.id;
                    mapped.goodsName = goodsItem.name;
                    mapped.goodsUnit = goodsItem.unitName;
                    mapped.goodsAdgt = goodsItem.adgt;
                    mapped.updateName = false;
                } else if (!StoreInputXmlImport::findGoodsById(mUser, this, mapped.goodsId, goodsItem, lookupError)) {
                    lookupError.clear();
                    if (!StoreInputXmlImport::findGoodsByExactName(mUser, this, mapped.goodsName, goodsItem, lookupError)) {
                        C5Message::error(lookupError.isEmpty()
                                             ? tr("Cannot load goods data for: %1").arg(mapped.goodsName)
                                             : lookupError);
                        return false;
                    }
                    mapped.goodsUnit = goodsItem.unitName;
                    mapped.goodsAdgt = goodsItem.adgt;
                } else {
                    mapped.goodsUnit = goodsItem.unitName;
                    mapped.goodsAdgt = goodsItem.adgt;
                }
                matchedRows.append(mapped);
            }
        }
    }

    for (const StoreInputXmlGoodsMappingRow &row : matchedRows) {
        if (row.updateName) {
            if (!mUser->check(cp_t6_goods)) {
                C5Message::error(mUser->error());
                return false;
            }
            error.clear();
            if (!StoreInputXmlImport::renameGoods(mUser, this, row.goodsId, row.source.description, error)) {
                C5Message::error(error);
                return false;
            }
        }
    }

    ui->wPartner->setCodeAndName(partner.value(QStringLiteral("f_id")).toInt(),
                                 partner.value(QStringLiteral("f_taxname")).toString());
    ui->deDate->setDate(invoice.supplyDate);
    ui->leDocNum->setText(invoice.docNumber);
    QString comment = invoice.additionalData.trimmed();
    if (!comment.isEmpty()) {
        ui->leComment->setText(comment);
    }

    for (const StoreInputXmlGoodsMappingRow &row : matchedRows) {
        GoodsItem goodsItem;
        QString lookupError;
        if (!StoreInputXmlImport::findGoodsById(mUser, this, row.goodsId, goodsItem, lookupError)) {
            goodsItem.id = row.goodsId;
            goodsItem.name = row.updateName ? row.source.description : row.goodsName;
            goodsItem.unitName = row.goodsUnit;
            goodsItem.adgt = row.goodsAdgt;
        } else if (row.updateName) {
            goodsItem.name = row.source.description;
        }

        const StoreInputXmlGoodLine &source = row.source;
        const double total = source.totalPrice > 0.0001 ? source.totalPrice : (source.qty * source.pricePerUnit);
        QString lineComment = source.classifierCode;
        if (!source.unit.isEmpty()) {
            lineComment = lineComment.isEmpty() ? source.unit : lineComment + QStringLiteral(" / ") + source.unit;
        }
        const int tableRow = addGoods(goodsItem.id,
                                      goodsItem.name,
                                      source.qty,
                                      goodsItem.unitName,
                                      source.pricePerUnit,
                                      total,
                                      lineComment,
                                      goodsItem.adgt);
        ui->tblGoods->lineEdit(tableRow, col_price)->setPlaceholderText(float_str(goodsItem.lastInputPrice, 2));
    }

    countTotal();
    return true;
}

void C5StoreInput::initPaymentCombo()
{
    mPaymentPresets.clear();
    setPaymentUnpaid();
}

void C5StoreInput::rebuildPaymentCombo()
{
    const QSignalBlocker blocker(ui->cbPayment);
    ui->cbPayment->clear();
    ui->cbPayment->addItem(tr("Не оплачено"));
    for (const StorePaymentPreset &p : mPaymentPresets) {
        ui->cbPayment->addItem(p.label());
    }
    int idx = 0;
    if (!mCurrentPayment.isUnpaid()) {
        for (int i = 0; i < mPaymentPresets.size(); ++i) {
            if (mPaymentPresets.at(i).matches(mCurrentPayment)) {
                idx = i + 1;
                break;
            }
        }
    }
    ui->cbPayment->setCurrentIndex(idx);
}

void C5StoreInput::setPaymentUnpaid()
{
    mCurrentPayment = StorePaymentPreset();
    mCurrentPayment.currencyId = 1;
    mCurrentPayment.currencyName = tr("Armenian dram");
    mPaidAmountUserEdited = false;
    setPaidAmountUi(0, false);
    rebuildPaymentCombo();
}

void C5StoreInput::applyPaymentPreset(const StorePaymentPreset &preset)
{
    mCurrentPayment = preset;
    if (mCurrentPayment.currencyId <= 0) {
        mCurrentPayment.currencyId = 1;
        mCurrentPayment.currencyName = tr("Armenian dram");
    }
    syncPaidAmountWithPayment();
}

int C5StoreInput::addOrSelectPaymentPreset(const StorePaymentPreset &preset)
{
    if (preset.isUnpaid()) {
        setPaymentUnpaid();
        return 0;
    }
    int found = -1;
    for (int i = 0; i < mPaymentPresets.size(); ++i) {
        if (mPaymentPresets.at(i).matches(preset)) {
            found = i;
            mPaymentPresets[i] = preset;
            break;
        }
    }
    if (found < 0) {
        mPaymentPresets.append(preset);
        found = mPaymentPresets.size() - 1;
    }
    applyPaymentPreset(mPaymentPresets.at(found));
    rebuildPaymentCombo();
    return found + 1;
}

StorePaymentPreset C5StoreInput::currentPaymentPreset() const
{
    return mCurrentPayment;
}

void C5StoreInput::on_btnPaymentConfig_clicked()
{
    StorePaymentPreset draft = mCurrentPayment;
    if (draft.currencyId <= 0) {
        draft.currencyId = 1;
        draft.currencyName = tr("Armenian dram");
    }
    QVector<StorePaymentPreset> presets = mPaymentPresets;
    if (!DlgStoreInputPayment::edit(presets, draft, this)) {
        return;
    }
    mPaymentPresets = presets;
    if (draft.isUnpaid()) {
        setPaymentUnpaid();
        return;
    }
    addOrSelectPaymentPreset(draft);
}

void C5StoreInput::on_cbPayment_currentIndexChanged(int index)
{
    if (index <= 0) {
        mCurrentPayment = StorePaymentPreset();
        mCurrentPayment.currencyId = 1;
        mCurrentPayment.currencyName = tr("Armenian dram");
        mPaidAmountUserEdited = false;
        setPaidAmountUi(0, false);
        return;
    }
    const int presetIndex = index - 1;
    if (presetIndex < 0 || presetIndex >= mPaymentPresets.size()) {
        return;
    }
    applyPaymentPreset(mPaymentPresets.at(presetIndex));
}

double C5StoreInput::paidAmountFromUi() const
{
    return str_float(ui->leParialPaymentAmount->text());
}

void C5StoreInput::setPaidAmountUi(double amount, bool markUserEdited)
{
    if (markUserEdited) {
        mPaidAmountUserEdited = true;
    }
    const QSignalBlocker blocker(ui->leParialPaymentAmount);
    ui->leParialPaymentAmount->setText(float_str(amount, 2));
}

void C5StoreInput::syncPaidAmountWithPayment()
{
    if (mDocData.status == STORE_DOC_STATUS_POSTED) {
        return;
    }
    if (mCurrentPayment.isUnpaid()) {
        setPaidAmountUi(0, false);
        mPaidAmountUserEdited = false;
        return;
    }
    if (!mPaidAmountUserEdited || paidAmountFromUi() < 0.001) {
        setPaidAmountUi(ui->leTotal->getDouble(), false);
        mPaidAmountUserEdited = false;
    }
}

void C5StoreInput::updatePaidAmountEditableState()
{
    const bool posted = mDocData.status == STORE_DOC_STATUS_POSTED;
    ui->leParialPaymentAmount->setReadOnly(posted);
}

void C5StoreInput::editPostedPaidAmount()
{
    if (mDocData.status != STORE_DOC_STATUS_POSTED || mDocData.uuid.isEmpty()) {
        return;
    }
    const double total = ui->leTotal->getDouble();
    bool ok = false;
    const double value = QInputDialog::getDouble(this,
                                                 tr("Partial paid"),
                                                 tr("Paid amount"),
                                                 paidAmountFromUi(),
                                                 0,
                                                 total,
                                                 2,
                                                 &ok);
    if (!ok) {
        return;
    }
    if (value > total + 0.001) {
        C5Message::error(tr("Paid amount cannot exceed document total"));
        return;
    }
    if (value > 0.001) {
        if (mCurrentPayment.cashboxId <= 0 || mCurrentPayment.paymentTypeId <= 0) {
            StorePaymentPreset draft = mCurrentPayment;
            if (draft.currencyId <= 0) {
                draft.currencyId = 1;
                draft.currencyName = tr("Armenian dram");
            }
            QVector<StorePaymentPreset> presets = mPaymentPresets;
            if (!DlgStoreInputPayment::edit(presets, draft, this)) {
                return;
            }
            mPaymentPresets = presets;
            if (draft.isUnpaid()) {
                return;
            }
            addOrSelectPaymentPreset(draft);
        }
    }

    QJsonObject jdoc{
        {QStringLiteral("doc_uuid"), mDocData.uuid},
        {QStringLiteral("doc_version"), mDocData.version},
        {QStringLiteral("doc_create_user"), mUser->id()},
        {QStringLiteral("cashbox_id"), mCurrentPayment.cashboxId},
        {QStringLiteral("payment_type_id"), mCurrentPayment.paymentTypeId},
        {QStringLiteral("currency_id"), mCurrentPayment.currencyId > 0 ? mCurrentPayment.currencyId : 1},
        {QStringLiteral("paid_amount"), value},
    };
    NInterface::query1("/engine/v2/common/store-move/input-paid",
                       mUser->mSessionKey,
                       this,
                       {{"doc", jdoc}},
                       [this, value](const QJsonObject jo) {
                           mDocData.version = jo.value(QStringLiteral("version")).toInt(mDocData.version + 1);
                           mDocData.paid_amount = jo.value(QStringLiteral("paid_amount")).toDouble(value);
                           mDocData.cashbox_id = mCurrentPayment.cashboxId;
                           mDocData.payment_type_id = mCurrentPayment.paymentTypeId;
                           mDocData.currency_id = mCurrentPayment.currencyId;
                           mDocData.data.insert(QStringLiteral("paid_amount"), mDocData.paid_amount);
                           mDocData.data.insert(QStringLiteral("cashbox_id"), mCurrentPayment.cashboxId);
                           mDocData.data.insert(QStringLiteral("payment_type_id"), mCurrentPayment.paymentTypeId);
                           mDocData.data.insert(QStringLiteral("currency_id"), mCurrentPayment.currencyId);
                           setPaidAmountUi(mDocData.paid_amount, true);
                           C5Message::info(tr("Saved"));
                       });
}

void C5StoreInput::onPaidAmountEditingFinished()
{
    if (mDocData.status == STORE_DOC_STATUS_POSTED) {
        return;
    }
    double paid = paidAmountFromUi();
    const double total = ui->leTotal->getDouble();
    if (paid < 0) {
        paid = 0;
    }
    if (paid - total > 0.001) {
        C5Message::error(tr("Paid amount cannot exceed document total"));
        paid = total;
    }
    mPaidAmountUserEdited = true;
    setPaidAmountUi(paid, true);
}

void C5StoreInput::onPriceWarnSettings()
{
    int mode = priceWarnMode();
    int percent = priceWarnPercent();
    bool allowZero = priceAllowZero();
    if (!DlgStoreInputPriceWarn::edit(mode, percent, allowZero, this)) {
        return;
    }
    __c5config.setRegValue(QStringLiteral("storedoc_price_warn_mode"), mode);
    __c5config.setRegValue(QStringLiteral("storedoc_price_warn_percent"), percent);
    __c5config.setRegValue(QStringLiteral("storedoc_allow_zero_price"), allowZero);
}

void C5StoreInput::onPriceEditingFinished()
{
    if (mSaveBusy || mPriceAskOpen || mPriceWarnSuppressed
        || priceWarnMode() != StoreInputPriceWarnOnFocus) {
        return;
    }
    int row = -1;
    int col = -1;
    if (!ui->tblGoods->findWidget(static_cast<QWidget *>(sender()), row, col)) {
        return;
    }
    if (col != col_price || row < 0) {
        return;
    }
    // Defer so Save/Draft from the same mouse click can run first and cancel this warn.
    mFocusPriceWarnRow = row;
    QTimer::singleShot(0, this, [this]() {
        const int row = mFocusPriceWarnRow;
        mFocusPriceWarnRow = -1;
        if (row < 0 || mSaveBusy || mPriceAskOpen || mPriceWarnSuppressed
            || priceWarnMode() != StoreInputPriceWarnOnFocus) {
            return;
        }
        if (row >= ui->tblGoods->rowCount()) {
            return;
        }
        acceptPriceDeviation(row, goodsRowPrice(row));
    });
}

void C5StoreInput::lineEditKeyPressed(const QChar &key)
{
    int row = -1;
    int col = -1;
    auto *le = qobject_cast<C5LineEdit *>(sender());
    if(!le || !ui->tblGoods->findWidget(le, row, col)) {
        return;
    }

    switch(key.toLatin1()) {
    case '+':
        if(col == col_price) {
            const double hint = str_float(le->placeholderText());
            if(hint > 0.001) {
                le->setDouble(hint);
                // Recalc row total and document total (same as typing the price).
                C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
                C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
                if(lqty && ltotal) {
                    ltotal->setDouble(lqty->getDouble() * hint);
                }
                countTotal();
            }
            return;
        }
        on_btnAddGoods_clicked();
        break;

    case '-':
        on_btnRemoveGoods_clicked();
        break;

    case '*': {
        double v = 0;
        if(Calculator::get(v, mUser)) {
            le->setDouble(v);
            if(col == col_price) {
                C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
                C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
                if(lqty && ltotal) {
                    ltotal->setDouble(lqty->getDouble() * v);
                }
                countTotal();
            } else if(col == col_goods_qty) {
                C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
                if(ltotal) {
                    ltotal->setDouble(v * goodsRowPrice(row));
                }
                countTotal();
            } else if(col == col_total) {
                C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
                C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
                if(lqty && lprice && lqty->getDouble() > 0.001) {
                    lprice->setDouble(v / lqty->getDouble());
                }
                countTotal();
            }
        }
        break;
    }
    default:
        break;
    }
}
