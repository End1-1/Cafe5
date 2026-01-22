#include "cr5consumptionbysales.h"
#include "cr5consumptionbysalesfilter.h"
#include "c5gridgilter.h"
#include "c5tablemodel.h"
#include "c5mainwindow.h"
#include "c5waiterorderdoc.h"
#include "c5progressdialog.h"
#include "c5user.h"
#include "c5srofinventory.h"
#include "c5double.h"
#include "dlgchangeoutputstore.h"
#include "c5storedraftwriter.h"
#include "c5message.h"
#include "c5config.h"
#include "c5storedoc.h"
#include "cr5goodsmovement.h"
#include "dlgsemireadyinout.h"
#include "cr5consuptionbysalesqty.h"
#include <QInputDialog>

static const int col_goods = 0;
static const int col_goodsgroup = 1;
static const int col_goodsname = 2;
static const int col_goodscode = 3;
static const int col_qtybefore = 4;
static const int col_qtyinput = 5;
static const int col_qtysale = 6;
static const int col_qtyout = 7;
static const int col_qtystore = 8;
static const int col_qtyafter = 9;
static const int col_qtyinv = 10;
static const int col_qtydiff = 11;
static const int col_qtyunit = 12;
static const int col_qty_sr = 13;

CR5ConsumptionBySales::CR5ConsumptionBySales(QWidget *parent) :
    C5ReportWidget(parent)
{
    fIconName = ":/goods.png";
    fLabel = tr("Consumption of goods base on sales");
    fColumnNameIndex["f_goods"] = col_goods;
    fColumnNameIndex["f_goodsgroup"] = col_goodsgroup;
    fColumnNameIndex["f_goodsname"] = col_goodsname;
    fColumnNameIndex["f_goodscode"] = col_goodscode;
    fColumnNameIndex["f_qtybefore"] = col_qtybefore;
    fColumnNameIndex["f_qtyinput"] = col_qtyinput;
    fColumnNameIndex["f_qtysale"] = col_qtysale;
    fColumnNameIndex["f_qtyout"] = col_qtyout;
    fColumnNameIndex["f_qtystore"] = col_qtystore;
    fColumnNameIndex["f_qtyafter"] = col_qtyafter;
    fColumnNameIndex["f_qtyinv"] = col_qtyinv;
    fColumnNameIndex["f_qtydiff"] = col_qtydiff;
    fColumnNameIndex["f_qtyunit"] = col_qtyunit;
    fColumnNameIndex["f_qty_sr"] = col_qty_sr;
    fTranslation["f_goods"] = tr("Goods code");
    fTranslation["f_goodsgroup"] = tr("Group");
    fTranslation["f_goodsname"] = tr("Goods name");
    fTranslation["f_goodscode"] = tr("Scancode");
    fTranslation["f_qtybefore"] = tr("Before");
    fTranslation["f_qtyinput"] = tr("Input");
    fTranslation["f_qtysale"] = tr("Sale");
    fTranslation["f_qtyout"] = tr("Out");
    fTranslation["f_qtystore"] = tr("Store");
    fTranslation["f_qtyafter"] = tr("After");
    fTranslation["f_qtyinv"] = tr("Inventory");
    fTranslation["f_qtydiff"] = tr("Diff");
    fTranslation["f_qtyunit"] = tr("Unit");
    fTranslation["f_qty_sr"] = tr("S/R");
    fColumnsSum << "f_qtybefore"
                << "f_qtyinput"
                << "f_qtysale"
                << "f_qtyout"
                << "f_qtystore"
                << "f_qtyafter"
                << "f_qtyinv"
                << "f_qtydiff";
    fFilterWidget = new CR5ConsumptionBySalesFilter();
    fFilter = static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget);
}

QToolBar* CR5ConsumptionBySales::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbFilter
            << ToolBarButtons::tbClearFilter
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
        fToolBar = createStandartToolbar(btn);
        QAction *a = new QAction(QIcon(":/goods.png"), tr("Make\ninput/output"), this);
        connect(a, SIGNAL(triggered(bool)), this, SLOT(makeOutput(bool)));
        fToolBar->insertAction(fToolBar->actions().at(0), a);
        auto *b = new QAction(QIcon(":/calculator.png"), tr("Count output\nbase on recipes"), this);
        connect(b, SIGNAL(triggered(bool)), this, SLOT(countOutputBasedOnRecipes()));

        if(__c5config.frontDeskMode() == FRONTDESK_WAITER) {
            fToolBar->insertAction(a, b);
        }

        auto *d = new QAction(QIcon(":/decision.png"), tr("Change the store of output"), this);
        connect(d, SIGNAL(triggered(bool)), this, SLOT(changeOutputStore()));
        fToolBar->insertAction(b, d);
        auto *c = new QAction(QIcon(":/goods.png"), tr("Sales\noutput"), this);
        connect(c, SIGNAL(triggered(bool)), this, SLOT(salesOutput(bool)));
        fToolBar->insertAction(a, c);
        auto *e = new QAction(QIcon(":/goodsback.png"), tr("Rollback goods output"), this);
        connect(e, SIGNAL(triggered(bool)), this, SLOT(rollbackGoodsOutput(bool)));
        fToolBar->insertAction(c, e);
        auto *f = new QAction(QIcon(":/goodsback.png"), tr("Get from AS"), this);
        connect(f, SIGNAL(triggered(bool)), this, SLOT(asinput(bool)));
        fToolBar->insertAction(e, f);
        auto *g = new QAction(QIcon(":/goodsback.png"), tr("Output to AS"), this);
        connect(g, SIGNAL(triggered(bool)), this, SLOT(asoutput(bool)));
        fToolBar->insertAction(e, g);
        auto *h = new QAction(QIcon(":/update_prices.png"), tr("Update prices\nof inventorization"), this);
        connect(h, SIGNAL(triggered(bool)), this, SLOT(updateInventorizatinPrices()));
        fToolBar->addAction(h);
        auto *i = new QAction(QIcon(":/goods.png"), tr("Semiready\nin/out"), this);
        connect(i, SIGNAL(triggered(bool)), this, SLOT(semireadyInOut()));
        fToolBar->addAction(i);
    }

    return fToolBar;
}

void CR5ConsumptionBySales::buildQuery()
{
    CR5ConsumptionBySalesFilter *f = static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget);

    if(f->store() ==  0) {
        if(!C5GridGilter::filter(mUser, f, fWhereCondition, fColumnsVisible, fTranslation)) {
            return;
        }
    }

    if(f->store() == 0) {
        C5Message::info(tr("Store must be defined"));
        return;
    }

    auto &rows = fModel->fRawData;
    rows.clear();
    QMap<int, int> goodsMap;
    QString cond = " where c.f_id>0 ";

    if(fFilter->group().length() > 0) {
        cond += " and c.f_group in (" + fFilter->group() + ") ";
    }

    C5Database db;
    /* get all goods */
    db.exec(QString("select c.f_id, g.f_name as f_groupname, c.f_name, c.f_scancode, "
                    "u.f_name as f_unitname "
                    "from c_goods c "
                    "left join c_groups g on g.f_id=c.f_group "
                    "left join c_units u on u.f_id=c.f_unit %1 "
                    "order by 2, 3").arg(cond));

    while(db.nextRow()) {
        QJsonArray row;
        row << db.getInt("f_id")
            << db.getString("f_groupname")
            << db.getString("f_name")
            << db.getString("f_scancode")
            << QJsonValue()
            << QJsonValue()
            << QJsonValue()
            << QJsonValue()
            << QJsonValue()
            << QJsonValue()
            << QJsonValue()
            << QJsonValue()
            << db.getString("f_unitname")
            << QJsonValue();
        rows.push_back(row);
        goodsMap[db.getInt("f_id")] = rows.size() - 1;
    }

    /* get qty before */
    db[":f_store"] = f->store();
    db[":f_date"] = f->date1().addDays(-1);
    db.exec(QString("select s.f_goods, sum(%1*s.f_type) as f_qty "
                    "from a_store_draft s "
                    "inner join a_header d on d.f_id=s.f_document "
                    "where s.f_store=:f_store and d.f_date<=:f_date "
                    "group by 1 ")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "s.f_total" : "s.f_qty"));

    while(db.nextRow()) {
        if(!goodsMap.contains(db.getInt(0))) {
            continue;
        }

        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtybefore] = db.getDouble("f_qty");
    }

    /* get input */
    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_type"] = 1;
    db[":f_store"] = f->store();
    db[":f_state"] = DOC_STATE_SAVED;
    db.exec(QString("select s.f_goods, sum(%1) "
                    "from a_store_draft s "
                    "left join a_header h on h.f_id=s.f_document "
                    "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
                    "and h.f_state=:f_state and s.f_type=:f_type "
                    "group by 1 ")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "s.f_total" : "s.f_qty"));

    while(db.nextRow()) {
        if(!goodsMap.contains(db.getInt(0))) {
            continue;
        }

        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyinput] = db.getDouble(1);
    }

    /* get output based on recipes */
    QString cond1;

    if(!fFilter->draft()) {
        db[":f_state"] = DOC_STATE_SAVED;
        cond1 += " and h.f_state=:f_state ";
    }

    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_type"] = DOC_TYPE_STORE_OUTPUT;
    db[":f_reason"] = DOC_REASON_SALE;
    db.exec(QString("select s.f_goods, sum(%1) as f_qty "
                    "from a_store_draft s "
                    "inner join a_header h on h.f_id=s.f_document "
                    "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
                    "and s.f_reason=:f_reason and h.f_type=:f_type " + cond1 +
                    "group by 1 ")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "s.f_total" : "s.f_qty"));

    while(db.nextRow()) {
        if(!goodsMap.contains(db.getInt(0))) {
            continue;
        }

        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtysale] = db.getDouble("f_qty");
    }

    /* get output */
    db[":f_store"] = f->store();
    db[":f_date1"] = f->date1();
    db[":f_date2"] = f->date2();
    db[":f_type"] = -1;

    if(!fFilter->draft()) {
        db[":f_state"] = DOC_STATE_SAVED;
        cond1 += " and h.f_state=:f_state ";
    } else {
        db[":f_state"] = DOC_STATE_SAVED;
    }

    db[":f_reason"] = DOC_REASON_SALE;
    db.exec(QString("select s.f_goods, sum(%1) as f_qty "
                    "from a_store_draft s "
                    "left join a_header h on h.f_id=s.f_document "
                    "where h.f_date between :f_date1 and :f_date2 and s.f_store=:f_store "
                    "and s.f_type=:f_type  "
                    "and s.f_reason<>:f_reason " + cond1 +
                    "group by 1 ")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "s.f_total" : "s.f_qty"));

    while(db.nextRow()) {
        if(!goodsMap.contains(db.getInt(0))) {
            continue;
        }

        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyout] = db.getDouble(1);
    }

    /* get store at the date end */
    db[":f_store"] = f->store();
    db[":f_date"] = f->date2();
    db.exec(QString("select s.f_goods, sum(%1*s.f_type) as f_qty "
                    "from a_store_draft s "
                    "inner join a_header d on d.f_id=s.f_document "
                    "where s.f_store=:f_store and d.f_date<=:f_date "
                    "group by 1 ")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "s.f_total" : "s.f_qty"));

    while(db.nextRow()) {
        if(!goodsMap.contains(db.getInt(0))) {
            continue;
        }

        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtystore] = db.getDouble("f_qty");
    }

    /* get counted qty */
    for(int i = 0; i < rows.size(); i++) {
        rows[i][col_qtyafter] = rows[i][col_qtybefore].toDouble()
                                + rows[i][col_qtyinput].toDouble()
                                - rows[i][col_qtysale].toDouble()
                                - rows[i][col_qtyout].toDouble();
    }

    /* get inventory quantities */
    db[":f_date"] = f->date2();
    db[":f_store"] = f->store();
    db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
    db.exec(QString("select i.f_goods, g.f_name, sum(%1) as f_qty "
                    "from a_store_inventory i "
                    "inner join a_header h on h.f_id=i.f_document "
                    "inner join c_goods g on g.f_id=i.f_goods "
                    "where h.f_date=:f_date and h.f_type=:f_type "
                    "and i.f_store=:f_store "
                    "group by 1, 2 ")
            .arg(fFilter->reportType() == REPORTTYPE_AMOUNTS ? "i.f_total" : "i.f_qty"));

    while(db.nextRow()) {
        if(!goodsMap.contains(db.getInt(0))) {
            continue;
        }

        int r = goodsMap[db.getInt(0)];
        rows[r][col_qtyinv] = db.getDouble("f_qty");
    }

    /* GET S/R quantities */
    db[":f_date"] = f->date2();
    db[":f_store"] = f->store();
    db.exec("select i.f_goods, sum(i.f_goods_qty) as f_qty "
            "from a_header_sr i "
            "where i.f_date=:f_date "
            "and i.f_store=:f_store "
            "group by 1 ");

    while(db.nextRow()) {
        if(!goodsMap.contains(db.getInt(0))) {
            continue;
        }

        int r = goodsMap[db.getInt(0)];
        rows[r][col_qty_sr] = db.getDouble("f_qty");
    }

    //remove unused
    for(int i  = rows.size() - 1; i > -1; i--) {
        if(zerodouble(rows[i][col_qtybefore].toDouble())
                && zerodouble(rows[i][col_qtysale].toDouble())
                && zerodouble(rows[i][col_qtyout].toDouble())
                && zerodouble(rows[i][col_qtystore].toDouble())
                && zerodouble(rows[i][col_qtyafter].toDouble())
                && zerodouble(rows[i][col_qtyinv].toDouble())
                && zerodouble(rows[i][col_qtydiff].toDouble())) {
            rows.erase(rows.begin() + i);
        }
    }

    fModel->setExternalData(fColumnNameIndex, fTranslation);
    fModel->fColumnType = {{0, QMetaType::Int},
        {1, QMetaType::Int},
        {2, QMetaType::QString},
        {3, QMetaType::Double},
        {4, QMetaType::Double},
        {5, QMetaType::Double},
        {6, QMetaType::Double},
        {7, QMetaType::Double},
        {8, QMetaType::Double},
        {9, QMetaType::Double},
        {10, QMetaType::Double},
        {11, QMetaType::Double},
        {12, QMetaType::QString},
        {13, QMetaType::Double}
    };

    for(int i  = rows.size() - 1; i > -1; i--) {
        countRowQty(i);
    }

    fColumnsVisible["f_goods"] = true;
    fColumnsVisible["f_goodsgroup"] = true;
    fColumnsVisible["f_goodsname"] = true;
    fColumnsVisible["f_qtybefore"] = true;
    fColumnsVisible["f_qtyinput"] = true;
    fColumnsVisible["f_qtysale"] = true;
    fColumnsVisible["f_qtyout"] = true;
    fColumnsVisible["f_qtystore"] = true;
    fColumnsVisible["f_qtyafter"] = true;
    fColumnsVisible["f_qtyinv"] = true;
    fColumnsVisible["f_qtydiff"] = true;
    restoreColumnsWidths();
    sumColumnsData();
    emit refreshed();
}

void CR5ConsumptionBySales::refreshData()
{
    buildQuery();
}

QString CR5ConsumptionBySales::documentForInventory(int store)
{
    QString result;
    CR5ConsumptionBySalesFilter *f = static_cast<CR5ConsumptionBySalesFilter*>(fFilterWidget);
    C5Database db;
    db[":f_date"] = f->date2();
    db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
    db.exec("select f_id from a_header where f_date=:f_date and f_type=:f_type");

    while(db.nextRow()) {
        C5Database db2;
        db2[":f_document"] = db.getString(0);
        db2[":f_store"] = store;
        db2.exec("select * from a_store_inventory where f_document=:f_document and f_store=:f_store limit 1");

        if(db2.nextRow()) {
            if(!result.isEmpty()) {
                result += ",";
            }

            result += QString("'%1'").arg(db.getString(0));
        }
    }

    if(result.isEmpty()) {
        C5StoreDraftWriter dw(db);
        dw.writeAHeader(result, QString::number(dw.counterAType(DOC_TYPE_STORE_INVENTORY)), DOC_STATE_SAVED,
                        DOC_TYPE_STORE_INVENTORY, mUser->id(), f->date2(), QDate::currentDate(), QTime::currentTime(), 0, 0,
                        tr("Created automaticaly"), 0, __c5config.getValue(param_default_currency).toInt());
        result = QString("'%1'").arg(result);
    }

    return result;
}

void CR5ConsumptionBySales::countRowQty(int row)
{
    double qtySale = 0.0;

    if(!fFilter->salesCounted()) {
        qtySale = fModel->data(row, col_qtysale, Qt::EditRole).toDouble();
    }

    fModel->setData(row, col_qtyafter,
                    fModel->data(row, col_qtybefore, Qt::EditRole).toDouble()
                    + fModel->data(row, col_qtyinput, Qt::EditRole).toDouble()
                    - qtySale
                    - fModel->data(row, col_qtyout, Qt::EditRole).toDouble());
    fModel->setData(row, col_qtydiff,
                    fModel->data(row, col_qtyinv, Qt::EditRole).toDouble()
                    - fModel->data(row, !fFilter->draft() ? col_qtyafter : col_qtystore, Qt::EditRole).toDouble());
}

void CR5ConsumptionBySales::writeInvQty(C5Database &db, double qty, int row, int column, int goods, bool checkSR)
{
    C5StoreDraftWriter dw(db);

    if(checkSR) {
        if(qty < fModel->data(row, col_qty_sr, Qt::EditRole).toDouble()) {
            C5Message::error(tr("Could not remove quantity of S/R"));
            return;
        }
    }

    QString docid = documentForInventory(fFilter->store());
    db[":f_goods"] = goods;
    db.exec(QString("delete from a_store_inventory where f_document in (%1) and f_goods=:f_goods").arg(docid));
    QString id;

    if(qty > 0.0001) {
        QString d = docid.split(",", Qt::SkipEmptyParts).at(0);
        dw.writeAStoreInventory(id, d.replace("'", ""), fFilter->store(), goods, qty, 0, 0);
    }

    fModel->setData(row, column, qty);
    countRowQty(row);
}

void CR5ConsumptionBySales::storeOutError(const QString &err)
{
    sender()->deleteLater();
    //C5Message::error(err);
    // if (fPd) {
    //     fPd->deleteLater();
    //     fPd = nullptr;
    // }
}

void CR5ConsumptionBySales::storeoutResponse(const QJsonObject &jdoc)
{
    QString id = jdoc["id"].toString();
    fHttp->httpQueryFinished(sender());

    for(int i = 0; i < fIDs.count(); i++) {
        QStringList &sl = fIDs[i];

        if(sl.contains(id)) {
            sl.removeOne(id);
            id = "";

            if(!sl.isEmpty()) {
                id = sl.first();
            }
        }
    }

    int remain = 0;

    for(const QStringList &sl : fIDs) {
        remain += sl.size();
    }

    if(!id.isEmpty()) {
        emit updateProgressValue(remain);
        fHttp->createHttpQuery("/engine/worker/create-store-out.php",
        QJsonObject{{"id", id}, {"action", "handle"}},
        SLOT(storeoutResponse(QJsonObject)),
        id, false);
    }

    if(fPd && remain == 0) {
        fPd->deleteLater();
        fPd = nullptr;
    }
}

bool CR5ConsumptionBySales::tblDoubleClicked(int row, int column, const QJsonArray &values)
{
    if(values.count() == 0) {
        return true;
    }

    double qty;
    C5Database db;

    switch(column) {
    case col_qtyinput: {
        auto *ddi = __mainWindow->createTab<CR5GoodsMovement>();
        ddi->setDocType(QString("%1,%2").arg(DOC_TYPE_STORE_INPUT).arg(DOC_TYPE_STORE_MOVE));
        ddi->setDate(fFilter->date1(), fFilter->date2());
        ddi->setStore(QString::number(fFilter->store()));
        ddi->setGoods(QString::number(values.at(0).toInt()));
        ddi->setReason(QString("%1,%2, %3").arg(DOC_REASON_INPUT).arg(DOC_REASON_MOVE).arg(DOC_REASON_SALE_RETURN));
        ddi->setInOut(1);
        ddi->buildQuery();
        break;
    }

    case col_qtyout: {
        auto *ddo = __mainWindow->createTab<CR5GoodsMovement>();
        ddo->setDate(fFilter->date1(), fFilter->date2());
        ddo->setDocType(QString("%1,%2").arg(DOC_TYPE_STORE_OUTPUT).arg(DOC_TYPE_STORE_MOVE));
        ddo->setStore(QString::number(fFilter->store()));
        ddo->setGoods(QString::number(values.at(0).toInt()));
        ddo->setReason(QString("%1,%2,%3").arg(DOC_REASON_OUT).arg(DOC_REASON_MOVE).arg(DOC_REASON_LOST));
        ddo->setInOut(-1);
        ddo->buildQuery();
        break;
    }

    case col_qtysale: {
        __mainWindow->createNTab("/engine/reports/consuption-reason.php",
                                 ":/cash.png",
        QJsonObject{
            {"date1", fFilter->date1().toString(FORMAT_DATE_TO_STR_MYSQL)},
            {"date2", fFilter->date2().toString(FORMAT_DATE_TO_STR_MYSQL)},
            {"store", fFilter->store()},
            {"goods", values.at(0).toInt()},
            {"date1", fFilter->date1().toString(FORMAT_DATE_TO_STR_MYSQL)},
            {"date2", fFilter->date2().toString(FORMAT_DATE_TO_STR_MYSQL)},
            {"store", fFilter->store()},
            {"goods", values.at(0).toInt()}
        });
        //OLD VERSION
        // CR5ConsuptionReason *s = __mainWindow->createTab<CR5ConsuptionReason>();
        // s->setFilterParams(fFilter->date1(), fFilter->date2(), fFilter->store(), values.at(0).toInt());
        break;
    }

    case col_qtyinv: {
        switch(cr5consuptionbysalesqty::qty(this, qty)) {
        case cr5consuptionbysalesqty::rtOk:
            writeInvQty(db, qty, row, column, values.at(0).toInt());
            break;

        case cr5consuptionbysalesqty::rtCancel:
            break;

        case cr5consuptionbysalesqty::rtAdd:
            qty += fModel->data(row, col_qtyinv, Qt::EditRole).toDouble();
            writeInvQty(db, qty, row, column, values.at(0).toInt());

        case cr5consuptionbysalesqty::rtClear:
            writeInvQty(db, 0, row, column, values.at(0).toInt());
            fModel->setData(row, col_qtyinv, "", Qt::EditRole);
            break;
        }

        break;
    }

    case col_qty_sr:
        C5SrOfInventory soi(mUser);
        soi.setGoods(fFilter->date2(), fFilter->store(), values.at(0).toInt());

        if(soi.exec() == QDialog::Accepted) {
            db[":f_rec"] = soi.fUuid;
            db.exec("select * from a_header_sr where f_rec=:f_rec");
            QMap<int, double> goods;

            while(db.nextRow()) {
                goods[db.getInt("f_goods")] = db.getDouble("f_goods_qty");
            }

            for(QMap<int, double>::const_iterator it = goods.begin(); it != goods.end(); it++) {
                for(int i = 0; i < fModel->rowCount(); i++) {
                    if(fModel->data(i, 0, Qt::EditRole).toInt() == it.key()) {
                        writeInvQty(db, it.value(), i, col_qtyinv, it.key(), false);
                        fModel->setData(i, col_qty_sr, fModel->data(i, col_qty_sr, Qt::EditRole).toDouble() - it.value());
                    }
                }
            }

            db[":f_rec"] = soi.fUuid;
            db.exec("delete from a_header_sr where f_rec=:f_rec");
        }

        break;
    }

    return true;
}

void CR5ConsumptionBySales::makeOutput(bool v)
{
    Q_UNUSED(v);
    QMap<int, double> goodsSale, goodsLost, goodsOver;
    std::vector<QJsonArray>& rows = fModel->fRawData;

    for(int i = 0; i < rows.size(); i++) {
        // if (rows[i][col_qtyinv].toString().isEmpty()) {
        //     //continue;
        // }
        double qty = rows[i][col_qtydiff].toDouble();

        if(rows[i][col_qtydiff].toDouble() > 0.0001) {
            qty = rows[i][col_qtydiff].toDouble();
        } else if(rows[i][col_qtydiff].toDouble() < -0.0001) {
            qty = rows[i][col_qtydiff].toDouble();
        }

        if(qty > 0.0001) {
            goodsOver[rows[i][col_goods].toInt()] = qty;
        }

        if(qty  < -0.0001) {
            goodsLost[rows[i][col_goods].toInt()] = qty * -1;
        }
    }

    if(goodsSale.count() > 0) {
        QList<OGoods> gl;

        for(QMap<int, double>::const_iterator it = goodsSale.begin(); it != goodsSale.end(); it++) {
            OGoods g;
            g.goods = it.key();
            g.qty = it.value();
            gl.append(g);
        }

        writeDocs(DOC_TYPE_STORE_OUTPUT, DOC_REASON_SALE, gl, tr("Sale"));
    }

    if(goodsLost.count() > 0) {
        QList<OGoods> gl;

        for(QMap<int, double>::const_iterator it = goodsLost.constBegin(); it != goodsLost.constEnd(); it++) {
            OGoods g;
            g.goods = it.key();
            g.qty = it.value();
            gl.append(g);
        }

        writeDocs(DOC_TYPE_STORE_OUTPUT, DOC_REASON_LOST, gl, tr("Lost"));
    }

    if(goodsOver.count() > 0) {
        QList<OGoods> gl;

        for(QMap<int, double>::const_iterator it = goodsOver.begin(); it != goodsOver.end(); it++) {
            OGoods g;
            g.goods = it.key();
            g.qty = it.value();
            gl.append(g);
        }

#ifdef NEWVERSION
        C5StoreDoc *sd = writeDocs(DOC_TYPE_STORE_INPUT, DOC_REASON_OVER, gl, tr("Over"));

        if(sd) {
            sd->setLastInputPrices();
        }

        __mainWindow->removeTab(sd);
#else
        C5StoreDoc *sd = writeDocs(DOC_TYPE_STORE_INPUT, DOC_REASON_OVER, gl, tr("Over"));

        if(sd) {
            sd->setLastInputPrices();
        }

#endif
    }
}

void CR5ConsumptionBySales::salesOutput(bool v)
{
    Q_UNUSED(v);
    QMap<int, double> goodsSale;
    std::vector<QJsonArray >& rows = fModel->fRawData;

    for(int i = 0; i < rows.size(); i++) {
        double qty = rows[i][col_qtysale].toDouble();

        if(qty > 0.0001) {
            goodsSale[rows[i][col_goods].toInt()] = qty;
        }
    }

    if(goodsSale.count() > 0) {
        QList<OGoods> gl;

        for(QMap<int, double>::const_iterator it = goodsSale.constBegin(); it != goodsSale.constEnd(); it++) {
            OGoods g;
            g.goods = it.key();
            g.qty = it.value();
            gl.append(g);
        }

        writeDocs(DOC_TYPE_STORE_OUTPUT, DOC_REASON_SALE, gl, tr("Sale"));
    }
}

void CR5ConsumptionBySales::rollbackGoodsOutput(bool v)
{
    Q_UNUSED(v);
    C5Database db;
    C5WaiterOrderDoc::clearStoreOutput(db, fFilter->date1(), fFilter->date2());
    C5Message::info(tr("Done"));
}

void CR5ConsumptionBySales::countOutputBasedOnRecipes()
{
    fPd = new C5ProgressDialog(this);
    connect(this, SIGNAL(updateProgressValue(int)), fPd, SLOT(updateProgressValue(int)));
    C5Database db;
    db[":d1"] = fFilter->date1();
    db[":d2"] = fFilter->date2();
    db.exec("select * from o_header where f_datecash between :d1 and :d2 and f_state=2");
    int remain = db.rowCount();
    int threads = 30;
    fIDs.clear();

    for(int i = 0; i < threads; i++) {
        fIDs.append(QStringList());
    }

    int r = 0;

    while(db.nextRow()) {
        fIDs[r++ % threads].append(db.getString(0));
    }

    if(r == 0) {
        return;
    }

    fPd->setMax(remain);
    fPd->show();
    emit updateProgressValue(fIDs.size());
    fHttp->fErrorSlot = (char*) SLOT(storeOutError(QString));
    fHttp->fErrorObject = this;

    for(int i = 0; i < threads; i++) {
        if(!fIDs[i].isEmpty()) {
            fHttp->createHttpQuery("/engine/worker/create-store-out.php",
            QJsonObject{{"id", fIDs[i].first()}, {"action", "handle"}},
            SLOT(storeoutResponse(QJsonObject)),
            fIDs[i].first(), false);
        }
    }
}

void CR5ConsumptionBySales::changeOutputStore()
{
    DlgChangeOutputStore *d = new DlgChangeOutputStore(mUser);
    d->refresh(fFilter->date1(), fFilter->date2());
    d->exec();
    delete d;
}

void CR5ConsumptionBySales::updateInventorizatinPrices()
{
    C5Database db;
    db[":f_store"] = fFilter->store();
    db[":f_date"] = fFilter->date2();
    db.exec(QString("select s.f_goods, sum(f_qty*s.f_type*f_price) / sum(f_qty*s.f_type) as f_price "
                    "from a_store s "
                    "inner join a_header d on d.f_id=s.f_document "
                    "where s.f_store=:f_store and d.f_date<=:f_date "
                    "group by 1 "));
    QMap<int, double> prices;

    while(db.nextRow()) {
        prices[db.getInt("f_goods")] = db.getDouble("f_price");
    }

    for(int i = 0; i < fModel->rowCount(); i++) {
        db[":f_type"] = DOC_TYPE_STORE_INVENTORY;
        db[":f_store"] = fFilter->store();
        db[":f_date"] = fFilter->date2();
        db[":f_goods"] = fModel->data(i, col_goods, Qt::EditRole);
        db[":f_price"] = prices[fModel->data(i, col_goods, Qt::EditRole).toInt()];
        db.exec("update a_store_inventory set f_price=:f_price, f_total=f_qty*:f_price where f_goods=:f_goods and f_store=:f_store "
                " and f_document in (select f_id from a_header where f_type=:f_type and f_date=:f_date)");
    }

    refreshData();
}

void CR5ConsumptionBySales::semireadyInOut()
{
    DlgSemireadyInOut d(mUser);

    if(d.exec()) {
        C5Database db;
        QString uuid = db.uuid();

        for(int i = 0; i < d.rowCount(); i++) {
            if(d.goodsQty(i) < 0.001) {
                continue;
            }

            bool found = false;

            for(int j = 0; j < fModel->rowCount(); j++) {
                if(fModel->data(j, col_goods, Qt::EditRole).toInt() == d.goodsId(i)) {
                    found = true;
                    \
                    double qty = fModel->data(j, col_qtyinv, Qt::EditRole).toDouble();
                    qty += d.goodsQty(i);
                    fModel->setData(j, col_qtyinv, qty);
                    fModel->setData(j, col_qty_sr, fModel->data(j, col_qty_sr, Qt::EditRole).toDouble() + d.goodsQty(i));
                    writeInvQty(db, qty, j, col_qtyinv, d.goodsId(i));
                    countRowQty(j);
                    break;
                }
            }

            if(!found) {
                db[":f_id"] = d.goodsId(i);
                db.exec("select g.f_id, gn.f_name as f_groupname, g.f_name, g.f_scancode, "
                        "gu.f_name as f_unitname "
                        "from c_goods g "
                        "left join c_groups gn on gn.f_id=g.f_group "
                        "left join c_units gu on gu.f_id=g.f_unit "
                        "where g.f_id=:f_id");
                db.nextRow();
                QVector<QJsonValue> row;
                row << db.getInt("f_id")
                    << db.getString("f_groupname")
                    << db.getString("f_name")
                    << db.getString("f_scancode")
                    << 0
                    << 0
                    << 0
                    << 0
                    << 0
                    << 0
                    << d.goodsQty(i)
                    << 0
                    << db.getString("f_unitname")
                    << 0;
                fModel->insertRow(fModel->rowCount());

                for(int k = 0; k < row.count(); k++) {
                    fModel->setData(fModel->rowCount() - 1, k, row.at(k));
                }

                fModel->setData(fModel->rowCount() - 1, col_qty_sr, fModel->data(fModel->rowCount() - 1, col_qty_sr,
                                Qt::EditRole).toDouble() + d.goodsQty(i));
                writeInvQty(db, d.goodsQty(i), fModel->rowCount() - 1, col_qtyinv, d.goodsId(i));
                countRowQty(fModel->rowCount() - 1);
            }

            db[":f_rec"] = uuid;
            db[":f_date"] = fFilter->date2();
            db[":f_store"] = fFilter->store();
            db[":f_dish"] = d.dishId();
            db[":f_dish_qty"] = d.dishQty();
            db[":f_goods"] = d.goodsId(i);
            db[":f_goods_qty"] = d.goodsQty(i);
            db.insert("a_header_sr", false);
        }
    }
}

C5StoreDoc* CR5ConsumptionBySales::writeDocs(int doctype, int reason, const QList<OGoods>& data, const QString &comment)
{
    C5Database db;
    C5StoreDraftWriter dw(db);
    QDate docDate = fFilter->date2();
    int store = 0;
    int storein = 0;
    int storeout = 0;
    int sdtype = 0;

    switch(doctype) {
    case DOC_TYPE_STORE_OUTPUT:
        store = fFilter->store();
        storeout = store;
        sdtype = -1;
        break;

    case DOC_TYPE_STORE_INPUT:
        store = fFilter->store();
        storein = store;
        sdtype = 1;
        break;
    }

    QString documentId;
    QString cashid, cashrowid;
    dw.writeAHeader(documentId, dw.storeDocNum(doctype, store, true, 0), DOC_STATE_DRAFT, doctype, mUser->id(), docDate,
                    QDate::currentDate(), QTime::currentTime(), 0, 0, comment, 0, __c5config.getValue(param_default_currency).toInt());

    if(storein > 0) {
        C5Document c5doc;
        QString purpose = tr("Store input, correction") + QDate::currentDate().toString(FORMAT_DATE_TO_STR);
        QString cashUserId = QString("%1").arg(c5doc.genNumber(DOC_TYPE_CASH), C5Config::docNumDigitsInput(), 10, QChar('0'));
        dw.writeAHeader(cashid, cashUserId, DOC_STATE_DRAFT, DOC_TYPE_CASH, mUser->id(), QDate::currentDate(),
                        QDate::currentDate(), QTime::currentTime(), 0, 0, purpose,
                        0, __c5config.getValue(param_default_currency).toInt());
        dw.writeAHeaderCash(cashid, 0, 0, 1, documentId, "");
        dw.writeECash(cashrowid, cashid, 0, -1, purpose, 0, cashrowid, 1);
    }

    dw.writeAHeaderStore(documentId, mUser->id(), mUser->id(), "", QDate(), storein, storeout, 0, cashid, 0, 0, "");
    int rownum = 1;

    foreach(const OGoods &g, data) {
        QString sdid;
        dw.writeAStoreDraft(sdid, documentId, store, sdtype, g.goods, g.qty, g.price, g.price * g.qty, reason, "", rownum++, "");
    }

    //= dw.writeDraft(docDate, doctype, store, reason, data, comment);
#ifdef NEWVERSION
    auto *sd = __mainWindow->createTab<C5StoreDoc>();
    QString e;

    if(doctype == DOC_TYPE_STORE_INPUT) {
        auto *ss = __mainWindow->createTab<StoreInputDocument>();
        ss->openDoc(documentId);
        ss->setLastInputPrices();
    } else {
        if(!sd->openDoc(documentId, e)) {
            __mainWindow->removeTab(sd);
            sd = nullptr;
            C5Message::error(e);
        }
    }

    return sd;
#else
    auto *sd = __mainWindow->createTab<C5StoreDoc>();
    QString e;

    if(!sd->openDoc(documentId, e)) {
        __mainWindow->removeTab(sd);
        sd = nullptr;
        C5Message::error(e);
    }

    return sd;
#endif
}
