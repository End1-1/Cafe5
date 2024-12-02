#include "dlgmovement.h"
#include "ui_dlgmovement.h"
#include "c5lineedit.h"
#include "c5config.h"
#include "c5user.h"
#include <QScrollBar>

DlgMovement::DlgMovement()
    : C5Dialog(__c5config.dbParams(), true)
    , ui(new Ui::DlgMovement)
{
    ui->setupUi(this);
    fState = DOC_STATE_DRAFT;
    fStoreOut = __c5config.defaultStore();
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 300, 200, 80, 80, 80, 0, 80);
    fHttp->createHttpQuery("/engine/shop/movement.php", QJsonObject{
        {"action", "setup"}
    }, SLOT(setupResponse(QJsonObject)));
}

DlgMovement::~DlgMovement()
{
    delete ui;
}

bool DlgMovement::openDoc(const QString &doc)
{
    fUuid = doc;
    C5Database db(__c5config.dbParams());
    db.exec(QString("select sf_open_store_document('{\"f_id\":\"%1\"}')").arg(doc));
    if (!db.nextRow()) {
        C5Message::error(tr("Document not exists"));
        return false;
    }
    QJsonObject reply = QJsonDocument::fromJson(db.getString(0).toUtf8()).object();
    if (reply["status"].toInt() != 1) {
        C5Message::error(reply["data"].toString());
        return false;
    }
    reply = reply["data"].toObject();
    QJsonObject doc_header = reply["doc_header"].toObject();
    QJsonArray doc_body = reply["doc_body"].toArray();
    QJsonObject doc_client = reply["doc_client"].toObject();
    QJsonObject doc_header_store = doc_header["f_body"].toObject();
    if (doc_header_store.isEmpty()) {
        doc_header_store = reply["doc_header_store"].toObject();
    }
    fState = doc_header["f_state"].toInt();
    ui->leComment->setText(doc_header["f_comment"].toString());
    if (fState == DOC_STATE_SAVED) {
        ui->btnSave->setEnabled(false);
    }
    if (__c5config.defaultStore() != doc_header_store["f_storeout"].toInt()) {
        ui->btnSave->setEnabled(false);
    }
    fStoreOut = doc_header_store["f_storeout"].toInt();
    ui->cbDstStore->setCurrentIndex(ui->cbDstStore->findData(doc_header_store["f_storein"].toInt()));
    ui->leComment->setEnabled(ui->btnSave->isEnabled() );
    ui->cbDstStore->setEnabled(ui->btnSave->isEnabled() );
    ui->leSearch->setEnabled(ui->btnSave->isEnabled() );
    ui->btnSaveAndAccept->setEnabled(fState == DOC_STATE_DRAFT
                                     && __c5config.defaultStore() == doc_header_store["f_storein"].toInt());
    for (int i = 0; i < doc_body.size(); i++) {
        const QJsonObject &jo = doc_body.at(i).toObject();
        if (jo["f_type"].toInt() == 1) {
            continue;
        }
        int r = newRow();
        ui->tbl->setString(r, 0, jo["f_id"].toString());
        ui->tbl->setString(r, 1, jo["f_goodsname"].toString());
        ui->tbl->setString(r, 2, jo["f_scancode"].toString());
        auto *l = ui->tbl->lineEdit(r, 4);
        l->setDouble(jo["f_qty"].toDouble());
        l->setEnabled(ui->btnSave->isEnabled());
        ui->tbl->setInteger(r, 5, jo["f_goods"].toInt());
    }
    countQty();
    return true;
}

void DlgMovement::setupResponse(const QJsonObject &jdoc)
{
    QJsonArray jstores = jdoc["availableoutstore"].toArray();
    for (int i = 0; i < jstores.size(); i++) {
        QJsonObject jt = jstores.at(i).toObject();
        ui->cbDstStore->addItem(jt["f_name"].toString(), jt["f_id"].toInt());
    }
    fHttp->httpQueryFinished(sender());
    showMaximized();
}

void DlgMovement::searchResponse(const QJsonObject &jdoc)
{
    QJsonObject jgoods = jdoc["goods"].toObject();
    int r = newRow();
    ui->tbl->setString(r, 1, jgoods["f_name"].toString());
    ui->tbl->setString(r, 2, jgoods["f_scancode"].toString());
    ui->tbl->setDouble(r, 3, jdoc["qty"].toDouble());
    ui->tbl->setInteger(r, 5, jgoods["f_id"].toInt());
    ui->tbl->verticalScrollBar()->setValue(999999999);
    ui->tbl->lineEdit(r, 4)->setFocus();
    fHttp->httpQueryFinished(sender());
}

int DlgMovement::newRow()
{
    int row = ui->tbl->addEmptyRow();
    auto *l = ui->tbl->createLineEdit(row, 4);
    l->setValidator(new QDoubleValidator(0, 999999999, 0));
    auto *btn = new QToolButton();
    btn->setIcon(QIcon(":/delete.png"));
    ui->tbl->setCellWidget(row, 7, btn);
    connect(btn, &QToolButton::clicked, [this, btn]() {
        int r, c;
        if (!ui->tbl->findWidget(btn, r, c)) {
            return;
        }
        ui->tbl->removeRow(r);
    });
    return row;
}

void DlgMovement::countQty()
{
    double qty = 0, amount = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        qty += ui->tbl->lineEdit(i, 4)->getDouble();
        amount += ui->tbl->lineEdit(i, 4)->getDouble() * ui->tbl->getDouble(i, 5);
    }
    ui->leTotalQty->setDouble(qty);
    ui->leTotalAmount->setDouble(amount);
}

bool DlgMovement::saveDoc(int state)
{
    for (int i = 0 ; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->lineEdit(i, 4)->getDouble() < 0.01) {
            C5Message::error(tr("Check quantities"));
            return false;
        }
    }
    if (fUuid.isEmpty()) {
        fUuid = C5Database::uuid();
    }
    QJsonObject jdoc;
    QJsonObject jheader;
    jheader["f_id"] = fUuid;
    jheader["f_userid"] = 0;
    jheader["f_state"] = state;
    jheader["f_type"] = DOC_TYPE_STORE_MOVE;
    jheader["f_date"] = QDate::currentDate().toString(FORMAT_DATE_TO_STR_MYSQL);
    jheader["f_operator"] = __user->id();
    jheader["f_partner"] = QJsonValue::Null;
    jheader["f_amount"] = 0;
    jheader["f_currency"] = 1;
    jheader["f_storein"] = ui->cbDstStore->currentData().toInt();
    jheader["f_storeout"] = fStoreOut;
    jheader["f_reason"] = DOC_REASON_MOVE;
    jheader["f_comment"] = ui->leComment->text();
    jheader["f_paid"] =  0;
    jdoc["header"] = jheader;
    QJsonObject jbody;
    jbody["f_accepted"] = 0;
    jbody["f_passed"] = 0;
    jbody["f_invoice"] = QJsonValue::Null;
    jbody["f_invoicedate"] = QJsonValue::Null;
    jbody["f_storein"] = ui->cbDstStore->currentData().toInt();
    jbody["f_storeout"] = fStoreOut;
    jbody["f_complectationcode"] = 0;
    jbody["f_complectationqty"] = 0;
    jbody["f_cashdoc"] = QJsonValue::Null;
    jbody["f_basedonsale"] = 0;
    jdoc["body"] = jbody;
    QJsonArray jadd;
    jdoc["add"] = jadd;
    QJsonObject jcomplect;
    jcomplect["f_goods"] = 0;
    jcomplect["f_qty"] = 0 ;
    jdoc["complect"] = jcomplect;
    QJsonArray jgoods;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        QJsonObject jitem;
        jitem["f_goods"] = ui->tbl->getInteger(i, 5);
        jitem["f_qty"] = ui->tbl->lineEdit(i, 4)->getDouble();
        jitem["f_price"] = 0;
        jitem["f_row"] = i;
        jgoods.append(jitem);
    }
    jdoc["goods"] = jgoods;
    QJsonObject jpartner;
    jpartner["partner"] = 0;
    jpartner["paid"] =  0;
    jpartner["cash"] = 0;
    jdoc["partner"] = jpartner;
    QString session = C5Database::uuid();
    jdoc["session"] = session;
    QJsonDocument json(jdoc);
    C5Database db(fDBParams);
    if (!db.execDirect(QString("call sf_create_store_document('%1')").arg(QString(json.toJson(QJsonDocument::Compact))))) {
        C5Message::error(db.fLastError);
    }
    db[":f_session"] = session;
    db.exec("select f_result from a_result where f_session=:f_session");
    if (db.nextRow()) {
        QJsonObject js = QJsonDocument::fromJson(db.getString(0).toUtf8()).object();
        if (js["status"].toInt() == 0) {
            C5Message::error(js["message"].toString());
            return false;
        }
    }
    return true;
}

void DlgMovement::on_leSearch_returnPressed()
{
    fHttp->createHttpQuery("/engine/shop/movement.php", QJsonObject{
        {"action", "search"},
        {"store", __c5config.defaultStore()},
        {"barcode", ui->leSearch->text()}
    }, SLOT(searchResponse(QJsonObject)));
    ui->leSearch->clear();
}

void DlgMovement::on_btnSave_clicked()
{
    if (saveDoc(DOC_STATE_DRAFT)) {
        C5Message::info(tr("Saved"));
        close();
    }
}

void DlgMovement::on_btnExit_clicked()
{
    if (C5Message::question(tr("Confirm to exit")) != QDialog::Accepted) {
        return;
    }
    accept();
}

void DlgMovement::on_leFilter_textChanged(const QString &arg1)
{
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        ui->tbl->setRowHidden(i,
                              !ui->tbl->getString(i, 1).contains(arg1, Qt::CaseInsensitive)
                              && !ui->tbl->getString(i, 2).contains(arg1, Qt::CaseInsensitive));
    }
}

void DlgMovement::on_btnSaveAndAccept_clicked()
{
    if (saveDoc(DOC_STATE_SAVED)) {
        C5Message::info(tr("Saved"));
        close();
    }
}

void DlgMovement::on_btnMinimize_clicked()
{
    showMinimized();
}
