#include "c5storedoc.h"
#include "ui_c5storedoc.h"
#include "c5cache.h"
#include "c5selector.h"
#include "c5printing.h"
#include "c5printpreview.h"
#include "c5editor.h"
#include "c5mainwindow.h"
#include "ce5partner.h"
#include "ce5goods.h"

C5StoreDoc::C5StoreDoc(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5StoreDoc)
{
    ui->setupUi(this);
    fIcon = ":/goods.png";
    fLabel = tr("Store document");
    ui->leStoreInput->setSelector(fDBParams, ui->leStoreInputName, cache_goods_store);
    ui->leStoreOutput->setSelector(fDBParams, ui->leStoreOutputName, cache_goods_store);
    ui->leReason->setSelector(fDBParams, ui->leReasonName, cache_store_reason);
    ui->lePartner->setSelector(fDBParams, ui->lePartnerName, cache_goods_partners);
    ui->tblGoods->setColumnWidths(7, 0, 0, 300, 80, 80, 80, 80);
    ui->tblGoodsStore->setColumnWidths(7, 0, 0, 200, 70, 50, 50, 70);
    ui->btnNewPartner->setVisible(pr(dbParams.at(1), cp_t6_partners));
    ui->btnNewGoods->setVisible(pr(dbParams.at(1), cp_t6_goods));
    ui->leAccepted->setSelector(dbParams, ui->leAcceptedName, cache_users);
    ui->lePassed->setSelector(dbParams, ui->lePassedName, cache_users);
    fInternalId = "";
    fDocState = DOC_STATE_DRAFT;
    ui->leScancode->setVisible(false);
    ui->tblGoodsGroup->viewport()->installEventFilter(this);
    fGroupTableCell = nullptr;
}

C5StoreDoc::~C5StoreDoc()
{
    delete ui;
}

bool C5StoreDoc::openDoc(QString id)
{
    ui->leDocNum->setPlaceholderText("");
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from a_header where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(tr("Invalid document number"));
        return false;
    }
    fInternalId = db.getString("f_id");
    ui->leDocNum->setText(db.getString("f_userid"));
    ui->deDate->setDate(db.getDate("f_date"));
    fDocType = db.getInt("f_type");
    fDocState = db.getInt("f_state");
    ui->lePartner->setValue(db.getString("f_partner"));
    ui->leComment->setText(db.getString("f_comment"));
    ui->leTotal->setDouble(db.getDouble("f_amount"));
    QJsonDocument jd = QJsonDocument::fromJson(db.getString("f_raw").toUtf8());
    QJsonObject jo = jd.object();
    ui->leStoreInput->setValue(jo["f_storein"].toString());
    ui->leStoreOutput->setValue(jo["f_storeout"].toString());
    ui->lePassed->setValue(jo["f_passed"].toString());
    ui->leAccepted->setValue(jo["f_accepted"].toString());
    ui->deInvoiceDate->setDate(QDate::fromString(jo["f_invoicedate"].toString(), "dd.MM.yyyy"));
    ui->leInvoiceNumber->setText(jo["f_invoice"].toString());
    setMode(static_cast<STORE_DOC>(fDocType));
    db[":f_document"] = id;
    db.exec("select d.f_id, d.f_goods, g.f_name, d.f_qty, u.f_name, d.f_price, d.f_total, d.f_reason \
            from a_store_draft d \
            left join c_goods g on g.f_id=d.f_goods \
            left join c_units u on u.f_id=g.f_unit \
            where d.f_document=:f_document ");
    while (db.nextRow()) {
        int row = addGoodsRow();
        ui->leReason->setValue(db.getString("f_reason"));
        ui->tblGoods->setInteger(row, 0, db.getInt(0));
        ui->tblGoods->setInteger(row, 1, db.getInt(1));
        ui->tblGoods->setString(row, 2, db.getString(2));
        ui->tblGoods->lineEdit(row, 3)->setDouble(db.getDouble(3));
        ui->tblGoods->setString(row, 4, db.getString(4));
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble(5));
        ui->tblGoods->lineEdit(row, 6)->setDouble(db.getDouble(6));
    }
    setDocEnabled(fDocState == DOC_STATE_DRAFT);
    return true;
}

void C5StoreDoc::setMode(C5StoreDoc::STORE_DOC sd)
{
    fDocType = sd;
    C5Database db(fDBParams);
    db.startTransaction();
    db[":f_id"] = (fDocType);
    db.exec("select f_counter from a_type where f_id=:f_id for update");
    db.nextRow();
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsInput(), 10, QChar('0')));
        break;
    case DOC_TYPE_STORE_MOVE:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsMove(), 10, QChar('0')));
        break;
    case DOC_TYPE_STORE_OUTPUT:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsOut(), 10, QChar('0')));
        break;
    }

    switch (sd) {
    case sdInput:
        ui->leStoreOutput->setVisible(false);
        ui->leStoreOutputName->setVisible(false);
        ui->lbStoreOutput->setVisible(false);
        ui->leReason->setValue(DOC_REASON_INPUT);
        break;
    case sdOutput:
        ui->leStoreInput->setVisible(false);
        ui->leStoreInputName->setVisible(false);
        ui->lePartner->setVisible(false);
        ui->lePartnerName->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbStoreInput->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(false);
        ui->btnNewPartner->setVisible(false);
        ui->leReason->setValue(DOC_REASON_OUT);
        break;
    case sdMovement:
        ui->lePartner->setVisible(false);
        ui->lePartnerName->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(false);
        ui->btnNewPartner->setVisible(false);
        ui->leReason->setValue(DOC_REASON_MOVE);
        break;
    default:
        break;
    }
    setGoodsPanelHidden(C5Config::getRegValue("showhidegoods").toBool());
}

QToolBar *C5StoreDoc::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDoc()));
        fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDoc()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New\ndocument"), this, SLOT(newDoc()));
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printDoc()));
        fToolBar->addAction(QIcon(":/show_list.png"), tr("Show/Hide\ngoods list"), this, SLOT(showHideGoodsList()));
    }
    return fToolBar;
}

bool C5StoreDoc::removeDoc(const QStringList &dbParams, QString id, bool showmessage)
{
    if (showmessage) {
        if (C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
            return false;
        }
    }
    QString err;
    C5Database db(dbParams);
    db[":f_basedoc"] = id;
    db.exec("select d.f_date, s.f_document from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
            "where f_basedoc=:f_basedoc and f_document<>:f_basedoc and s.f_type=-1");
    while (db.nextRow()) {
        err += QString("No: %1, %2<br>").arg(db.getInt(1)).arg(db.getDate(0).toString(FORMAT_DATE_TO_STR));
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return false;
    }
    db[":f_document"] = id;
    db.exec("delete from a_store where f_document=:f_document");
    db[":f_document"] = id;
    db.exec("delete from a_store_draft where f_document=:f_document");
    db[":f_id"] = id;
    db.exec("delete from a_header where f_id=:f_id");
    return err.isEmpty();
}

bool C5StoreDoc::save(int state, QString &err, bool showMsg)
{
    save(state, err);
    if (showMsg && !err.isEmpty()) {
        err.insert(0, QString("%1 #%2<br>").arg(tr("Document")).arg(ui->leDocNum->text()));
        C5Message::error(err);
        return false;
    }
    return err.isEmpty();
}

bool C5StoreDoc::allowChangeDatabase()
{
    return false;
}

bool C5StoreDoc::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->tblGoodsGroup->viewport()) {
        if (e->type() == QEvent::MouseButtonPress) {
            QCursor c;
            QTableWidgetItem *item = ui->tblGoodsGroup->itemAt(ui->tblGoodsGroup->mapFromGlobal(c.pos()));
            if (!item) {
                return false;
            }
            fGroupTableCell = new TableCell(this, item);
            fGroupTableCell->setMinimumSize(ui->tblGoodsGroup->columnWidth(0), ui->tblGoodsGroup->rowHeight(0));
            fGroupTableCell->setMaximumSize(ui->tblGoodsGroup->columnWidth(0), ui->tblGoodsGroup->rowHeight(0));
            fGroupTableCell->move(ui->tblGoodsGroup->mapToGlobal(c.pos()));
            fGroupTableCell->show();
        } else if(e->type() == QEvent::MouseButtonRelease) {
            if (fGroupTableCell) {
                QTableWidgetItem *oldItem = fGroupTableCell->fOldItem;
                delete fGroupTableCell;
                fGroupTableCell = nullptr;
                QTableWidgetItem *item = ui->tblGoodsGroup->itemAt(ui->tblGoodsGroup->mapFromGlobal(QCursor().pos()));
                if (item) {
                    QTableWidgetItem tempItem;
                    tempItem.setText(oldItem->text());
                    tempItem.setData(Qt::UserRole, oldItem->data(Qt::UserRole));
                    tempItem.setData(Qt::UserRole + 1, oldItem->data(Qt::UserRole + 1));
                    oldItem->setText(item->text());
                    oldItem->setData(Qt::UserRole, item->data(Qt::UserRole));
                    oldItem->setData(Qt::UserRole + 1, item->data(Qt::UserRole + 1));
                    item->setText(tempItem.text());
                    item->setData(Qt::UserRole, tempItem.data(Qt::UserRole));
                    item->setData(Qt::UserRole + 1, tempItem.data(Qt::UserRole + 1));
                    C5Database db(fDBParams);
                    for (int r = 0; r < ui->tblGoodsGroup->rowCount(); r++) {
                        for (int c = 0; c < ui->tblGoodsGroup->columnCount(); c++) {
                            QTableWidgetItem *i = ui->tblGoodsGroup->item(r, c);
                            if (i == nullptr) {
                                continue;
                            }
                            db[":f_order"] = (c + (r * ui->tblGoodsGroup->columnCount()));
                            db.update("c_groups", where_id(i->data(Qt::UserRole).toInt()));
                        }
                    }
                }
            }
        } else if (e->type() == QEvent::MouseMove) {
            if (fGroupTableCell) {
                fGroupTableCell->move(mapFromGlobal(QCursor().pos()));
                return true;
            }
        }
    }
    return C5Widget::eventFilter(o, e);
}

void C5StoreDoc::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, 6)->getDouble();
    }
    ui->leTotal->setDouble(total);
}

bool C5StoreDoc::docCheck(QString &err)
{
    switch (fDocType) {
    case sdInput:
        if (ui->leStoreInput->getInteger() == 0) {
            err += tr("Input store is not defined") + "<br>";
        }
        break;
    case sdOutput:
        if (ui->leStoreOutput->getInteger() == 0) {
            err = tr("Output store is not defined") + "<br>";
        }
        break;
    case sdMovement:
        if (ui->leStoreInput->getInteger() == 0) {
            err += tr("Input store is not defined") + "<br>";
        }
        if (ui->leStoreOutput->getInteger() == 0) {
            err = tr("Output store is not defined") + "<br>";
        }
        if (ui->leStoreInput->getInteger() == ui->leStoreOutput->getInteger() && ui->leStoreInput->getInteger() != 0) {
            err += tr("Input store and output store cannot be same") + "<br>";
        }
        break;
    }
    if (ui->tblGoods->rowCount() == 0) {
        err += tr("Cannot save an emtpy document") + "<br>";
    }
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->lineEdit(i, 3)->getDouble() < 0.001) {
            err += QString("%1 #%2, %3, missing quantity").arg(tr("Row")).arg(i + 1).arg(ui->tblGoods->getString(i, 2)) + "<br>";
        }
    }
    return err.isEmpty();
}

bool C5StoreDoc::save(int state, QString &err)
{
    C5Database db(fDBParams);
    docCheck(err);
    if (ui->leDocNum->isEmpty()) {
        db.startTransaction();
        db[":f_id"] = (fDocType);
        db.exec("select f_counter from a_type where f_id=:f_id for update");
        db.nextRow();
        switch (fDocType) {
        case DOC_TYPE_STORE_INPUT:
            ui->leDocNum->setText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsInput(), 10, QChar('0')));
            break;
        case DOC_TYPE_STORE_MOVE:
            ui->leDocNum->setText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsMove(), 10, QChar('0')));
            break;
        case DOC_TYPE_STORE_OUTPUT:
            ui->leDocNum->setText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsOut(), 10, QChar('0')));
            break;
        }
        db[":f_counter"] = db.getInt(0) + 1;
        if (!db.update("a_type", where_id(fDocType))) {
            C5Message::error(db.fLastError);
        }
        db.commit();
    } else {
        int docnum = ui->leDocNum->getInteger();
        db[":f_id"] = fDocType;
        db[":f_counter"] = docnum;
        db.exec("update a_type set f_counter=:f_counter where f_id=:f_id and f_counter<:f_counter");
    }
    if (DOC_STATE_SAVED == state) {
        if (ui->leReason->getInteger() == 0) {
            err += tr("Reason is not defined")+ "<br>";
        }
    }
    if (!err.isEmpty()) {
        return false;
    }
    if (state == DOC_STATE_SAVED && !ui->wheader->isEnabled()) {
        err += tr("Already saved") + "<br>";
        return false;
    }

    if (state != fDocState) {
        if (state == DOC_STATE_DRAFT && !fInternalId.isEmpty()) {
            db[":f_basedoc"] = fInternalId;
            db.exec("select d.f_date, s.f_document, d.f_userid from a_store s "
                    "inner join a_header d on d.f_id=s.f_document "
                    "where f_basedoc=:f_basedoc and f_document<>:f_basedoc and s.f_type=-1");
            err = "";
            while (db.nextRow()) {
                err += QString("No: %1/%2 %3<br>").arg(db.getInt(1)).arg(db.getString(2)).arg(db.getDate(0).toString(FORMAT_DATE_TO_STR));
            }
            if (!err.isEmpty()) {
                err += tr("This order used in next documents") + "<br>" + err;
                return false;
            }
            switch (fDocType) {
            case DOC_TYPE_STORE_INPUT:
            case DOC_TYPE_STORE_OUTPUT:
            case DOC_TYPE_STORE_MOVE:
                db[":f_document"] = fInternalId;
                db.exec("delete from a_store where f_document=:f_document");
                break;
            }
        }
    }

    QJsonObject jo;
    jo["f_storein"] = ui->leStoreInput->text();
    jo["f_storeout"] = ui->leStoreOutput->text();
    jo["f_passed"] = ui->lePassed->text();
    jo["f_accepted"] = ui->leAccepted->text();
    jo["f_invoicedate"] = ui->deDate->date().toString("dd.MM.yyyy");
    jo["f_invoice"] = ui->leInvoiceNumber->text();
    QJsonDocument jd(jo);
    if (fInternalId.isEmpty()) {
        fInternalId = C5Database::uuid();
        db[":f_id"] = fInternalId;
        db.insert("a_header", false);
    }
    db[":f_userid"] = ui->leDocNum->text();
    db[":f_state"] = state;
    db[":f_type"] = fDocType;
    db[":f_operator"] = __userid;
    db[":f_date"] = ui->deDate->date();
    db[":f_createDate"] = QDate::currentDate();
    db[":f_createTime"] = QTime::currentTime();
    db[":f_partner"] = ui->lePartner->getInteger();
    db[":f_amount"] = ui->leTotal->getDouble();
    db[":f_comment"] = ui->leComment->text();
    db[":f_raw"] = jd.toJson();
    db.update("a_header", where_id(fInternalId));

    QStringList outId;
    if (fDocState == DOC_STATE_DRAFT) {
        if (state == DOC_STATE_SAVED) {
            switch (fDocType) {
            case DOC_TYPE_STORE_INPUT:
                writeInput();
                break;
            case DOC_TYPE_STORE_OUTPUT: {
                double amount = 0;
                if (!writeOutput(ui->deDate->date(), fInternalId, ui->leStoreOutput->getInteger(), amount, outId, err)) {
                    return false;
                }
                db[":f_amount"] = amount;
                db.update("a_header", where_id(fInternalId));
                break;
            }
            case DOC_TYPE_STORE_MOVE: {
                double amount = 0;
                if (!writeOutput(ui->deDate->date(), fInternalId, ui->leStoreOutput->getInteger(), amount, outId, err)) {
                    return false;
                }
                db[":f_amount"] = amount;
                db.update("a_header", where_id(fInternalId));
                C5Database db1(fDBParams);
                C5Database db2(fDBParams);
                foreach (QString recid, outId) {
                    db1[":f_id"] = recid;
                    db1.exec("select * from a_store where f_id=:f_id");
                    db1.nextRow();
                    db2.setBindValues(db1.getBindValues());
                    db2.removeBindValue(":f_id");
                    db2[":f_id"] = C5Database::uuid();
                    db2[":f_document"] = fInternalId;
                    db2[":f_type"] = 1;
                    db2[":f_store"] = ui->leStoreInput->getInteger();
                    db2.insert("a_store", false);
                }
            }
            }
        }
    }

    if (state != fDocState) {
        db[":f_document"] = fInternalId;
        db.exec("delete from a_store_draft where f_document=:f_document");
        for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
            QString draftId = C5Database::uuid();
            ui->tblGoods->setString(i, 0, draftId);
            db[":f_id"] = draftId;
            db[":f_document"] = fInternalId;
            db[":f_goods"] = ui->tblGoods->getInteger(i, 1);
            db[":f_qty"] = ui->tblGoods->lineEdit(i, 3)->getDouble();
            db[":f_price"] = ui->tblGoods->lineEdit(i, 5)->getDouble();
            db[":f_total"] = ui->tblGoods->lineEdit(i, 6)->getDouble();
            db[":f_reason"] = ui->leReason->getInteger();
            db.insert("a_store_draft", false);
        }
    }
    fDocState = state;
    setDocEnabled(fDocState == DOC_STATE_DRAFT);
    countTotal();
    return true;
}

void C5StoreDoc::writeInput()
{
    C5Database db(fDBParams);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        QString id = C5Database::uuid();
        db[":f_id"] = id;
        db[":f_document"] = fInternalId;
        db[":f_store"] = ui->leStoreInput->getInteger();
        db[":f_type"] = 1;
        db[":f_goods"] = ui->tblGoods->getInteger(i, 1);
        db[":f_qty"] = ui->tblGoods->lineEdit(i, 3)->getDouble();
        db[":f_price"] = ui->tblGoods->lineEdit(i, 5)->getDouble();
        db[":f_total"] = ui->tblGoods->lineEdit(i, 6)->getDouble();
        db[":f_base"] = id;
        db[":f_basedoc"] = fInternalId;
        db[":f_reason"] = ui->leReason->getInteger();
        db.insert("a_store", false);
    }
}

bool C5StoreDoc::writeOutput(const QDate &date, QString docNum, int store, double &amount, QStringList &outId, QString &err)
{
    amount = 0;
    C5Database db(fDBParams);
    C5Database dbdoc(fDBParams);
    db.startTransaction();
    QStringList goodsID;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        goodsID << ui->tblGoods->getString(i, 1);
    }
    QList<QList<QVariant> > storeData;
    db[":f_store"] = store;
    db[":f_date"] = date;
    db.exec(QString("select s.f_base, s.f_goods, sum(s.f_qty*s.f_type), s.f_price, sum(s.f_total*s.f_type) "
            "from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
            "where s.f_goods in (%1) and s.f_store=:f_store and d.f_date<=:f_date "
            "group by 1, 2, 4 "
            "having sum(s.f_qty*s.f_type) > 0.001 "
            "for update ").arg(goodsID.join(",")), storeData);
    QList<QMap<QString, QVariant> > queries;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        double qty = ui->tblGoods->lineEdit(i, 3)->getDouble();
        ui->tblGoods->lineEdit(i, 6)->setDouble(0);
        for (int j = 0; j < storeData.count(); j++) {
            if (storeData.at(j).at(1).toInt() == ui->tblGoods->getInteger(i, 1)) {
                if (storeData.at(j).at(2).toDouble() > 0) {
                    if (storeData.at(j).at(2).toDouble() >= qty) {
                        storeData[j][2] = storeData.at(j).at(2).toDouble() - qty;
                        dbdoc[":f_base"] = storeData.at(j).at(0).toString();
                        dbdoc.exec("select f_document from a_store where f_base=:f_base and f_type=1");
                        dbdoc.nextRow();
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docNum;
                        newrec[":f_store"] = store;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = ui->tblGoods->getInteger(i, 1);
                        newrec[":f_qty"] = qty;
                        newrec[":f_price"] = storeData.at(j).at(3).toDouble();
                        newrec[":f_total"] = storeData.at(j).at(3).toDouble() * qty;
                        newrec[":f_base"] = storeData.at(j).at(0).toString();
                        newrec[":f_basedoc"] = dbdoc.getString(0);
                        newrec[":f_reason"] = ui->leReason->getInteger();
                        queries << newrec;
                        amount += storeData.at(j).at(3).toDouble() * qty;
                        ui->tblGoods->lineEdit(i, 6)->setDouble(ui->tblGoods->lineEdit(i, 6)->getDouble() + (storeData.at(j).at(3).toDouble() * qty));
                        ui->tblGoods->lineEdit(i, 5)->setDouble(ui->tblGoods->lineEdit(i, 6)->getDouble() / ui->tblGoods->lineEdit(i, 3)->getDouble());
                        qty = 0;
                    } else {
                        dbdoc[":f_base"] = storeData.at(j).at(0).toInt();
                        dbdoc.exec("select f_document from a_store where f_base=:f_base and f_type=1");
                        dbdoc.nextRow();
                        QMap<QString, QVariant> newrec;
                        newrec[":f_document"] = docNum;
                        newrec[":f_store"] = store;
                        newrec[":f_type"] = -1;
                        newrec[":f_goods"] = ui->tblGoods->getInteger(i, 1);
                        newrec[":f_qty"] = storeData.at(j).at(2).toDouble();
                        newrec[":f_price"] = storeData.at(j).at(3).toDouble();
                        newrec[":f_total"] = storeData.at(j).at(3).toDouble() * storeData.at(j).at(2).toDouble();
                        newrec[":f_base"] = storeData.at(j).at(0).toString();
                        newrec[":f_basedoc"] = dbdoc.getString(0);
                        newrec[":f_reason"] = ui->leReason->getInteger();
                        queries << newrec;
                        ui->tblGoods->lineEdit(i, 6)->setDouble(ui->tblGoods->lineEdit(i, 6)->getDouble() + (storeData.at(j).at(3).toDouble() * storeData.at(j).at(2).toDouble()));
                        ui->tblGoods->lineEdit(i, 5)->setDouble(ui->tblGoods->lineEdit(i, 6)->getDouble() / ui->tblGoods->lineEdit(i, 3)->getDouble());
                        amount += storeData.at(j).at(3).toDouble() * storeData.at(j).at(2).toDouble();
                        qty -= storeData.at(j).at(2).toDouble();
                        storeData[j][2] = 0.0;
                    }
                }
            }
            if (qty < 0.001) {
                break;
            }
        }
        if (qty > 0.001) {
            if (err.isEmpty()) {
                err += tr("No enaugh materials in the store") + "<br>";
            }
            err += QString("%1 - %2").arg(ui->tblGoods->getString(i, 2)).arg(qty);
        }
    }
    if (err.isEmpty()) {
        for (QList<QMap<QString, QVariant> >::const_iterator it = queries.begin(); it != queries.end(); it++) {
            QString newId = C5Database::uuid();
            outId << newId;
            db.setBindValues(*it);
            db[":f_id"] = newId;
            db.insert("a_store", false);
        }
    }
    db.commit();
    return err.isEmpty();
}

int C5StoreDoc::addGoodsRow()
{
    int row = ui->tblGoods->rowCount();
    ui->tblGoods->setRowCount(row + 1);
    ui->tblGoods->setItem(row, 0, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 1, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 2, new QTableWidgetItem());
    C5LineEdit *lqty = ui->tblGoods->createLineEdit(row, 3);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 3));
    ui->tblGoods->setItem(row, 4, new QTableWidgetItem());
    C5LineEdit *lprice = ui->tblGoods->createLineEdit(row, 5);
    lprice->setValidator(new QDoubleValidator(0, 100000000, 3));
    lprice->fDecimalPlaces = 3;
    C5LineEdit *ltotal = ui->tblGoods->createLineEdit(row, 6);
    ltotal->setValidator(new QDoubleValidator(0, 100000000, 2));
    connect(lqty, SIGNAL(textEdited(QString)), this, SLOT(tblQtyChanged(QString)));
    connect(lprice, SIGNAL(textEdited(QString)), this, SLOT(tblPriceChanged(QString)));
    connect(ltotal, SIGNAL(textEdited(QString)), this, SLOT(tblTotalChanged(QString)));
    return row;
}

void C5StoreDoc::setDocEnabled(bool v)
{
    ui->deDate->setEnabled(v);
    ui->leStoreInput->setEnabled(v);
    ui->leStoreOutput->setEnabled(v);
    ui->wtoolbar->setEnabled(v);
    for (int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for (int c = 0; c < ui->tblGoods->columnCount(); c++) {
            C5LineEdit *l = dynamic_cast<C5LineEdit*>(ui->tblGoods->cellWidget(r, c));
            if (l) {
                l->setEnabled(v);
            }
        }
    }
    if (fToolBar) {
        fToolBar->actions().at(1)->setEnabled(!v);
    }
}

void C5StoreDoc::loadGroupsInput()
{
    C5Database db(fDBParams);
    db.exec("select f_id, f_name from c_groups order by f_order, f_name");
    int col = -1, row = 0;
    ui->tblGoodsGroup->clearContents();
    ui->tblGoodsGroup->setRowCount(0);
    while (db.nextRow()) {
        col++;
        if (col > ui->tblGoodsGroup->columnCount() - 1) {
            col = 0;
            row = ui->tblGoodsGroup->addEmptyRow();
        }
        if (row > ui->tblGoodsGroup->rowCount() - 1) {
            row = ui->tblGoodsGroup->addEmptyRow();
        }
        QTableWidgetItem *item = new QTableWidgetItem(db.getString("f_name"));
        item->setData(Qt::UserRole, db.getInt(0));
        ui->tblGoodsGroup->setItem(row, col, item);
    }
}

void C5StoreDoc::loadGoodsInput()
{
    ui->tblGoodsStore->setSortingEnabled(false);
    ui->tblGoodsStore->clearContents();
    ui->tblGoodsStore->setRowCount(0);
    C5Database db(fDBParams);
    db.exec(QString("select g.f_id, g.f_group, g.f_name as f_goodsname, u.f_name as f_unitname, "
              "sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount "
              "from c_goods g "
              "left join a_store s on s.f_goods=g.f_id and s.f_store=%1 "
              "left join a_header d on d.f_id=s.f_document and d.f_date<=%2 "
              "inner join c_groups gg on gg.f_id=g.f_group "
              "inner join c_units u on u.f_id=g.f_unit "
              "group by 1, 2, 3, 4 ")
            .arg(ui->leStoreInput->getInteger())
            .arg(ui->deDate->toMySQLDate()));
    while (db.nextRow()) {
        int row = ui->tblGoodsStore->addEmptyRow();
        ui->tblGoodsStore->setInteger(row, 0, db.getInt(0));
        ui->tblGoodsStore->setInteger(row, 1, db.getInt(1));
        ui->tblGoodsStore->setString(row, 2, db.getString(2));
        ui->tblGoodsStore->setDouble(row, 3, db.getDouble(4));
        ui->tblGoodsStore->setString(row, 4, db.getString(3));
        ui->tblGoodsStore->setDouble(row, 6, db.getDouble(5));
        if (ui->tblGoodsStore->getDouble(row, 3) > 0.0001) {
            ui->tblGoodsStore->setDouble(row, 5, ui->tblGoodsStore->getDouble(row, 6) / ui->tblGoodsStore->getDouble(row, 3));
        }
    }
    ui->tblGoodsStore->setSortingEnabled(true);
}

void C5StoreDoc::loadGoodsOutput()
{
    loadGoods(ui->leStoreOutput->getInteger());
}

void C5StoreDoc::loadGoods(int store)
{
    ui->tblGoodsStore->setSortingEnabled(false);
    ui->tblGoodsStore->clearContents();
    ui->tblGoodsStore->setRowCount(0);
    C5Database db(fDBParams);
    db.exec(QString("select s.f_goods, g.f_group, g.f_name as f_goodsname, u.f_name as f_unitname, "
              "sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount "
              "from c_goods g "
              "left join a_store s on s.f_goods=g.f_id "
              "left join a_header d on d.f_id=s.f_document "
              "inner join c_groups gg on gg.f_id=g.f_group "
              "inner join c_units u on u.f_id=g.f_unit "
              "where s.f_store=%1 and d.f_date<=%2 "
              "group by 1, 2, 3, 4 "
              "having sum(s.f_qty*s.f_type) > 0.001")
            .arg(store)
            .arg(ui->deDate->toMySQLDate()));
    while (db.nextRow()) {
        int row = ui->tblGoodsStore->addEmptyRow();
        ui->tblGoodsStore->setInteger(row, 0, db.getInt(0));
        ui->tblGoodsStore->setInteger(row, 1, db.getInt(1));
        ui->tblGoodsStore->setString(row, 2, db.getString(2));
        ui->tblGoodsStore->setDouble(row, 3, db.getDouble(4));
        ui->tblGoodsStore->setString(row, 4, db.getString(3));
        ui->tblGoodsStore->setDouble(row, 6, db.getDouble(5));
        if (ui->tblGoodsStore->getDouble(row, 3) > 0.0001) {
            ui->tblGoodsStore->setDouble(row, 5, ui->tblGoodsStore->getDouble(row, 6) / ui->tblGoodsStore->getDouble(row, 3));
        }
    }
    ui->tblGoodsStore->setSortingEnabled(true);
}

void C5StoreDoc::setGoodsPanelHidden(bool v)
{
    if (!isVisible()) {
        return;
    }
    C5Config::setRegValue("showhidegoods", v);
    ui->wGoods->setVisible(v);
    if (v) {
        switch (fDocType) {
        case DOC_TYPE_STORE_INPUT:
            loadGroupsInput();
            loadGoodsInput();
            break;
        case DOC_TYPE_STORE_OUTPUT:
        case DOC_TYPE_STORE_MOVE:
            loadGroupsInput();
            loadGoodsOutput();
            break;
        }
        ui->tblGoodsGroup->fitColumnsToWidth(45);
        ui->tblGoodsStore->fitColumnsToWidth(45);
    }
}

void C5StoreDoc::markGoodsComplited()
{
    QColor sel = QColor::fromRgb(245, 135, 157);
    QColor blank = QColor::fromRgb(255, 255, 255);
    QSet<int> goods;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        goods << ui->tblGoods->item(i, 1)->data(Qt::EditRole).toInt();
    }
    for (int r = 0; r < ui->tblGoodsStore->rowCount(); r++) {
        QColor color = goods.contains(ui->tblGoodsStore->item(r, 0)->data(Qt::EditRole).toInt()) ? sel : blank;
        for (int c = 0; c < ui->tblGoodsStore->columnCount(); c++) {
            QTableWidgetItem *i = ui->tblGoodsStore->item(r, c);
            if (i) {
                i->setData(Qt::BackgroundRole, color);
            }
        }
    }
}

void C5StoreDoc::newDoc()
{
    fInternalId = "";
    fDocState = DOC_STATE_DRAFT;
    ui->leDocNum->clear();
    ui->lePartner->setValue(0);
    ui->leComment->clear();
    ui->leStoreInput->setValue(0);
    ui->leStoreOutput->setValue(0);
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    ui->leInvoiceNumber->clear();
    setDocEnabled(true);
    C5Database db(fDBParams);
    db.startTransaction();
    db[":f_id"] = (fDocType);
    db.exec("select f_counter from a_type where f_id=:f_id for update");
    db.nextRow();
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsInput(), 10, QChar('0')));
        break;
    case DOC_TYPE_STORE_MOVE:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsMove(), 10, QChar('0')));
        break;
    case DOC_TYPE_STORE_OUTPUT:
        ui->leDocNum->setPlaceholderText(QString("%1").arg(db.getInt(0) + 1, C5Config::docNumDigitsOut(), 10, QChar('0')));
        break;
    }
    countTotal();
}

void C5StoreDoc::getInput()
{
    QList<QVariant> vals;
    if (!C5Selector::getValue(fDBParams, cache_goods, vals)) {
        return;
    }
    int row = addGoodsRow();
    ui->tblGoods->setString(row, 1, vals.at(0).toString());
    ui->tblGoods->setString(row, 2, vals.at(2).toString());
    ui->tblGoods->setString(row, 4, vals.at(3).toString());
    ui->tblGoods->lineEdit(row, 3)->setFocus();
}

void C5StoreDoc::getOutput()
{
    QString err;
    if (ui->leStoreOutput->getInteger() == 0) {
        err += tr("Output store must be defined") + "<br>";
    }
    if (!err.isEmpty()) {
        C5Message::error(err);
        return;
    }
    QString query = QString("select s.f_goods, gg.f_name as f_groupname, g.f_name as f_goodsname, u.f_name as f_unitname, "
                          "sum(s.f_qty*s.f_type) as f_qty, sum(s.f_total*s.f_type) as f_amount "
                          "from a_store s "
                          "inner join a_header d on d.f_id=s.f_document "
                          "inner join c_goods g on g.f_id=s.f_goods "
                          "inner join c_groups gg on gg.f_id=g.f_group "
                          "inner join c_units u on u.f_id=g.f_unit "
                          "where s.f_store=%1 and d.f_date<=%2 "
                          "group by 1, 2, 3, 4 "
                          "having sum(s.f_qty*s.f_type) > 0.001 ")
            .arg(ui->leStoreOutput->getInteger())
            .arg(ui->deDate->toMySQLDate());
    QList<QVariant> vals;
    QMap<QString, QString> trans;
    trans[":f_goods"] = tr("Code");
    trans[":f_groupname"] = tr("Group");
    trans[":f_goodsname"] = tr("Name");
    trans[":f_unitname"] = tr("Unit");
    trans[":f_qty"] = tr("Qty");
    trans[":f_amount"] = tr("Amount");
    if (!C5Selector::getValues(fDBParams, query, vals, trans)) {
        return;
    }
    int row = addGoodsRow();
    ui->tblGoods->setString(row, 1, vals.at(0).toString());
    ui->tblGoods->setString(row, 2, vals.at(1).toString());
    ui->tblGoods->setString(row, 4, vals.at(3).toString());
    ui->tblGoods->lineEdit(row, 3)->setFocus();
}

void C5StoreDoc::saveDoc()
{
    QString err;
    save(DOC_STATE_SAVED, err, true);
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
}

void C5StoreDoc::draftDoc()
{
    QString err;
    save(DOC_STATE_DRAFT, err, true);
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
}

void C5StoreDoc::removeDocument()
{
    if (removeDoc(fDBParams, fInternalId, true)) {
        __mainWindow->removeTab(this);
    }
}

void C5StoreDoc::printDoc()
{
    if (fInternalId == nullptr) {
        C5Message::error(tr("Document is not saved"));
        return;
    }
    C5Printing p;
    QList<qreal> points;
    QStringList vals;
    p.setSceneParams(2000, 2800, QPrinter::Portrait);

    p.setFontSize(25);
    p.setFontBold(true);
    QString docTypeText;
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        docTypeText = tr("Store input");
        break;
    case DOC_TYPE_STORE_OUTPUT:
        docTypeText = tr("Store output");
        break;
    case DOC_TYPE_STORE_MOVE:
        docTypeText = tr("Store movement");
        break;
    }

    p.ctext(QString("%1 N%2").arg(docTypeText).arg(ui->leDocNum->text()));
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
        p.ltext(ui->leComment->text(), 0);
        p.br();
    }
    p.br();
    p.setFontBold(true);
    points.clear();
    points << 50 << 200;
    vals << tr("Date");
    if (!storeInName.isEmpty()) {
        vals << tr("Store, input");
        points << 400;
    }
    if (!storeOutName.isEmpty()) {
        vals << tr("Store, output");
        points << 400;
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

    points.clear();
    points << 50 << 100 << 200 << 600 << 250 << 250 << 250 << 270;
    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << tr("Goods")
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Total");
    p.setFontBold(true);
    p.tableText(points, vals, p.fLineHeight + 20);
    p.br(p.fLineHeight + 20);
    p.setFontBold(false);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        vals.clear();
        vals << QString::number(i + 1);
        vals << ui->tblGoods->getString(i, 1);
        vals << ui->tblGoods->getString(i, 2);
        vals << ui->tblGoods->lineEdit(i, 3)->text();
        vals << ui->tblGoods->getString(i, 4);
        vals << ui->tblGoods->lineEdit(i, 5)->text();
        vals << ui->tblGoods->lineEdit(i, 6)->text();
        p.tableText(points, vals, p.fLineHeight + 20);
        p.br(p.fLineHeight + 20);
    }
    points.clear();
    points << 1200
           << 500
           << 270;
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

    C5PrintPreview pp(&p, fDBParams);
    pp.exec();
}

void C5StoreDoc::filterGoodsPanel(int group)
{
    for (int i = 0; i < ui->tblGoodsStore->rowCount(); i++) {
        bool h = (group != 0) && (group != ui->tblGoodsStore->getInteger(i, 1));
        ui->tblGoodsStore->setRowHidden(i, h);
    }
}

void C5StoreDoc::showHideGoodsList()
{
    bool v = !ui->wGoods->isVisible();
    setGoodsPanelHidden(v);
}

void C5StoreDoc::tblQtyChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col + 2);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col + 3);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void C5StoreDoc::tblPriceChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col - 2);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col + 1);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
    countTotal();
}

void C5StoreDoc::tblTotalChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;
    if (!ui->tblGoods->findWidget(static_cast<QWidget*>(sender()), row, col)) {
        return;
    }
    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col - 3);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col - 1);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col);
    if (lqty->getDouble() > 0.001) {
        lprice->setDouble(ltotal->getDouble() / lqty->getDouble());
    }
    countTotal();
}

void C5StoreDoc::on_btnAddGoods_clicked()
{
    switch (fDocType) {
    case DOC_TYPE_STORE_INPUT:
        getInput();
        break;
    case DOC_TYPE_STORE_OUTPUT:
    case DOC_TYPE_STORE_MOVE:
        getOutput();
        break;
    }
    markGoodsComplited();
}

void C5StoreDoc::on_btnRemoveGoods_clicked()
{
    int row = ui->tblGoods->currentRow();
    if (row < 0) {
        return;
    }
    if (C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblGoods->item(row, 2)->text()) != QDialog::Accepted) {
        return;
    }
    ui->tblGoods->removeRow(row);
    countTotal();
}

void C5StoreDoc::on_leStoreInput_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
}

void C5StoreDoc::on_leStoreOutput_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    bool v = C5Config::getRegValue("showhidegoods").toBool();
    if (v) {
        setGoodsPanelHidden(v);
    }
}

void C5StoreDoc::on_tblGoodsGroup_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    filterGoodsPanel(item->data(Qt::UserRole).toInt());
}

void C5StoreDoc::on_tblGoodsStore_itemDoubleClicked(QTableWidgetItem *item)
{
    if (fDocState == DOC_STATE_SAVED) {
        C5Message::error(tr("Document is saved, unable to make changes"));
        return;
    }
    int r = item->row();
    if (r < 0) {
        return;
    }
    int row = addGoodsRow();
    ui->tblGoods->setInteger(row, 1, ui->tblGoodsStore->getInteger(r, 0));
    ui->tblGoods->setString(row, 2, ui->tblGoodsStore->getString(r, 2));
    ui->tblGoods->setString(row, 4, ui->tblGoodsStore->getString(r, 4));
    ui->tblGoods->lineEdit(row, 3)->setFocus();
    markGoodsComplited();
}

void C5StoreDoc::on_btnNewPartner_clicked()
{
    CE5Partner *ep = new CE5Partner(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->lePartner->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void C5StoreDoc::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        int row = addGoodsRow();
        ui->tblGoods->setData(row, 1, data.at(0)["f_id"]);
        ui->tblGoods->setData(row, 2, data.at(0)["f_name"]);
        ui->tblGoods->setData(row, 4, data.at(0)["f_unitname"]);
        ui->tblGoods->lineEdit(row, 3)->setFocus();
    }
    delete e;
}

void C5StoreDoc::on_leScancode_returnPressed()
{

}

TableCell::TableCell(QWidget *parent, QTableWidgetItem *item) :
    QLabel(parent, Qt::FramelessWindowHint)
{
    setText(item->text());
    fOldItem = item;
}
