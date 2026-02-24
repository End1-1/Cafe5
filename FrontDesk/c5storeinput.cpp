#include "c5storeinput.h"
#include <QClipboard>
#include <QDesktopServices>
#include <QFileDialog>
#include <QHash>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QMenu>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QShortcut>
#include <QSqlQuery>
#include "bclientdebts.h"
#include "c5codenameselector.h"
#include "c5codenameselectorfunctions.h"
#include "c5config.h"
#include "c5editor.h"
#include "c5htmlprint.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5permissions.h"
#include "c5storebarcode.h"
#include "c5storedocselectprinttemplate.h"
#include "c5storedraftwriter.h"
#include "c5user.h"
#include "c5utils.h"
#include "calculator.h"
#include "ce5goods.h"
#include "ce5partner.h"
#include "ui_c5storeinput.h"
#include <xlsxdocument.h>

#define col_rec_in_id 0
#define col_rec_out_id 1
#define col_rec_base_id 2
#define col_goods_id 3
#define col_goods_name 4
#define col_adgt 5
#define col_goods_qty 6
#define col_goods_unit 7
#define col_price 8
#define col_total 9
#define col_valid_date 10
#define col_comment 11
#define col_remain 12

C5StoreInput::C5StoreInput(C5User *user, const QString &title, QIcon icon, QWidget *parent)
    : C5Widget(parent)
    , ui(new Ui::C5StoreInput)
{
    mUser = user;
    ui->setupUi(this);
    fLabel = title;
    fIcon = icon;
    ui->wInputStore->selectorCallback = storeItemSelector;
    ui->wSearchInDocs->setVisible(false);
    ui->lePartner->setCallbackWidget(this);
    QMap<int, int> colwidths = {{col_rec_in_id, 0},
                                {col_rec_out_id, 0},
                                {col_rec_base_id, 0},
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

    ui->btnNewPartner->setEnabled(mUser->check(cp_t7_partners));
    ui->btnNewGoods->setEnabled(mUser->check(cp_t6_goods));
    ui->btnEditGoods->setEnabled(mUser->check(cp_t6_goods));
    ui->lbCashDoc->setVisible(!C5Config::noCashDocStore());
    ui->leCash->setVisible(!C5Config::noCashDocStore());
    ui->leCashName->setVisible(!C5Config::noCashDocStore());
    ui->deCashDate->setVisible(!C5Config::noCashDocStore());
    ui->chPaid->setVisible(!C5Config::noCashDocStore());
    ui->leScancode->setVisible(!C5Config::noScanCodeStore());
    fInternalId = C5Database::uuid();
    fDocState = DOC_STATE_DRAFT;
    fBasedOnSale = 0;
    connect(ui->leInvoiceNumber, SIGNAL(focusOut()), this, SLOT(checkInvoiceDuplicate()));
    fFocusNextChild = false;
    fCanChangeFocus = true;
    ui->tblAdd->setColumnWidths(ui->tblAdd->columnCount(), 0, 300, 80);
    ui->deDate->setEnabled(mUser->check(cp_t1_allow_change_store_doc_date));
    ui->btnRememberStoreIn->setChecked(__c5config.getRegValue("storedoc_storeinput").toBool());

    if (__c5config.getRegValue("storedoc_storeinput").toBool()) {
        ui->wInputStore
            ->setCodeAndName(__c5config.getRegValue("storedoc_storeinput_id").toInt(),
                             __c5config.getRegValue("storedoc_storeinput_name").toString());
    }

    if (__c5config.getRegValue("storedoc_fixpartner").toInt() > 0) {
        ui->lePartner->setValue(__c5config.getRegValue("storedoc_fixpartner").toInt());
        ui->btnFixPartner->setChecked(true);
    }

    adjustSize();
    C5Database db;
    db.exec("select * from e_currency order by f_id");

    while (db.nextRow()) {
        ui->cbCurrency->addItem(db.getString("f_name"), db.getInt("f_id"));
    }

    ui->cbCurrency->setCurrentIndex(
        ui->cbCurrency->findData(__c5config.getValue(param_default_currency).toInt()));
}

C5StoreInput::~C5StoreInput()
{
    __mainWindow->removeBroadcastListener(this);
    delete ui;
}

bool C5StoreInput::openDoc(QString id, QString &err)
{
    return true;
}

void C5StoreInput::correctDebt()
{
    double amount = ui->leTotal->getDouble() * -1;
    C5Database db;
    C5StoreDraftWriter dw(db);

    if (fDocState == DOC_STATE_SAVED) {
        if (ui->lePartner->getInteger() > 0) {
            dw.updateField("b_clients_debts", "f_amount", amount, "f_storedoc", fInternalId);
        }
    }
}

QToolBar *C5StoreInput::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDoc()));
        fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDoc()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New\ndocument"), this, SLOT(newDoc()));
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

bool C5StoreInput::removeDoc(QString id, bool showmessage)
{
    if (showmessage) {
        if (C5Message::question(tr("Confirm to remove document")) != QDialog::Accepted) {
            return false;
        }
    }

    QString err;
    C5Database db;
    QJsonObject jo;
    jo["removeid"] = id;
    db.exec(QString("select sf_remove_store_doc('%1')").arg(json_str(jo)));

    if (db.nextRow()) {
        jo = str_json(db.getString(0));
        return jo["status"] == 1;
    }

    return false;
}

bool C5StoreInput::allowChangeDatabase()
{
    return false;
}

void C5StoreInput::addByScancode(const QString &code, const QString &qty, QString price)
{
    if (ui->chWholeGroup->isChecked()) {
        //add whole group
        return;
    }

    //       int row = addGoodsRow();
    //       ui->tblGoods->setInteger(row, col_goods_id, db.getInt("f_id"));
    //       ui->tblGoods->setString(row, col_goods_name, db.getString("f_name"));
    //       ui->tblGoods->setString(row, col_goods_unit, db.getString("f_unitname"));
    //       ui->tblGoods->item(row, col_goods_name)->setSelected(true);
    //       fCanChangeFocus = false;
    //       ui->tblGoods->lineEdit(row, col_goods_qty)->setText(qty);
    //       ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
    //       ui->tblGoods->lineEdit(row, col_price)->setText(price);
    //       ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(db.getDouble("f_lastinputprice"), 2));

    //       if(__c5config.getValue(param_input_doc_fix_price).toInt() > 0) {
    //           if(price.toDouble() < 0.001) {
    //               if(db.getDouble("f_lastinputprice") > 0.001) {
    //                   ui->tblGoods->lineEdit(row, col_price)->setDouble(db.getDouble("f_lastinputprice"));
    //                   price = db.getString("f_lastinputprice");
    //               }
    //           }
    //       }

    //       emit ui->tblGoods->lineEdit(row, col_price)->textEdited(price);

    //       if(fDocType == DOC_TYPE_STORE_INPUT) {
    //           fHttp->createHttpQuery("/engine/worker/check-qty.php", QJsonObject{
    //               {"method", "checkStore"}, {"goodsid", ui->tblGoods->getInteger(row, col_goods_id)}
    //           }, SLOT(slotCheckQtyResponse(QJsonObject)), QVariant(), false);
    //       }
    //   } else {
    //       db[":f_scancode"] = code;
    //       db.exec("select gg.f_scancode, gg.f_id, concat(gg.f_name, ' ', gg.f_scancode) as f_name, gu.f_name as f_unitname, gg.f_saleprice, "
    //               "gr.f_taxdept, gr.f_adgcode, gg.f_lastinputprice "
    //               "from c_goods gg  "
    //               "left join c_groups gr on gr.f_id=gg.f_group "
    //               "left join c_units gu on gu.f_id=gg.f_unit "
    //               "where gg.f_id in (select f_goods from c_goods_multiscancode where f_id=:f_scancode)");

    //       if(db.nextRow()) {
    //           int row = addGoodsRow();
    //           ui->tblGoods->setInteger(row, col_goods_id, db.getInt("f_id"));
    //           ui->tblGoods->setString(row, col_goods_name, db.getString("f_name"));
    //           ui->tblGoods->setString(row, col_goods_unit, db.getString("f_unitname"));
    //           ui->tblGoods->item(row, col_goods_name)->setSelected(true);
    //           fCanChangeFocus = false;
    //           ui->tblGoods->lineEdit(row, col_goods_qty)->setText(qty);
    //           ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
    //           ui->tblGoods->lineEdit(row, col_price)->setText(price);
    //           ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(db.getDouble("f_lastinputprice"), 2));

    //           if(__c5config.getValue(param_input_doc_fix_price).toInt() > 0) {
    //               if(price.toDouble() < 0.001) {
    //                   if(db.getDouble("f_lastinputprice") > 0.001) {
    //                       ui->tblGoods->lineEdit(row, col_price)->setDouble(db.getDouble("f_lastinputprice"));
    //                       price = db.getString("f_lastinputprice");
    //                   }
    //               }
    //           }

    //           emit ui->tblGoods->lineEdit(row, col_price)->textEdited(price);

    //               fHttp->createHttpQuery("/engine/worker/check-qty.php", QJsonObject{
    //                   {"method", "checkStore"}, {"goodsid", ui->tblGoods->getInteger(row, col_goods_id)}
    //               }, SLOT(slotCheckQtyResponse(QJsonObject)), QVariant(), false);
    //           }
}

double C5StoreInput::total()
{
    return ui->leTotal->getDouble();
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

bool C5StoreInput::writeDocument(int state, QString &err)
{
    if (!docCheck(err, state)) {
        return false;
    }

    return true;
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
}

bool C5StoreInput::docCheck(QString &err, int state)
{
    rowsCheck(err);

    if (ui->wInputStore->value() == 0) {
        err += tr("Input store is not defined") + "<br>";
    }

    if (ui->chPaid->isChecked() && ui->leCash->getInteger() == 0) {
        err += tr("Cash must be selected") + "<br>";
    }

    if (state == 1 && ui->lePartner->getInteger() == 0) {
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
    ui->tblGoods->setItem(row, 0, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 1, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 2, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 3, new QTableWidgetItem());
    ui->tblGoods->setItem(row, 4, new QTableWidgetItem());
    C5LineEdit *lqty = ui->tblGoods->createLineEdit(row, col_goods_qty);
    lqty->setValidator(new QDoubleValidator(0, 1000000, 4));
    lqty->fDecimalPlaces = 4;
    lqty->addEventKeys("+-*");
    connect(lqty, SIGNAL(keyPressed(QChar)), this, SLOT(lineEditKeyPressed(QChar)));
    connect(lqty, SIGNAL(returnPressed()), this, SLOT(focusNextChildren()));
    ui->tblGoods->setItem(row, col_goods_unit, new QTableWidgetItem());
    C5LineEdit *lprice = ui->tblGoods->createLineEdit(row, col_price);
    lprice->setValidator(new QDoubleValidator(0, 100000000, col_goods_id));
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
    ui->tblGoods->setItem(row, col_adgt, new QTableWidgetItem());
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

    ui->btnChangePartner->setEnabled(!v);
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

void C5StoreInput::printV1()
{
    QString tpl = loadTemplate("store_doc_v1.html");

    if (tpl.isEmpty()) {
        C5Message::error(tr("Template not found: store_doc_v1.html"));
        return;
    }

    // 1) Заголовок (docTypeText)
    QString docTypeText = tr("Store input");

    // 2) Stores
    QString storeInName, storeOutName;

    if (ui->wInputStore->value() > 0)
        storeInName = ui->wInputStore->name();

    // 3) Комментарий
    QString commentBlock;

    if (!ui->leComment->isEmpty()) {
        commentBlock = "<div class='note'>" + htmlEscape(ui->leComment->text()) + "</div>";
    }

    // 4) Header (date/stores) — динамические колонки
    QString thStoreIn, tdStoreIn, thStoreOut, tdStoreOut;

    if (!storeInName.isEmpty()) {
        thStoreIn = "<th>" + htmlEscape(tr("Store, input")) + "</th>";
        tdStoreIn = "<td>" + htmlEscape(storeInName) + "</td>";
    }

    if (!storeOutName.isEmpty()) {
        thStoreOut = "<th>" + htmlEscape(tr("Store, output")) + "</th>";
        tdStoreOut = "<td>" + htmlEscape(storeOutName) + "</td>";
    }

    // 5) Supplier/Invoice block — как у тебя через tableText(points)
    QString supplierInvoiceBlock;
    {
        bool hasSupplier = ui->lePartner->getInteger() > 0;
        bool hasInvoice = !ui->leInvoiceNumber->isEmpty();

        if (hasSupplier || hasInvoice) {
            QString t;
            QTextStream s(&t);
            s << "<table class='mt6'>"
              << "<tr>";

            if (hasSupplier)
                s << "<th style='width:55%'>" << htmlEscape(tr("Supplier")) << "</th>";

            if (hasInvoice)
                s << "<th>" << htmlEscape(tr("Purchase document")) << "</th>";

            s << "</tr><tr>";

            if (hasSupplier)
                s << "<td>" << htmlEscape(ui->lePartnerName->text()) << "</td>";

            if (hasInvoice)
                s << "<td>"
                  << htmlEscape(ui->leInvoiceNumber->text() + " / " + ui->deInvoiceDate->text())
                  << "</td>";

            s << "</tr></table>";
            supplierInvoiceBlock = t;
        } else {
            supplierInvoiceBlock = "";
        }
    }

    // 7) Формируем goodsGroupData (оставляем твою логику 1:1, только без печати)
    QMap<int, int> goodsGroupMap;
    QList<QStringList> goodsGroupData;

    for (int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        QStringList val
            = {"", "", "", "0", "", "", "0", ""}; // [NN, code, name, qty, unit, price, total, remain]

        if (ui->chLeaveFocusOnBarcode->isChecked()) {
            int key = ui->tblGoods->getInteger(i, col_goods_id);

            if (goodsGroupMap.contains(key)) {
                val = goodsGroupData[goodsGroupMap[key]];
            } else {
                val[0] = QString::number(goodsGroupData.count() + 1);
            }
        } else {
            val[0] = QString::number(i + 1);
        }

        val[1] = ui->tblGoods->getString(i, col_goods_id);   // material code?
        val[2] = ui->tblGoods->getString(i, col_goods_name); // goods name

        if (ui->chLeaveFocusOnBarcode->isChecked()) {
            val[3] = float_str(str_float(val[3])
                                   + ui->tblGoods->lineEdit(i, col_goods_qty)->getDouble(),
                               3);
        } else {
            val[3] = ui->tblGoods->lineEdit(i, col_goods_qty)->text();
        }

        val[4] = ui->tblGoods->getString(i, col_goods_unit); // unit
        val[5] = "0";
        val[6] = "0";
        // !!! у тебя потом price пересчитывается из total/qty:
        val[5] = float_str(str_float(val[6]) / qMax(0.000001, str_float(val[3])), 2);
        val[7] = float_str(ui->tblGoods->getDouble(i, col_remain), 2);

        if (ui->chLeaveFocusOnBarcode->isChecked()) {
            int key = ui->tblGoods->getInteger(i, col_goods_id);

            if (goodsGroupMap.contains(key)) {
                goodsGroupData[goodsGroupMap[key]] = val;
            } else {
                goodsGroupData.append(val);
                goodsGroupMap[key] = goodsGroupData.count() - 1;
            }
        } else {
            goodsGroupData.append(val);
        }
    }

    // 8) Заголовок колонки Goods зависит от типа документа
    QString goodsColName = tr("Goods");

    // 9) Goods table HTML
    QStringList goodsHdr{tr("NN"),
                         tr("Material code"),
                         goodsColName,
                         tr("Qty"),
                         tr("Unit"),
                         tr("Price"),
                         tr("Total"),
                         tr("Remain")};
    // right align: Qty(3), Price(5), Total(6), Remain(7)
    QSet<int> rightCols{3, 5, 6, 7};
    QString goodsTable = makeGoodsTableHtml(goodsHdr, goodsGroupData, rightCols);
    // 10) Подписи (в зависимости от типа)
    QString signLeftTitle, signRightTitle, signLeftName, signRightName;

    signLeftTitle = tr("Accepted");
    signRightTitle = tr("Passed");
    signLeftName = ui->leAcceptedName->text();
    signRightName = ui->lePassedName->text();

    // 11) Переменные шаблона
    QMap<QString, QString> v;
    v["doc_title"] = htmlEscape(QString("%1 N%2").arg(docTypeText, ui->leDocNum->text()));
    v["reason"] = htmlEscape(ui->leReasonName->text());
    v["comment_block"] = commentBlock;
    v["lbl_date"] = htmlEscape(tr("Date"));
    v["date"] = htmlEscape(ui->deDate->text());
    v["th_store_in"] = thStoreIn;
    v["td_store_in"] = tdStoreIn;
    v["th_store_out"] = thStoreOut;
    v["td_store_out"] = tdStoreOut;
    v["supplier_invoice_block"] = supplierInvoiceBlock;
    v["goods_table"] = goodsTable;
    v["lbl_total_amount"] = htmlEscape(tr("Total amount"));
    v["total_amount"] = htmlEscape(ui->leTotal->text());
    v["sign_left_title"] = htmlEscape(signLeftTitle);
    v["sign_right_title"] = htmlEscape(signRightTitle);
    v["sign_left_name"] = htmlEscape(signLeftName);
    v["sign_right_name"] = htmlEscape(signRightName);
    QString html = applyTemplate(tpl, v);
    // 12) Preview/Print
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setFullPage(false);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, [&](QPrinter *prn) { doc.print(prn); });
    preview.exec();
}

void C5StoreInput::printV2()
{
    const QString tplSheet = loadTemplate("store_doc_v2_two_up.html");
    const QString tplOne = loadTemplate("store_doc_v2_one_copy.html");

    if (tplSheet.isEmpty() || tplOne.isEmpty()) {
        C5Message::error(tr("Template not found"));
        return;
    }

    // ---- docTypeText
    QString docTypeText = tr("Store input");
    // ---- stores
    QString storeInName, storeOutName;

    if (ui->wInputStore->value() > 0)
        storeInName = ui->wInputStore->name();

    // ---- comment
    QString commentBlock;

    if (!ui->leComment->isEmpty()) {
        commentBlock = "<div class='note'>" + htmlEscape(ui->leComment->text()) + "</div>";
    }

    // ---- dynamic store columns
    QString thStoreIn, tdStoreIn, thStoreOut, tdStoreOut;

    if (!storeInName.isEmpty()) {
        thStoreIn = "<th>" + htmlEscape(tr("Store, input")) + "</th>";
        tdStoreIn = "<td>" + htmlEscape(storeInName) + "</td>";
    }

    if (!storeOutName.isEmpty()) {
        thStoreOut = "<th>" + htmlEscape(tr("Store, output")) + "</th>";
        tdStoreOut = "<td>" + htmlEscape(storeOutName) + "</td>";
    }

    // ---- supplier/invoice block
    QString supplierInvoiceBlock;
    {
        bool hasSupplier = ui->lePartner->getInteger() > 0;
        bool hasInvoice = !ui->leInvoiceNumber->isEmpty();

        if (hasSupplier || hasInvoice) {
            QString t;
            QTextStream s(&t);
            s << "<table class='mt6'><tr>";

            if (hasSupplier)
                s << "<th style='width:55%'>" << htmlEscape(tr("Supplier")) << "</th>";

            if (hasInvoice)
                s << "<th>" << htmlEscape(tr("Purchase document")) << "</th>";

            s << "</tr><tr>";

            if (hasSupplier)
                s << "<td>" << htmlEscape(ui->lePartnerName->text()) << "</td>";

            if (hasInvoice)
                s << "<td>"
                  << htmlEscape(ui->leInvoiceNumber->text() + " / " + ui->deInvoiceDate->text())
                  << "</td>";

            s << "</tr></table>";
            supplierInvoiceBlock = t;
        }
    }

    // ---- goods table
    QString goodsColName = tr("Goods");

    // ВАЖНО: V2 печатает из ui->tblGoods построчно, без группировки по barcode как V1.
    // Оставим ту же логику, что была в V2.
    QList<QStringList> rows;

    for (int i = 0; i < ui->tblGoods->rowCount(); ++i) {
        QStringList r;
        r << QString::number(i + 1);
        r << ui->tblGoods->getString(i, col_goods_id);
        r << ui->tblGoods->getString(i, col_goods_name);
        r << ui->tblGoods->lineEdit(i, col_goods_qty)->text();
        r << ui->tblGoods->getString(i, col_goods_unit);
        r << "0";
        r << "0";
        rows << r;
    }

    QStringList hdr{tr("NN"),
                    tr("Material code"),
                    goodsColName,
                    tr("Qty"),
                    tr("Unit"),
                    tr("Price"),
                    tr("Total")};
    // right align: Qty(3), Price(5), Total(6)
    QSet<int> rightCols{3, 5, 6};
    QString goodsTable = makeGoodsTableHtml(hdr, rows, rightCols);
    // ---- signatures
    QString signLeftTitle, signRightTitle, signLeftName, signRightName;

    signLeftTitle = tr("Accepted");
    signRightTitle = tr("Passed");
    signLeftName = ui->leAcceptedName->text();
    signRightName = ui->lePassedName->text();

    // ---- vars for one copy
    QMap<QString, QString> v;
    v["doc_title"] = htmlEscape(QString("%1 N%2").arg(docTypeText, ui->leDocNum->text()));
    v["reason"] = htmlEscape(ui->leReasonName->text());
    v["comment_block"] = commentBlock;
    v["lbl_date"] = htmlEscape(tr("Date"));
    v["date"] = htmlEscape(ui->deDate->text());
    v["th_store_in"] = thStoreIn;
    v["td_store_in"] = tdStoreIn;
    v["th_store_out"] = thStoreOut;
    v["td_store_out"] = tdStoreOut;
    v["supplier_invoice_block"] = supplierInvoiceBlock;
    v["goods_table"] = goodsTable;
    v["lbl_total_amount"] = htmlEscape(tr("Total amount"));
    v["total_amount"] = htmlEscape(ui->leTotal->text());
    v["sign_left_title"] = htmlEscape(signLeftTitle);
    v["sign_right_title"] = htmlEscape(signRightTitle);
    v["sign_left_name"] = htmlEscape(signLeftName);
    v["sign_right_name"] = htmlEscape(signRightName);
    const QString oneCopyHtml = applyTemplate(tplOne, v);
    // ---- sheet vars
    QMap<QString, QString> svars;
    svars["one_copy"] = oneCopyHtml;
    const QString html = applyTemplate(tplSheet, svars);
    // ---- preview/print
    QTextDocument doc;
    doc.setHtml(html);
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPageSize::A4);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setFullPage(false);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, [&](QPrinter *p) { doc.print(p); });
    preview.exec();
}

double C5StoreInput::additionalCost()
{
    double c = 0;

    for(int i = 0; i < ui->tblAdd->rowCount(); i++) {
        c += ui->tblAdd->lineEdit(i, 2)->getDouble();
    }

    return c;
}

double C5StoreInput::additionalCostForEveryGoods()
{
    if(ui->leTotalQty->getDouble() > 0) {
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
    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if(ui->tblGoods->getInteger(i, col_goods_id) == jdoc["goodsid"].toInt()) {
            ui->tblGoods->setDouble(i, col_remain, jdoc["remain"].toDouble());
        }
    }
}

void C5StoreInput::exportToExcel()
{
    QString docTypeText = tr("Store input");

    QString storeInName;

    if (ui->wInputStore->value() > 0) {
        storeInName = ui->wInputStore->name();
    }

    QXlsx::Document d;
    d.addSheet("Sheet1");
    /* HEADER */
    QColor color = Qt::white;
    QFont headerFont(qApp->font());
    QXlsx::Format hf;
    hf.setFont(headerFont);
    hf.setFontColor(Qt::black);
    hf.setBorderStyle(QXlsx::Format::BorderThin);
    hf.setPatternBackgroundColor(color);
    d.setColumnWidth(1, 10);
    d.setColumnWidth(2, 15);
    d.setColumnWidth(3, 15);
    d.setColumnWidth(4, 50);
    d.setColumnWidth(5, 20);
    d.setColumnWidth(6, 20);
    d.setColumnWidth(7, 20);
    d.setColumnWidth(8, 20);
    int col = 1, row = 1;
    d.write(row, col, QString("%1 N%2").arg(docTypeText, ui->leDocNum->text()), hf);
    row++;
    d.write(row, col, QString("%1 %2").arg(tr("Reason"), ui->leReasonName->text()), hf);
    row++;

    if(!ui->leComment->isEmpty()) {
        d.write(row, col, tr("Comment") + " " + ui->leComment->text(), hf);
        row++;
    }

    QList<int> cols;
    QStringList vals;
    col = 1;
    cols << col++;
    vals << tr("Date");

    if(!storeInName.isEmpty()) {
        vals << tr("Store, input");
        cols << col++;
    }

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), hf);
    }

    row++;
    vals.clear();
    vals << ui->deDate->text();

    if(!storeInName.isEmpty()) {
        vals << storeInName;
    }

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), hf);
    }

    row += 2;
    cols.clear();

    for(int i = 0; i < 7; i++) {
        cols << i + 1;
    }

    vals.clear();
    vals << tr("NN")
         << tr("Material code")
         << tr("Adgt")
         << tr("Goods")
         << tr("Qty")
         << tr("Unit")
         << tr("Price")
         << tr("Total");

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), hf);
    }

    row++;
    QFont bodyFont(qApp->font());
    QXlsx::Format bf;
    bf.setFontColor(Qt::black);
    bf.setFont(bodyFont);
    bf.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    bf.setBorderStyle(QXlsx::Format::BorderThin);

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        vals.clear();
        vals << QString::number(i + 1);
        vals << ui->tblGoods->getString(i, col_goods_id);
        vals << ui->tblGoods->getString(i, col_adgt);
        vals << ui->tblGoods->getString(i, col_goods_name);
        vals << QString::number(str_float(ui->tblGoods->lineEdit(i, col_goods_qty)->text()));
        vals << ui->tblGoods->getString(i, col_goods_unit);
        vals << QString::number(str_float(ui->tblGoods->lineEdit(i, col_price)->text()));
        vals << QString::number(str_float(ui->tblGoods->lineEdit(i, col_total)->text()));

        for(int i = 0; i < cols.count(); i++) {
            d.write(row, cols.at(i), vals.at(i), bf);
        }

        row++;
    }

    cols.clear();
    cols << 5 << 6 << 7;
    vals.clear();
    vals << tr("Total amount");
    vals << QString::number(str_float(ui->leTotal->text()));
    vals << ui->cbCurrency->currentText();

    for(int i = 0; i < cols.count(); i++) {
        d.write(row, cols.at(i), vals.at(i), hf);
    }

    row++;
    d.mergeCells("A1:G1");
    d.mergeCells("A2:G2");
    d.mergeCells("A3:G3");
    QString filename = QFileDialog::getSaveFileName(nullptr, "", "", "*.xlsx");

    if(filename.isEmpty()) {
        return;
    }

    d.saveAs(filename);
    QDesktopServices::openUrl(filename);
}

void C5StoreInput::getInput()
{
    QJsonArray vals;


    if(vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add goods without code"));
        return;
    }

    int row = addGoods(vals.at(1).toInt(), vals.at(3).toString(), -1, vals.at(4).toString(), -1, -1, "", vals.at(11).toString());

    if(__c5config.getValue(param_input_doc_fix_price).toInt() > 0) {
        ui->tblGoods->lineEdit(row, col_price)->setDouble(vals.at(6).toDouble());
    }

    ui->tblGoods->lineEdit(row, col_price)->setPlaceholderText(float_str(vals.at(6).toDouble(), 2));
    ui->tblGoods->lineEdit(row, col_goods_qty)->setFocus();
    fHttp->createHttpQuery("/engine/worker/check-qty.php", {
        {"method", "checkStore"}, {"goodsid", vals.at(1).toInt()}
    }, SLOT(slotCheckQtyResponse(QJsonObject)), QVariant(), false);
}

void C5StoreInput::saveDoc()
{
    if (__c5config.getRegValue("storedoc_storeinput").toBool()) {
        __c5config.setRegValue("storedoc_storeinput_id", ui->wInputStore->value());
        __c5config.setRegValue("storedoc_storeinput_name", ui->wInputStore->name());
    }

    __c5config.setRegValue("storedoc_fixpartner", ui->btnFixPartner->isChecked() ? ui->lePartner->getInteger() : 0);
}

void C5StoreInput::removeDocument()
{
    if(removeDoc(fInternalId, true)) {
        __mainWindow->removeTab(this);
    }
}

void C5StoreInput::printDoc()
{
    if(fInternalId == nullptr) {
        C5Message::error(tr("Document is not saved"));
        return;
    }

    C5StoreDocSelectPrintTemplate d;

    switch(d.exec()) {
    case 1:
        printV1();
        break;

    case 2:
        printV2();
        break;

    default:
        return;
    }
}

void C5StoreInput::printBarcode()
{
    C5StoreBarcode *b = __mainWindow->createTab<C5StoreBarcode>();
    C5Database db;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        db[":f_id"] = ui->tblGoods->getInteger(i, col_goods_id);
        db.exec("select f_name, f_scancode from c_goods where f_id=:f_id");

        if(db.nextRow()) {
            b->addRow(db.getString("f_name"), db.getString("f_scancode"),
                      ui->tblGoods->lineEdit(i, col_goods_qty)->getInteger() < 1 ? 1 : ui->tblGoods->lineEdit(i, col_goods_qty)->getInteger(),
                      ui->cbCurrency->currentData().toInt(), "");
        }
    }
}

void C5StoreInput::checkInvoiceDuplicate()
{
    ui->leInvoiceNumber->setStyleSheet("");
    C5Database db;
    db[":f_invoice"] = ui->leInvoiceNumber->text();
    db[":f_id"] = fInternalId;
    db.exec("select count(f_id) from a_header_store where f_invoice=:f_invoice and f_id<>:f_id");

    if(db.nextRow()) {
        if(db.getInt(0) > 0) {
            ui->leInvoiceNumber->setStyleSheet("background:red;");
        } else {
            ui->leInvoiceNumber->setStyleSheet("");
        }
    }
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

    if(!ui->tblGoods->findWidget(static_cast<QWidget* >(sender()), row, col)) {
        return;
    }

    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
    ltotal->setDouble(lqty->getDouble() *lprice->getDouble());
    countTotal();
}

void C5StoreInput::tblPriceChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    int row, col;

    if(!ui->tblGoods->findWidget(static_cast<QWidget* >(sender()), row, col)) {
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

    if(!ui->tblGoods->findWidget(static_cast<QWidget* >(sender()), row, col)) {
        return;
    }

    C5LineEdit *lqty = ui->tblGoods->lineEdit(row, col_goods_qty);
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);

    if(lqty->getDouble() > 0.001) {
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

    if(e->getResult(data)) {
        // ui->lePartner->setValue(data.at(0)["f_id"].toString());
    }

    delete e;
}

void C5StoreInput::on_btnNewGoods_clicked()
{
    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    QJsonObject data;

    if(e->getJsonObject(data)) {
        QJsonObject j = data["goods"].toObject();

        if(j["f_id"].toInt() == 0) {
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
    QString qty  = ui->chLeaveFocusOnBarcode->isChecked() ? "1" : "";
    addByScancode(ui->leScancode->text(), qty, "");

    if(ui->chLeaveFocusOnBarcode->isChecked()) {
        ui->leScancode->setFocus();
    }
}


void C5StoreInput::on_chPaid_clicked(bool checked)
{
    ui->lbCashDoc->setEnabled(checked);
    ui->leCash->setEnabled(checked);
    ui->leCashName->setEnabled(checked);
    ui->deCashDate->setEnabled(checked);
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

    if(row < 0) {
        return;
    }

    if(C5Message::question(tr("Confirm to remove") + "<br>" + ui->tblAdd->lineEdit(row, 2)->text()) != QDialog::Accepted) {
        return;
    }

    ui->tblAdd->removeRow(row);
    countTotal();
}

void C5StoreInput::on_btnEditGoods_clicked()
{
    int row = ui->tblGoods->currentRow();

    if(row < 0) {
        return;
    }

    if(ui->tblGoods->getInteger(row, col_goods_id) == 0) {
        return;
    }

    CE5Goods *ep = new CE5Goods();
    C5Editor *e = C5Editor::createEditor(mUser, ep, 0);
    ep->setId(ui->tblGoods->getInteger(row, col_goods_id));
    QJsonObject data;

    if(e->getJsonObject(data)) {
        QJsonObject j = data["goods"].toObject();

        if(j["f_id"].toInt() == 0) {
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
    qApp->clipboard()->setText(fInternalId);
}

void C5StoreInput::on_leSearchInDoc_textChanged(const QString &arg1)
{
    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        bool hidden = !arg1.isEmpty();

        if(!arg1.isEmpty()) {
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

void C5StoreInput::on_btnFillRemote_clicked(bool checked)
{
    if(checked) {
        ui->btnFillRemote->setIcon(QIcon(":/wifi.png"));
        __mainWindow->addBroadcastListener(this);
    } else {
        ui->btnFillRemote->setIcon(QIcon(":/wifib.png"));
        __mainWindow->removeBroadcastListener(this);
    }
}

void C5StoreInput::on_btnChangePartner_clicked() {}

void C5StoreInput::on_btnFixPartner_clicked(bool checked)
{
    __c5config.setRegValue("storedoc_fixpartner", checked ? ui->lePartner->getInteger() : 0);
}

void C5StoreInput::on_btnCopyLastAdd_clicked()
{
    C5Database db;
    db.exec("select * from s_draft");

    if(db.nextRow()) {
        db[":f_header"] = db.getString(1);
        db.exec("select * from a_complectation_additions where f_header=:f_header order by f_row");

        while(db.nextRow()) {
            int row = ui->tblAdd->rowCount();
            ui->tblAdd->setRowCount(row + 1);
            C5LineEdit *l = ui->tblAdd->createLineEdit(row, 1);
            l->setFocus();
            l->setText(db.getString("f_name"));
            l = ui->tblAdd->createLineEdit(row, 2);
            l->setValidator(new QDoubleValidator());
            l->setDouble(db.getDouble("f_amount"));
            connect(l, SIGNAL(textEdited(QString)), this, SLOT(tblAddChanged(QString)));
        }
    }

    countTotal();
}

void C5StoreInput::on_btnEditPassed_clicked()
{
    QJsonArray values;


    if(values.count() == 0) {
        return;
    }

    if(fDocState == DOC_STATE_SAVED) {
        if(C5Message::question(tr("This action will result in immediate changes to the data.")) != QDialog::Accepted) {
            return;
        }

        C5Database db;
        db[":f_id"] = fInternalId;
        db[":f_userpass"] = values.at(1).toInt();
        db.exec("update a_header set f_body=json_set(f_body, '$.f_passed', :f_userpass) where f_id=:f_id");
    }

    ui->lePassed->setValue(values.at(1).toInt());
}

void C5StoreInput::on_btnEditAccept_clicked()
{
    QJsonArray values;

    if(values.count() == 0) {
        return;
    }

    if(fDocState == DOC_STATE_SAVED) {
        if(C5Message::question(tr("This action will result in immediate changes to the data.")) != QDialog::Accepted) {
            return;
        }

        C5Database db;
        db[":f_id"] = fInternalId;
        db[":f_useraccept"] = values.at(1).toInt();
        db.exec("update a_header set f_body=json_set(f_body, '$.f_accepted', :f_useraccept) where f_id=:f_id");
    }

    ui->leAccepted->setValue(values.at(1).toInt());
}

void C5StoreInput::on_btnSaveComment_clicked()
{
    C5Database db;
    db[":f_comment"] = ui->leComment->text();
    db[":f_id"] = fInternalId;
    db.exec("update a_header set f_comment=:f_comment where f_id=:f_id");
}

void C5StoreInput::on_btnSavePayment_clicked()
{
    if(fDocState == DOC_STATE_DRAFT) {
        C5Message::error(tr("Inpossible in draft mode"));
        return;
    }

    QString err;
    C5Database db;

    if(ui->chPaid->isChecked()) {
        if(ui->leCash->getInteger() == 0) {
            C5Message::error(tr("Cash must be selected"));
            return;
        }

        QJsonObject jo;
        jo["f_cashout"] = ui->leCash->getInteger();
        jo["f_currency"] = ui->cbCurrency->currentData().toInt();
        jo["f_amount"] = ui->leTotal->getDouble();
        jo["f_purpose"] = QString("%1 %2").arg(tr("Store input"), ui->leDocNum->text());
        jo["f_comment"] = QString("%1 %2").arg(tr("Store input"), ui->leDocNum->text());
        jo["f_operator"] = mUser->id();
        jo["f_date"] = ui->deCashDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
        db[":params"] = QString(QJsonDocument(jo).toJson());
        db.exec("select sf_create_cashdoc(:params)");
        db.nextRow();
        jo = QJsonDocument::fromJson(db.getString(0).toUtf8()).object();
        fCashDocUuid = jo["cashdoc"].toString();
        db[":f_id"] = fInternalId;
        db[":f_payment"] = fCashDocUuid;
        db.exec("update a_header set f_payment=:f_payment where f_id=:f_id");
        db[":f_storedoc"] = fInternalId;
        db.exec("delete from b_clients_debts where f_storedoc=:f_storedoc");
    } else {
        db[":cashdoc"] = fCashDocUuid;
        db.exec("select sf_remove_cashdoc(:cashdoc) ");
        BClientDebts b;
        b.date = ui->deDate->date();
        b.costumer = ui->lePartner->getInteger();
        b.currency = ui->cbCurrency->currentData().toInt();
        b.amount = -1 * ui->leTotal->getDouble();
        b.source = 1;
        b.store = fInternalId;
        b.write(db, err);
        b.comment = QString("%1 %2").arg(tr("Store input"), ui->leDocNum->text());
    }

    C5Message::info(tr("Payment saved"));
}
