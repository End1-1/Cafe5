#include "c5storedoc.h"
#include "ui_c5storedoc.h"
#include "c5cache.h"
#include "c5selector.h"

C5StoreDoc::C5StoreDoc(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5StoreDoc)
{
    ui->setupUi(this);
    fIcon = ":/goods.png";
    fLabel = tr("Store document");
    ui->leStoreInput->setSelector(fDBParams, ui->leStoreInputName, cache_goods_store);
    ui->leStoreOutput->setSelector(fDBParams, ui->leStoreOutputName, cache_goods_store);
    ui->lePartner->setSelector(fDBParams, ui->lePartnerName, cache_goods_partners);
    ui->tblGoods->setColumnWidths(7, 0, 0, 300, 80, 80, 80, 80);
}

C5StoreDoc::~C5StoreDoc()
{
    delete ui;
}

bool C5StoreDoc::openDoc(int id)
{
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from a_header where f_id=:f_id");
    if (!db.nextRow()) {
        C5Message::error(tr("Invalid document number"));
        return false;
    }
    ui->leDocNum->setText(db.getString("f_id"));
    ui->deDate->setDate(db.getDate("f_date"));
    fDocType = db.getInt("f_type");
    fDocState = db.getInt("f_state");
    ui->lePartner->setValue(db.getString("f_partner"));
    ui->leComment->setText(db.getString("f_comment"));
    QJsonDocument jd = QJsonDocument::fromJson(db.getString("f_raw").toUtf8());
    QJsonObject jo = jd.object();
    ui->leStoreInput->setValue(jo["f_storein"].toString());
    ui->leStoreOutput->setValue(jo["f_storeout"].toString());
    db[":f_document"] = id;
    db.exec("select d.f_id, d.f_goods, g.f_name, d.f_qty, u.f_name, d.f_price, d.f_total \
            from a_store_draft d \
            left join c_goods g on g.f_id=d.f_goods \
            left join c_units u on u.f_id=g.f_unit \
            where d.f_document=:f_document ");
    while (db.nextRow()) {
        int row = addGoodsRow();
        ui->tblGoods->setInteger(row, 0, db.getInt(0));
        ui->tblGoods->setInteger(row, 1, db.getInt(1));
        ui->tblGoods->setString(row, 2, db.getString(2));
        ui->tblGoods->lineEdit(row, 3)->setDouble(db.getDouble(3));
        ui->tblGoods->setString(row, 4, db.getString(4));
        ui->tblGoods->lineEdit(row, 5)->setDouble(db.getDouble(5));
        ui->tblGoods->lineEdit(row, 6)->setDouble(db.getDouble(6));
    }
    setMode(static_cast<STORE_DOC>(fDocType));
    setDocEnabled(fDocState == DOC_STATE_DRAFT);
    return true;
}

void C5StoreDoc::setMode(C5StoreDoc::STORE_DOC sd)
{
    fDocType = sd;
    switch (sd) {
    case sdInput:
        ui->leStoreOutput->setVisible(false);
        ui->leStoreOutputName->setVisible(false);
        ui->lbStoreOutput->setVisible(false);
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
        break;
    case sdMovement:
        ui->lePartner->setVisible(false);
        ui->lePartnerName->setVisible(false);
        ui->leInvoiceNumber->setVisible(false);
        ui->deInvoiceDate->setVisible(false);
        ui->lbInvoiceDate->setVisible(false);
        ui->lbInvoiceNumber->setVisible(false);
        ui->lbPartner->setVisible(false);
        break;
    default:
        break;
    }
}

QToolBar *C5StoreDoc::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDoc()));
        fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDoc()));
    }
    return fToolBar;
}

bool C5StoreDoc::removeDoc(int id)
{
    qDebug() << id;
    return true;
}

void C5StoreDoc::countTotal()
{
    double total = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        total += ui->tblGoods->lineEdit(i, 6)->getDouble();
    }
    ui->leTotal->setDouble(total);
}

bool C5StoreDoc::docCheck()
{
    return true;
}

void C5StoreDoc::save(int state)
{
    C5Database db(fDBParams);
    QString err;
    if (!docCheck()) {
        return;
    }
    if (state == DOC_STATE_SAVED && !ui->wheader->isEnabled()) {
        return;
    }
    if (state == DOC_STATE_DRAFT && ui->leDocNum->getInteger() > 0) {
        db[":f_basedoc"] = ui->leDocNum->getInteger();
        db.exec("select d.f_date, s.f_document from a_store s "
                "inner join a_header d on d.f_id=s.f_document "
                "where f_basedoc=:f_basedoc and f_document<>:f_basedoc and s.f_type=-1");
        err = "";
        while (db.nextRow()) {
            err += QString("No: %1 %2<br>").arg(db.getInt(1)).arg(db.getDate(0).toString(FORMAT_DATE_TO_STR));
        }
        if (!err.isEmpty()) {
            C5Message::info(tr("This order used in next documents") + "<br>" + err);
            return;
        }
        switch (fDocType) {
        case DOC_TYPE_STORE_INPUT:
        case DOC_TYPE_STORE_OUTPUT:
            db[":f_document"] = ui->leDocNum->getInteger();
            db.exec("delete from a_store where f_document=:f_document");
            break;
        }
    }

    QJsonObject jo;
    jo["f_storein"] = ui->leStoreInput->text();
    jo["f_storeout"] = ui->leStoreOutput->text();
    QJsonDocument jd(jo);
    if (ui->leDocNum->getInteger() == 0) {
        db[":f_id"] = 0;
        ui->leDocNum->setInteger(db.insert("a_header"));
    }
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
    db.update("a_header", where_id(ui->leDocNum->getInteger()));

    if (state == DOC_STATE_SAVED) {
        switch (fDocType) {
        case DOC_TYPE_STORE_INPUT:
            writeInput();
            break;
        case DOC_TYPE_STORE_OUTPUT: {
            double amount = 0;
            if (!writeOutput(ui->deDate->date(), ui->leDocNum->getInteger(), ui->leStoreOutput->getInteger(), amount, err)) {
                C5Message::error(err);
                return;
            }
            db[":f_amount"] = amount;
            db.update("a_header", where_id(ui->leDocNum->getInteger()));
            break;
        }
        }
        setDocEnabled(false);
    } else {
        setDocEnabled(true);
    }

    db[":f_document"] = ui->leDocNum->getInteger();
    db.exec("delete from a_store_draft where f_document=:f_document");
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_id"] = 0;
        ui->tblGoods->setInteger(i, 0, db.insert("a_store_draft"));
        db[":f_document"] = ui->leDocNum->getInteger();
        db[":f_goods"] = ui->tblGoods->getInteger(i, 1);
        db[":f_qty"] = ui->tblGoods->lineEdit(i, 3)->getDouble();
        db[":f_price"] = ui->tblGoods->lineEdit(i, 5)->getDouble();
        db[":f_total"] = ui->tblGoods->lineEdit(i, 6)->getDouble();
        db.update("a_store_draft", where_id(ui->tblGoods->getInteger(i, 0)));
    }

    C5Message::info(tr("Saved"));
}

void C5StoreDoc::writeInput()
{
    C5Database db(fDBParams);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_id"] = 0;
        int id = db.insert("a_store");
        db[":f_document"] = ui->leDocNum->getInteger();
        db[":f_store"] = ui->leStoreInput->getInteger();
        db[":f_type"] = 1;
        db[":f_goods"] = ui->tblGoods->getInteger(i, 1);
        db[":f_qty"] = ui->tblGoods->lineEdit(i, 3)->getDouble();
        db[":f_price"] = ui->tblGoods->lineEdit(i, 5)->getDouble();
        db[":f_total"] = ui->tblGoods->lineEdit(i, 6)->getDouble();
        db[":f_base"] = id;
        db[":f_basedoc"] = ui->leDocNum->getInteger();
        db.update("a_store", where_id(id));
    }
}

bool C5StoreDoc::writeOutput(const QDate &date, int docNum, int store, double &amount, QString &err)
{
    C5Database db(fDBParams);
    C5Database dbdoc(fDBParams);
    db.startTransaction();
    QStringList goodsID;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        goodsID << ui->tblGoods->getString(i, 1);
    }
    QList<QList<QVariant> > storeData;
    QStringList queries;
    db[":f_store"] = store;
    db[":f_date"] = date;
    db.exec(QString("select s.f_base, s.f_goods, sum(s.f_qty*s.f_type), s.f_price, sum(s.f_total*s.f_type) "
            "from a_store s "
            "inner join a_header d on d.f_id=s.f_document "
            "where s.f_goods in (%1) and s.f_store=:f_store and d.f_date<=:f_date "
            "group by 1, 2, 4 "
            "having sum(s.f_qty*s.f_type) > 0.001 "
            "for update ").arg(goodsID.join(",")), storeData);
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        double qty = ui->tblGoods->lineEdit(i, 3)->getDouble();
        for (int j = 0; j < storeData.count(); j++) {
            if (storeData.at(j).at(1).toInt() == ui->tblGoods->getInteger(i, 1)) {
                if (storeData.at(j).at(2).toDouble() > 0) {
                    qDebug() << storeData.at(j).at(2).toDouble();
                    if (storeData.at(j).at(2).toDouble() >= qty) {
                        storeData[j][2] = storeData.at(j).at(2).toDouble() - qty;
                        dbdoc[":f_base"] = storeData.at(j).at(0).toInt();
                        dbdoc.exec("select f_document from a_store where f_base=:f_base and f_type=1");
                        dbdoc.nextRow();
                        queries << QString("insert into a_store (f_document, f_store, f_type, f_goods, f_qty, f_price, f_total, f_base, f_basedoc) "
                                           "values (%1, %2, -1, %3, %4, %5, %6, %7, %8)")
                                   .arg(docNum)
                                   .arg(store)
                                   .arg(ui->tblGoods->getInteger(i, 1))
                                   .arg(qty)
                                   .arg(storeData.at(j).at(3).toDouble())
                                   .arg(storeData.at(j).at(3).toDouble() * qty)
                                   .arg(storeData.at(j).at(0).toInt())
                                   .arg(dbdoc.getInt(0));
                        amount += storeData.at(j).at(3).toDouble() * qty;
                        ui->tblGoods->lineEdit(i, 6)->setDouble(ui->tblGoods->lineEdit(i, 6)->getDouble() + storeData.at(j).at(3).toDouble() * qty);
                        qty = 0;
                    } else {
                        dbdoc[":f_base"] = storeData.at(j).at(0).toInt();
                        dbdoc.exec("select f_document from a_store where f_base=:f_base and f_type=1");
                        dbdoc.nextRow();
                        queries << QString("insert into a_store (f_document, f_store, f_type, f_goods, f_qty, f_price, f_total, f_base, f_basedoc) "
                                           "values (%1, %2, -1, %3, %4, %5, %6, %7, %8)")
                                   .arg(docNum)
                                   .arg(store)
                                   .arg(ui->tblGoods->getInteger(i, 1))
                                   .arg(storeData.at(j).at(2).toDouble())
                                   .arg(storeData.at(j).at(3).toDouble())
                                   .arg(storeData.at(j).at(3).toDouble() * qty)
                                   .arg(storeData.at(j).at(0).toInt())
                                   .arg(dbdoc.getInt(0));
                        amount += storeData.at(j).at(3).toDouble() * qty;
                        qty -= storeData.at(j).at(2).toDouble();
                        storeData[j][2] = 0.0;
                        ui->tblGoods->lineEdit(i, 6)->setDouble(ui->tblGoods->lineEdit(i, 6)->getDouble() + storeData.at(j).at(3).toDouble() * qty);
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
        foreach (QString s, queries) {
            db.exec(s);
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
    lprice->setValidator(new QDoubleValidator(0, 100000000, 2));
    C5LineEdit *ltotal = ui->tblGoods->createLineEdit(row, 6);
    ltotal->setValidator(new QDoubleValidator(0, 100000000, 2));
    connect(lqty, SIGNAL(textChanged(QString)), this, SLOT(tblQtyChanged(QString)));
    connect(lprice, SIGNAL(textChanged(QString)), this, SLOT(tblPriceChanged(QString)));
    connect(ltotal, SIGNAL(textChanged(QString)), this, SLOT(tblTotalChanged(QString)));
    return row;
}

void C5StoreDoc::setDocEnabled(bool v)
{
    ui->wheader->setEnabled(v);
    ui->wtoolbar->setEnabled(v);
    for (int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for (int c = 0; c < ui->tblGoods->columnCount(); c++) {
            C5LineEdit *l = dynamic_cast<C5LineEdit*>(ui->tblGoods->cellWidget(r, c));
            if (l) {
                l->setEnabled(v);
            }
        }
    }
    fToolBar->actions().at(0)->setEnabled(v);
    fToolBar->actions().at(1)->setEnabled(!v);
}

void C5StoreDoc::saveDoc()
{
    save(DOC_STATE_SAVED);
}

void C5StoreDoc::draftDoc()
{
    save(DOC_STATE_DRAFT);
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