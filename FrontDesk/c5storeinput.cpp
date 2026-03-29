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
#include "c5codenameselector.h"
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
#include "calculator.h"
#include "ce5goods.h"
#include "ce5partner.h"
#include "format_date.h"
#include "ui_c5storeinput.h"
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
    ui->deDate->setEnabled(mUser->check(cp_t1_allow_change_store_doc_date));

    if (__c5config.getRegValue("storedoc_storeinput").toBool()) {
        ui->wInputStore
            ->setCodeAndName(__c5config.getRegValue("storedoc_storeinput_id").toInt(),
                             __c5config.getRegValue("storedoc_storeinput_name").toString());
    }

    adjustSize();
}

C5StoreInput::~C5StoreInput()
{
    __mainWindow->removeBroadcastListener(this);
    delete ui;
}

void C5StoreInput::setDocument(StoreInputDocument doc)
{
    mDocData = doc;
    ui->deDate->setDate(QDateTime::fromString(doc.date, FORMAT_DATETIME_TO_STR_MYSQL).date());
    ui->leDocNum->setText(doc.user_id);
    ui->wInputStore->setCodeAndName(doc.store_in, doc.store_in_name);
    ui->wPartner->setCodeAndName(doc.partner, doc.partnerName());
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

QToolBar *C5StoreInput::toolBar()
{
    if (!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        mActionSave = fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDocument()));
        mActionDraft = fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDocument()));
        fToolBar->addAction(QIcon(":/new.png"), tr("New\ndocument"), this, [this]() {
            __mainWindow->addWidget(new C5StoreInput(mUser, tr("Store input"), QIcon(":/storage.png")));
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

void C5StoreInput::addByScancode(const QString &code, const QString &qty, QString price)
{
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
    if (mDocData.uuid.isEmpty()) {
        mDocData.uuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
    }
    mDocData.user_id = ui->leDocNum->text();
    mDocData.date = ui->deDate->toMySQLDate(false);
    mDocData.type = DOC_TYPE_STORE_INPUT;
    mDocData.create_user = mUser->id();
    mDocData.store_in = ui->wInputStore->value();
    mDocData.partner = ui->wPartner->value();
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
        if (st.qty < 0.001) {
            err += tr("Quantity not valid on row #") + QString::number(i + 1) + "<br>";
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
    mActionSave->setEnabled(mDocData.status == 0);
    ui->wtoolbar->setEnabled(mActionSave->isEnabled());
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
    if (!buildDoc()) {
        return;
    }
    mDocData.status = 1;
    QJsonObject jdoc = mDocData.toJson();
    NInterface::query1("/engine/v2/common/store-move/input", mUser->mSessionKey, this, {{"doc", jdoc}}, [this](const QJsonObject) {
        mDocData.version++;
        setState();
        C5Message::info(tr("Saved"));
    });
}

void C5StoreInput::draftDocument()
{
    if (!buildDoc()) {
        return;
    }
    mDocData.status = 0;
    QJsonObject jdoc = mDocData.toJson();
    NInterface::query1("/engine/v2/common/store-move/input", mUser->mSessionKey, this, {{"doc", jdoc}}, [this](const QJsonObject) {
        mDocData.version++;
        mActionSave->setEnabled(true);
        mActionDraft->setEnabled(true);
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

void C5StoreInput::removeDocument() {}

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
    C5LineEdit *lprice = ui->tblGoods->lineEdit(row, col_price);
    C5LineEdit *ltotal = ui->tblGoods->lineEdit(row, col_total);
    ltotal->setDouble(lqty->getDouble() * lprice->getDouble());
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
    addByScancode(ui->leScancode->text(), qty, "");

    if (ui->chLeaveFocusOnBarcode->isChecked()) {
        ui->leScancode->setFocus();
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
