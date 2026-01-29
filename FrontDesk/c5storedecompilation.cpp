#include "c5storedecompilation.h"
#include "ui_c5storedecompilation.h"
#include "c5codenameselectorfunctions.h"
#include "c5lineedit.h"
#include "appwebsocket.h"
#include "logwriter.h"
#include "c5message.h"
#include "c5user.h"
#include "format_date.h"
#include "dict_doc_reason.h"
#include "c5utils.h"

#define col1_rec 0
#define col1_goods_id 1
#define col1_goods_name 2
#define col1_barcode 3
#define col1_qty 4
#define col1_unit 5
#define col1_price 6
#define col1_total 7

#define mode_qty 1
#define mode_price 2
#define mode_total 3

C5StoreDecompilation::C5StoreDecompilation(QWidget *parent)
    : C5Widget(parent), ui(new Ui::C5StoreDecompilation)
{
    ui->setupUi(this);
    fLabel = tr("Disassembly");
    fIconName = ":/disassembly.png";
    ui->woutputstore->selectorCallback = storeItemSelector;
    ui->winputstore->selectorCallback = storeItemSelector;
    ui->wcomplect->selectorCallback = makeSelector<GoodsItem>([this](const GoodsItem & g) {
        QString name = ui->wcomplect->name();

        if(!name.isEmpty()) {
            name += " | " + g.barcode;
        }

        ui->wcomplect->setName(name);
    }) ;
    connect(AppWebSocket::instance, &AppWebSocket::bMessageReceived, this, &C5StoreDecompilation::bMessageReceived);
    ui->tblGoods->setColumnWidths(ui->tblGoods->columnCount(), 0, 0, 250, 150, 80, 100, 80, 80);
    mInternalId = QUuid::createUuid().toString(QUuid::WithoutBraces);
}

C5StoreDecompilation::~C5StoreDecompilation()
{
    delete ui;
}

QToolBar* C5StoreDecompilation::toolBar()
{
    if(!fToolBar) {
        fToolBar = createStandartToolbar(QList<ToolBarButtons>());
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveDoc()));
        fToolBar->addAction(QIcon(":/draft.png"), tr("Draft"), this, SLOT(draftDoc()));
        fToolBar->addAction(QIcon(":/recycle.png"), tr("Remove"), this, SLOT(removeDocument()));
        fToolBar->addAction(QIcon(":/print.png"), tr("Print"), this, SLOT(printDoc()));
    }

    return fToolBar;
}

void C5StoreDecompilation::rowTextEdited(const QString &text)
{
    auto *le = static_cast<C5LineEdit*>(sender());

    if(!le) {
        return;
    }

    int r, c;

    if(!ui->tblGoods->findWidget(le, r, c)) {
        return;
    }

    int mode = le->property("mode").toInt();

    switch(mode) {
    case mode_qty:
        ui->tblGoods->lineEdit(r, col1_total)->setDouble(text.toDouble() * ui->tblGoods->lineEdit(r, col1_price)->getDouble());
        break;

    case mode_price:
        ui->tblGoods->lineEdit(r, col1_total)->setDouble(text.toDouble() * ui->tblGoods->lineEdit(r, col1_qty)->getDouble());
        break;

    case mode_total:
        if(ui->tblGoods->lineEdit(r, col1_qty)->getDouble() > 0.01) {
            ui->tblGoods->lineEdit(r, col1_price)->setDouble(text.toDouble() / ui->tblGoods->lineEdit(r, col1_qty)->getDouble());
        } else {
            ui->tblGoods->lineEdit(r, col1_price)->setDouble(0);
        }

        break;
    }
}

void C5StoreDecompilation::saveDoc()
{
    QString err;

    if(ui->woutputstore->value() == 0) {
        err += tr("Output store is not defined") + "<br>";
    }

    if(ui->winputstore->value() == 0) {
        err += tr("Input store is not defined") + "<br>";
    }

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if(ui->tblGoods->lineEdit(i, col1_qty)->getDouble() < 0.001) {
            err += tr("Row") + " #" + QString::number(i + 1) + " " + tr("has no quantity") + "<br>";
        }
    }

    if(!err.isEmpty()) {
        C5Message::error(err);
        return;
    }

    QJsonObject jdoc;
    QJsonObject jheader;
    jheader["f_id"] = mInternalId;
    jheader["f_userid"] = ui->leDocNum->text();
    jheader["f_state"] = 1;
    jheader["f_type"] = 8;
    jheader["f_date"] = ui->leDate->date().toString(FORMAT_DATE_TO_STR_MYSQL);
    jheader["f_operator"] = mUser->id();
    jheader["f_partner"] = QJsonValue::Null;;
    jheader["f_amount"] = 0;
    jheader["f_currency"] = 1;
    jheader["f_storein"] = ui->winputstore->value();
    jheader["f_storeout"] = ui->woutputstore->value();
    jheader["f_reason"] = DOC_REASON_DISASSMLY;
    jheader["f_comment"] = ui->leComment->text();
    jheader["f_payment"] =  QJsonValue::Null;
    jheader["f_paid"] = 0;
    jdoc["header"] = jheader;
    QJsonObject jbody;
    jbody["f_accepted"] = mUser->id();
    jbody["f_passed"] = mUser->id();
    jbody["f_invoice"] = "";
    jbody["f_invoicedate"] = QJsonValue::Null;
    jbody["f_reason"] = DOC_REASON_DISASSMLY;
    jheader["f_storein"] = ui->winputstore->value();
    jheader["f_storeout"] = ui->woutputstore->value();
    jbody["f_complectationcode"] = 0;
    jbody["f_complectationqty"] = 0;
    jbody["f_cashdoc"] = "";
    jbody["f_basedonsale"] = 0;
    QJsonArray jadd;
    jdoc["add"] = jadd;
    QJsonObject jcomplect;
    jcomplect["f_goods"] = 0;
    jcomplect["f_qty"] = 0;
    jdoc["complect"] = jcomplect;
    QJsonArray jgoodsOut;
    QJsonArray jgoodsIn;
    QJsonObject jremains;

    for(int i = 0; i < ui->tblGoods->rowCount(); i++) {
        QJsonObject jitem;
        jitem["f_goods"] = ui->tblGoods->getInteger(i, col1_goods_id);
        jitem["f_qty"] = ui->tblGoods->lineEdit(i, col1_qty)->getDouble();
        jitem["f_price"] = 0;
        jitem["f_validto"] = QJsonValue::Null;
        jitem["f_comment"] = "";
        jitem["f_row"] = i;
        jgoodsIn.append(jitem);
    }

    jbody["remains"] = jremains;
    jdoc["goodsOut"] = jgoodsOut;
    jdoc["goodsIn"] = jgoodsIn;
    QJsonObject jpartner;
    jpartner["partner"] = 0;
    jpartner["paid"] = 0;
    jpartner["cash"] = 0;
    jdoc["partner"] = jpartner;
    jdoc["body"] = jbody;
    QString session = QUuid::createUuid().toString(QUuid::WithoutBraces);
    jdoc["session"] = session;
    NInterface::query1("/engine/v2/officen/documents/save-store-doc", mUser->mSessionKey, this, jdoc,
    [this](const QJsonObject & jdoc) {
        QJsonArray jg = jdoc["complect_items"].toArray();
        ui->tblGoods->clearContents();
        ui->tblGoods->setRowCount(0);

        for(int i = 0; i < jg.size(); i++) {
            GoodsItem gi = JsonParser<GoodsItem>::fromJson(jg.at(i).toObject());
            addGoods1Row(gi);
        }
    });
}

void C5StoreDecompilation::bMessageReceived(const QJsonObject &jo)
{
    LogWriter::write(LogWriterLevel::verbose, "C5StoreDecompilation::bMessageReceived", QJsonDocument(jo).toJson());

    if(jo.value("requestId").toString() != mLastQuery) {
        LogWriter::write(LogWriterLevel::verbose, "C5StoreDecompilation::bMessageReceived", "ignoring by requestId");
        return;
    }

    QJsonArray jr = jo.value("result").toArray();

    if(jr.isEmpty()) {
        return;
    }

    if(jr.size() > 1) {
        C5Message::info(tr("Multiple rows in result, getting first"));
    }

    if(jo.value("actionId").toString() == "search_complect_barcode") {
        GoodsItem gi = JsonParser<GoodsItem>::fromJson(jr.first().toObject());
        ui->wcomplect->setCodeAndName(gi.id, gi.name);
    }
}

void C5StoreDecompilation::on_btnAddComplect_clicked()
{
    auto r = selectItem<GoodsItem>(false, false, ui->btnAddComplect->mapToGlobal(QPoint(0, ui->btnAddComplect->height())));

    if(r.isEmpty()) {
        return;
    }

    addGoods1Row(r.first());
}

void C5StoreDecompilation::on_leBarcode_returnPressed()
{
    QString barcode = ui->leBarcode->text().trimmed();
    ui->leBarcode->clear();

    if(barcode.isEmpty()) {
        return;
    }

    mLastQuery = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QJsonObject jo;
    jo["command"] = SelectorName<GoodsItem>::value;
    jo["barcode"] = barcode;
    jo["requestId"] = mLastQuery;
    jo["actionId"] = "search_complect_barcode";
    qDebug() << "Sending request" << jo;
    AppWebSocket::instance->sendBinaryMessage(QJsonDocument(jo).toJson());
}

int C5StoreDecompilation::addGoods1Row(GoodsItem g)
{
    int row = ui->tblGoods->addEmptyRow();
    ui->tblGoods->setInteger(row, 1, g.id);
    ui->tblGoods->setString(row, 2, g.name);
    ui->tblGoods->setString(row, 3, g.barcode);
    auto *leQty = ui->tblGoods->createWidget<C5LineEdit>(row, 4);
    leQty->setProperty("mode", mode_qty);
    leQty->setValidator(new QDoubleValidator(0, 999999, 2));
    leQty->setFocus();
    ui->tblGoods->setString(row, 5, g.unitName);
    auto *lePrice = ui->tblGoods->createLineEdit(row, col1_price);
    lePrice->setProperty("mode", mode_price);
    lePrice->setPlaceholderText(float_str(g.lastInputPrice, 2));
    auto *leTotal = ui->tblGoods->createLineEdit(row, col1_total);
    leTotal->setProperty("mode", mode_total);
    connect(leQty, &C5LineEdit::textEdited, this, &C5StoreDecompilation::rowTextEdited);
    connect(lePrice, &C5LineEdit::textEdited, this, &C5StoreDecompilation::rowTextEdited);
    connect(leTotal, &C5LineEdit::textEdited, this, &C5StoreDecompilation::rowTextEdited);
    return row;
}

void C5StoreDecompilation::on_btnFillComplect_clicked()
{
    if(ui->wcomplect->value() == 0) {
        C5Message::error(tr("Select complect"));
        return;
    }

    NInterface::query1("/engine/v2/officen/complect/get", mUser->mSessionKey, this,
    {{"complect_id", ui->wcomplect->value()}},
    [this](const QJsonObject & jdoc) {
        QJsonArray jg = jdoc["complect_items"].toArray();
        ui->tblGoods->clearContents();
        ui->tblGoods->setRowCount(0);

        for(int i = 0; i < jg.size(); i++) {
            auto const &jo = jg.at(i).toObject();
            GoodsItem gi = JsonParser<GoodsItem>::fromJson(jo);
            int row = addGoods1Row(gi);
            ui->tblGoods->setDouble(row, col1_qty, jo.value("f_qty").toDouble());
            ui->tblGoods->lineEdit(row, col1_qty)->setDouble(jo.value("f_qty").toDouble() * ui->leQty->getDouble());
        }
    });
}

void C5StoreDecompilation::on_btnRemove_clicked()
{
    auto ml = ui->tblGoods->selectionModel()->selectedRows();

    if(ml.isEmpty()) {
        return;
    }

    std::sort(ml.begin(), ml.end(), [](const QModelIndex & a, const QModelIndex & b) {
        return a.row() > b.row();
    });

    for(const auto &index : ml) {
        ui->tblGoods->removeRow(index.row());
    }
}
