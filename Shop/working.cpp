#include "working.h"
#include "ui_working.h"
#include "worder.h"
#include "c5database.h"
#include "goods.h"
#include "printtaxn.h"
#include "sales.h"
#include "c5replication.h"
#include "c5config.h"
#include "c5userauth.h"
#include "loghistory.h"
#include "dlgpin.h"
#include "searchitems.h"
#include "c5connection.h"
#include "c5logsystem.h"
#include "storeinput.h"
#include "selectstaff.h"
#include <QShortcut>
#include <QInputDialog>
#include <QKeyEvent>
#include <QTimer>
#include <QSettings>

QMap<QString, Goods> Working::fGoods;
QHash<int, QString> Working::fGoodsCodeForPrint;
QHash<QString, int> Working::fGoodsRows;
static QSettings __s(QString("%1\\%2\\%3").arg(_ORGANIZATION_).arg(_APPLICATION_).arg(_MODULE_));
QHash<int, UncomplectGoods> Working::fUncomplectGoods;

Working::Working(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Working)
{
    ui->setupUi(this);
    QShortcut *sF1 = new QShortcut(QKeySequence(Qt::Key_F1), this);
    QShortcut *sF2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    QShortcut *sF3 = new QShortcut(QKeySequence(Qt::Key_F3), this);
    QShortcut *sF4 = new QShortcut(QKeySequence(Qt::Key_F4), this);
    QShortcut *sF6 = new QShortcut(QKeySequence(Qt::Key_F6), this);
    QShortcut *sF7 = new QShortcut(QKeySequence(Qt::Key_F7), this);
    QShortcut *sF8 = new QShortcut(QKeySequence(Qt::Key_F8), this);
    QShortcut *sF9 = new QShortcut(QKeySequence(Qt::Key_F9), this);
    QShortcut *sF10 = new QShortcut(QKeySequence(Qt::Key_F10), this);
    QShortcut *sF11 = new QShortcut(QKeySequence(Qt::Key_F11), this);
    QShortcut *sF12 = new QShortcut(QKeySequence(Qt::Key_F12), this);
    QShortcut *sDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
    QShortcut *sUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
    QShortcut *sEsc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(sF1, SIGNAL(activated()), this, SLOT(shortcutF1()));
    connect(sF2, SIGNAL(activated()), this, SLOT(shortcutF2()));
    connect(sF3, SIGNAL(activated()), this, SLOT(shortcutF3()));
    connect(sF4, SIGNAL(activated()), this, SLOT(shortcutF4()));
    connect(sF6, SIGNAL(activated()), this, SLOT(shortcurF6()));
    connect(sF7, SIGNAL(activated()), this, SLOT(shortcutF7()));
    connect(sF8, SIGNAL(activated()), this, SLOT(shortcutF8()));
    connect(sF9, SIGNAL(activated()), this, SLOT(shortcutF9()));
    connect(sF10, SIGNAL(activated()), this, SLOT(shortcutF10()));
    connect(sF11, SIGNAL(activated()), this, SLOT(shortcutF11()));
    connect(sF12, SIGNAL(activated()), this, SLOT(shortcutF12()));
    connect(sDown, SIGNAL(activated()), this, SLOT(shortcutDown()));
    connect(sUp, SIGNAL(activated()), this, SLOT(shortcutUp()));
    connect(sEsc, SIGNAL(activated()), this, SLOT(escape()));
    makeWGoods();
    ui->leCode->installEventFilter(this);
    ui->tab->installEventFilter(this);
    on_btnNewOrder_clicked();
    ui->wGoods->setVisible(false);
    ui->btnNewOrder->setVisible(!__c5config.shopDenyF1());
    ui->btnNewWhosale->setVisible(!__c5config.shopDenyF2());
    fTimerCounter = 0;
    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(1000);
    if (__c5config.shopDifferentStaff()) {
        loadStaff();
    }
    fHaveChanges = false;
    fUpFinished = true;
    fTab = ui->tab;
}

Working::~Working()
{
    delete ui;
}

bool Working::eventFilter(QObject *watched, QEvent *event)
{
    WOrder *w = nullptr;
    if (event->type() == QEvent::KeyRelease) {
        auto *ke = static_cast<QKeyEvent*>(event);
        switch (ke->key()) {
        case Qt::Key_Plus:
            ui->leCode->clear();
            w = static_cast<WOrder*>(ui->tab->currentWidget());
            if (w) {
                w->changeQty();
            }
            event->accept();
            return true;
        case Qt::Key_Minus:
            ui->leCode->clear();
            w = static_cast<WOrder*>(ui->tab->currentWidget());
            if (w) {
                w->removeRow();
            }
            event->accept();
            return true;
        case Qt::Key_Asterisk:
            if (__c5config.shopDenyPriceChange()) {
                return QWidget::eventFilter(watched, event);
            }
            ui->leCode->clear();
            w = static_cast<WOrder*>(ui->tab->currentWidget());
            if (w) {
                w->changePrice();
            }
            event->accept();
            return true;
        case Qt::Key_F5:
            on_btnShowGoodsList_clicked();
            break;
        case Qt::Key_F9:
            on_btnSaveOrderNoTax_clicked();
            break;
        case Qt::Key_S:
            if (ke->modifiers() & Qt::ControlModifier) {
                SearchItems *si = new SearchItems(this);
                si->exec();
                si->deleteLater();
                event->accept();
                return true;
            }
            break;
        case Qt::Key_I:
            if (ke->modifiers() & Qt::ControlModifier) {
                QString pin, pass;
                if (DlgPin::getPin(pin, pass)) {
                    C5Database db(__c5config.dbParams());
                    C5UserAuth ua(db);
                    int user, group;
                    QString name;
                    if (ua.authByPinPass(pin, pass, user, group, name)) {
                        db[":f_user"] = user;
                        db.exec("select * from s_salary_inout where f_user=:f_user and f_dateout is null");
                        if (db.nextRow()) {
                            C5Message::error(tr("Cannot input without output"));
                        } else {
                            db[":f_user"] = user;
                            db[":f_datein"] = QDate::currentDate();
                            db[":f_timein"] = QTime::currentTime();
                            db.insert("s_salary_inout", false);
                            loadStaff();
                            C5Message::info(QString("%1,<br>%2").arg(tr("Welcome")).arg(name));
                        }
                    } else {
                        C5Message::error(ua.error());
                    }
                }
            }
            break;
        case Qt::Key_O:
            if (ke->modifiers() & Qt::ControlModifier) {
                QString pin, pass;
                if (DlgPin::getPin(pin, pass)) {
                    C5Database db(__c5config.dbParams());
                    C5UserAuth ua(db);
                    int user, group;
                    QString name;
                    if (ua.authByPinPass(pin, pass, user, group, name)) {
                        db[":f_user"] = user;
                        db.exec("select * from s_salary_inout where f_user=:f_user and f_dateout is null");
                        if (db.nextRow()) {
                            db[":f_user"] = user;
                            db[":f_dateout"] = QDate::currentDate();
                            db[":f_timeout"] = QTime::currentTime();
                            db.update("s_salary_inout", where_id(db.getInt("f_id")));
                            loadStaff();
                            C5Message::info(QString("%1,<br>%2").arg(tr("Good bye")).arg(name));
                        } else {
                            C5Message::error(tr("Cannot output without input"));
                        }
                    } else {
                        C5Message::error(ua.error());
                    }
                }
            }
            break;
        case Qt::Key_T:
            if (ke->modifiers() & Qt::ControlModifier) {
                C5Database db(__c5config.replicaDbParams());
                db[":f_hall"] = __c5config.defaultHall();
                db[":f_datecash"] = QDate::currentDate();
                db[":f_state"] = ORDER_STATE_CLOSE;
                db.exec("select concat(w.f_last, ' ', w.f_first) as f_staff, if(f_amounttotal<0,-1,1) as f_sign,"
                        "sum(oh.f_amounttotal) as f_amounttotal "
                        "from o_header oh "
                        "left join s_user w on w.f_id=oh.f_staff  "
                        "where oh.f_hall=:f_hall and oh.f_datecash=:f_datecash  and oh.f_state=:f_state and oh.f_amounttotal>0 "
                        "group by 1, 2 "
                        "union "
                        "select '-' as f_staff, if(f_amounttotal<0,-1,1) as f_sign,"
                        "sum(oh.f_amounttotal) as f_amounttotal "
                        "from o_header oh "
                        "left join s_user w on w.f_id=oh.f_staff  "
                        "where oh.f_hall=:f_hall and oh.f_datecash=:f_datecash  and oh.f_state=:f_state and oh.f_amounttotal<0 "
                        "group by 1, 2 ");
                QString info = tr("Total today") + "<br>";
                while (db.nextRow()) {
                    info += QString("%1: %2<br>").arg(db.getString("f_staff")).arg(float_str(db.getDouble("f_amounttotal"), 2));
                }
                C5Message::info(info);
                event->accept();
            }
            break;
        case Qt::Key_A:
            if (ke->modifiers() & Qt::ControlModifier) {
                newSale(SALE_PREORDER);
                event->accept();
            }
            break;
        case Qt::Key_H:
            if (ke->modifiers() & Qt::ControlModifier) {
                auto *l = new LogHistory(this);
                l->exec();
                delete l;
            }
            break;
        }
    }
    return QWidget::eventFilter(watched, event);
}

QString Working::goodsCode(int code) const
{
    if (fGoodsCodeForPrint.contains(code)) {
        return fGoodsCodeForPrint[code] + " ";
    } else {
        return "";
    }
}

bool Working::getAdministratorRights()
{
    bool ok;
    QString pwd = QInputDialog::getText(this, tr("Administrator password"), tr("Password"), QLineEdit::Password, "", &ok);
    if (!ok) {
        return false;
    }
    C5Database db(C5Config::dbParams());
    db[":f_altPassword"] = password(pwd);
    db[":f_state"] = 1;
    db.exec("select f_id, f_group, f_first, f_last from s_user where f_altPassword=:f_altPassword and f_state=:f_state");
    if (db.nextRow()) {
        if (db.getInt(1) != 1) {
            db[":f_group"] = db.getValue(1);
            db[":f_key"] = cp_t5_refund_goods;
            db.exec("select f_key from s_user_access where f_group=:f_group and f_key=:f_key and f_value=1");
            if (db.nextRow()) {
                return true;
            } else {
                C5Message::error(tr("Access denied"));
                return false;
            }
        }
    } else {
        C5Message::error(tr("Access denied"));
        return false;
    }
    return true;
}

void Working::decQty(const IGoods &g)
{
    QString code = fGoodsCodeForPrint[g.goodsId];
    int row = fGoodsRows[code];
    ui->tblGoods->setDouble(row, 14, ui->tblGoods->getDouble(row, 14) - g.goodsQty);
    fGoods[code].fQty -= g.goodsQty;
}

void Working::makeWGoods()
{
    QList<int> cw;
    //cw << 0 << 100 << 200 << 200 << 300 << 80 << 80 << 80 << 100 << 100 << 100 << 100 << 0 << 0 << 80;
    cw << 0 << 150 << 0 << 200 << 400 << 80 << 80 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 80 << 0;
    for (int i = 0; i < cw.count(); i++) {
        ui->tblGoods->setColumnWidth(i, cw.at(i));
    }
    ui->tblGoods->setColumnHidden(5, __c5config.shopDenyF1());
    ui->tblGoods->setColumnHidden(6, __c5config.shopDenyF2());
    ui->lbTotalRetail->setVisible(!__c5config.shopDenyF1());
    ui->leTotalRetail->setVisible(!__c5config.shopDenyF1());
    ui->lbTotalWhosale->setVisible(!__c5config.shopDenyF2());
    ui->leTotalWhosale->setVisible(!__c5config.shopDenyF2());
    ui->tblGoods->verticalHeader()->setDefaultSectionSize(30);
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    int row = 0;
    fGoods.clear();
    fGoodsCodeForPrint.clear();
    C5Database db(C5Config::dbParams());
    if (C5Config::controlShopQty()) {
        if (__c5config.rdbReplica()) {
            db[":f_store"] = C5Config::defaultStore();
            db.exec("select g.f_id, g.f_scancode, cp.f_taxname, gg.f_name as f_groupname, g.f_name as f_goodsname, "
                    "g.f_saleprice, g.f_saleprice2, u.f_name as f_unitname, "
                    "gca.f_name as group1, gcb.f_name group2, gcc.f_name as group3, gcd.f_name as group4, "
                    "gg.f_taxdept, gg.f_adgcode, s.f_qty, g.f_unit, go.f_flaguncomplectfrom, go.f_uncomplectfromqty  "
                    "from a_store_temp s "
                    "inner join c_goods g on g.f_id=s.f_goods "
                    "inner join c_groups gg on gg.f_id=g.f_group "
                    "inner join c_units u on u.f_id=g.f_unit "
                    "left join c_goods_classes gca on gca.f_id=g.f_group1 "
                    "left join c_goods_classes gcb on gcb.f_id=g.f_group2 "
                    "left join c_goods_classes gcc on gcc.f_id=g.f_group3 "
                    "left join c_goods_classes gcd on gcd.f_id=g.f_group4 "
                    "left join c_goods_option go on go.f_id=g.f_id and go.f_flaguncomplectfrom>0 "
                    "left join c_partners cp on cp.f_id=g.f_supplier "
                    "where s.f_store=:f_store and s.f_qty>0 ");
        } else {
            db[":f_store"] = C5Config::defaultStore();
            db[":f_date"] = QDate::currentDate();
            db[":f_state"] = DOC_STATE_SAVED;
            db.exec("select g.f_id, g.f_scancode, cp.f_taxname, gg.f_name as f_groupname, g.f_name as f_goodsname, "
                    "g.f_saleprice, g.f_saleprice2, u.f_name as f_unitname, "
                    "gca.f_name as group1, gcb.f_name group2, gcc.f_name as group3, gcd.f_name as group4, "
                    "gg.f_taxdept, gg.f_adgcode, sum(s.f_qty*s.f_type) as f_qty, g.f_unit, "
                    "go.f_flaguncomplectfrom, go.f_uncomplectfromqty "
                    "from a_store_draft s "
                    "inner join c_goods g on g.f_id=s.f_goods "
                    "inner join c_groups gg on gg.f_id=g.f_group "
                    "inner join c_units u on u.f_id=g.f_unit "
                    "left join c_goods_classes gca on gca.f_id=g.f_group1 "
                    "left join c_goods_classes gcb on gcb.f_id=g.f_group2 "
                    "left join c_goods_classes gcc on gcc.f_id=g.f_group3 "
                    "left join c_goods_classes gcd on gcd.f_id=g.f_group4 "
                    "left join c_goods_option go on go.f_id=g.f_id and go.f_flaguncomplectfrom>0 "
                    "left join c_partners cp on cp.f_id=g.f_supplier "
                    "inner join a_header h on h.f_id=s.f_document "
                    "where h.f_date<=:f_date and s.f_store=:f_store and h.f_state=:f_state and g.f_enabled=1 "
                    "group by g.f_id,gg.f_name,g.f_name,g.f_lastinputprice,g.f_saleprice "
                    "having sum(s.f_qty*s.f_type) > 0 ");
        }
    } else {
        db.exec("select gs.f_id, gs.f_scancode, cp.f_taxname, gr.f_name as f_groupname, gs.f_name as f_goodsname,  "
                "gs.f_saleprice, gs.f_saleprice2, gu.f_name as f_unitname, "
                "gca.f_name as group1, gcb.f_name group2, gcc.f_name as group3, gcd.f_name as group4, "
                "gr.f_taxdept, gr.f_adgcode, 0 as f_qty, gs.f_unit, go.f_flaguncomplectfrom, go.f_uncomplectfromqty "
                "from c_goods gs "
                "left join c_groups gr on gr.f_id=gs.f_group "
                "left join c_units gu on gu.f_id=gs.f_unit "
                "left join c_partners cp on cp.f_id=gs.f_supplier "
                "left join c_goods_classes gca on gca.f_id=gs.f_group1 "
                "left join c_goods_classes gcb on gcb.f_id=gs.f_group2 "
                "left join c_goods_classes gcc on gcc.f_id=gs.f_group3 "
                "left join c_goods_classes gcd on gcd.f_id=gs.f_group4 "
                "left join c_goods_option go on go.f_id=gs.f_id and go.f_flaguncomplectfrom>0 "
                "where gs.f_enabled=1 and gs.f_service=0 "
                "order by gr.f_name, gca.f_name ");
    }
    ui->tblGoods->setRowCount(db.rowCount());
    ui->leTotalRetail->setDouble(0);
    ui->leTotalRetail->setDouble(0);
    while (db.nextRow()) {
        Goods g;
        g.fScanCode = db.getString("f_scancode");
        g.fCode = db.getString("f_id");
        g.fName = db.getString("f_goodsname");
        g.fUnit = db.getString("f_unitname");
        g.fUnitCode = db.getInt("f_unit");
        g.fRetailPrice = db.getDouble("f_saleprice");
        g.fWhosalePrice = db.getDouble("f_saleprice2");
        g.fTaxDept = db.getInt("f_taxdept");
        g.fAdgCode = db.getString("f_adgcode");
        g.fQty = db.getDouble("f_qty");
        g.fIsService = false;
        g.fUncomplectFrom = db.getInt("f_flaguncomplectfrom");
        g.fUncomplectQty = db.getDouble("f_uncomplectfromqty");
        fGoods[g.fScanCode] = g;
        fGoodsCodeForPrint[g.fCode.toInt()] = g.fScanCode;
        fGoodsRows[g.fScanCode] = row;
        for (int i = 0; i < db.columnCount(); i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::EditRole, db.getValue(i));
            ui->tblGoods->setItem(row, i, item);
        }
        ui->leTotalRetail->setDouble(ui->leTotalRetail->getDouble() + (g.fRetailPrice * g.fQty));
        ui->leTotalWhosale->setDouble(ui->leTotalWhosale->getDouble() + (g.fWhosalePrice * g.fQty));
        row++;
    }
    //services
    db.exec("select gs.f_id, gs.f_scancode, cp.f_taxname, gr.f_name as f_groupname, gs.f_name as f_goodsname,  "
            "gs.f_saleprice, gs.f_saleprice2, gu.f_name as f_unitname, "
            "gca.f_name as group1, gcb.f_name group2, gcc.f_name as group3, gcd.f_name as group4, "
            "gr.f_taxdept, gr.f_adgcode, 0 as f_qty, gs.f_unit, go.f_flaguncomplectfrom, go.f_uncomplectfromqty, "
            "gs.f_service "
            "from c_goods gs "
            "left join c_groups gr on gr.f_id=gs.f_group "
            "left join c_units gu on gu.f_id=gs.f_unit "
            "left join c_partners cp on cp.f_id=gs.f_supplier "
            "left join c_goods_classes gca on gca.f_id=gs.f_group1 "
            "left join c_goods_classes gcb on gcb.f_id=gs.f_group2 "
            "left join c_goods_classes gcc on gcc.f_id=gs.f_group3 "
            "left join c_goods_classes gcd on gcd.f_id=gs.f_group4 "
            "left join c_goods_option go on go.f_id=gs.f_id and go.f_flaguncomplectfrom>0 "
            "where gs.f_enabled=1 and gs.f_service=1 or (gs.f_id in (select f_id from c_goods_option where f_flaguncomplectfrom>0)) "
            "order by gr.f_name, gca.f_name ");
    //ui->tblGoods->setRowCount(ui->tblGoods->rowCount() + db.rowCount());
    while (db.nextRow()) {
        if (db.getInt("f_flaguncomplectfrom") > 0) {
            if (fGoods.contains(db.getString("f_scancode"))) {
                continue;
            }
        }
        ui->tblGoods->setRowCount(ui->tblGoods->rowCount() + 1);
        Goods g;
        g.fScanCode = db.getString("f_scancode");
        g.fCode = db.getString("f_id");
        g.fName = db.getString("f_goodsname");
        g.fUnit = db.getString("f_unitname");
        g.fUnitCode = db.getInt("f_unit");
        g.fRetailPrice = db.getDouble("f_saleprice");
        g.fWhosalePrice = db.getDouble("f_saleprice2");
        g.fTaxDept = db.getInt("f_taxdept");
        g.fAdgCode = db.getString("f_adgcode");
        g.fQty = db.getDouble("f_qty");
        g.fIsService = db.getInt("f_service") > 0;
        g.fUncomplectFrom = db.getInt("f_flaguncomplectfrom");
        g.fUncomplectQty = db.getDouble("f_uncomplectfromqty");
        fGoods[g.fScanCode] = g;
        fGoodsCodeForPrint[g.fCode.toInt()] = g.fScanCode;
        for (int i = 0; i < db.columnCount(); i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::EditRole, db.getValue(i));
            ui->tblGoods->setItem(row, i, item);
        }
        ui->leTotalRetail->setDouble(ui->leTotalRetail->getDouble() + (g.fRetailPrice * g.fQty));
        ui->leTotalWhosale->setDouble(ui->leTotalWhosale->getDouble() + (g.fWhosalePrice * g.fQty));
        row++;
    }

    fUncomplectGoods.clear();
    db.exec("select f_id, f_flaguncomplectfrom, f_uncomplectfromqty from c_goods_option where f_flaguncomplectfrom>0 ");
    while (db.nextRow()) {
        UncomplectGoods g;
        g.uncomplectGoods = db.getInt(1);
        g.qty = db.getDouble(2);
        fUncomplectGoods.insert(db.getInt(0), g);
    }

    ui->lbLastUpdate->setText(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
}

void Working::loadStaff()
{
    fCurrentUsers.clear();
    C5Database db(__c5config.dbParams());
    db.exec("select u.f_id, u.f_group, concat(u.f_last, ' ' , u.f_first) as f_name, p.f_data "
            "from s_salary_inout s "
            "inner join s_user u on u.f_id=s.f_user "
            "left join s_user_photo p on p.f_id=u.f_id "
            "where s.f_dateout is null");
    while (db.nextRow()) {
        IUser u;
        u.id = db.getInt("f_id");
        u.group = db.getInt("f_group");
        u.name = db.getString("f_name");
        QPixmap p;
        if (!p.loadFromData(db.getValue("f_data").toByteArray())) {
            p = QPixmap(":/staff.png");
        }
        u.photo = p;
        fCurrentUsers.append(u);
    }
}

void Working::addGoods(QString &code)
{
    if (code.isEmpty()) {
        return;
    }
    ui->leCode->clear();
    ui->leCode->setFocus();
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    if (code.at(0).toLower() == "?" ) {
        if (code.at(1).toLower() == "c") {
            code.remove(0, 2);
            w->fixCostumer(code);
            return;
        }
    }
    if (code.at(0).toLower() == "/") {
        code.remove(0, 1);
        w->discountRow(code);
        return;
    }
    Goods g;
    g.fCode = "0";
    if (fGoods.contains(code)) {
        g = fGoods[code];
    } else {
        if (w->fSaleType == SALE_PREORDER) {
            C5Database db(__c5config.dbParams());
            db[":f_scancode"] = code;
            db.exec("select gs.f_id, gs.f_scancode, cp.f_taxname, gr.f_name as f_groupname, gs.f_name as f_goodsname,  "
                      "gs.f_saleprice, gs.f_saleprice2, gu.f_name as f_unitname, "
                      "gca.f_name as group1, gcb.f_name group2, gcc.f_name as group3, gcd.f_name as group4, "
                      "gr.f_taxdept, gr.f_adgcode, 0 as f_qty, gs.f_unit, gs.f_service "
                      "from c_goods gs "
                      "left join c_groups gr on gr.f_id=gs.f_group "
                      "left join c_units gu on gu.f_id=gs.f_unit "
                      "left join c_partners cp on cp.f_id=gs.f_supplier "
                      "left join c_goods_classes gca on gca.f_id=gs.f_group1 "
                      "left join c_goods_classes gcb on gcb.f_id=gs.f_group2 "
                      "left join c_goods_classes gcc on gcc.f_id=gs.f_group3 "
                      "left join c_goods_classes gcd on gcd.f_id=gs.f_group4 "
                      "where gs.f_enabled=1 and gs.f_scancode=:f_scancode "
                      "order by gr.f_name, gca.f_name ");
            if (db.nextRow()) {
                g.fScanCode = db.getString("f_scancode");
                g.fCode = db.getString("f_id");
                g.fName = db.getString("f_goodsname");
                g.fUnit = db.getString("f_unitname");
                g.fUnitCode = db.getInt("f_unit");
                g.fRetailPrice = db.getDouble("f_saleprice");
                g.fWhosalePrice = db.getDouble("f_saleprice2");
                g.fTaxDept = db.getInt("f_taxdept");
                g.fAdgCode = db.getString("f_adgcode");
                g.fQty = db.getDouble("f_qty");
                g.fIsService = db.getInt("f_service") == 1;
            }
        }
    }
    if (g.fCode.toInt() == 0) {
        ls(tr("Invalid code entered: ") + code);
        return;
    }
    switch (w->fSaleType) {
    case SALE_RETAIL:
    case SALE_WHOSALE:
        w->addGoods(g);
        break;
    case SALE_PREORDER:
        w->addGoodsToTable(g);
        break;
    }
}

void Working::newSale(int type)
{
    WOrder *w = new WOrder(type, this);
    QObjectList ol = w->children();
    for (QObject *o: ol) {
        auto wd = dynamic_cast<QWidget*>(o);
        if (wd) {
            wd->installEventFilter(this);
        }
    }
    QString title;
    switch (type) {
    case SALE_RETAIL:
        title = tr("Retail");
        break;
    case SALE_WHOSALE:
        title = tr("Whosale");
        break;
    case SALE_PREORDER:
        title = tr("Preorder");
        break;
    }
    ui->tab->addTab(w, QString("%1 #%2").arg(title).arg(ordersCount()));
    ui->tab->setCurrentIndex(ui->tab->count() - 1);
    C5LogSystem::writeEvent(QString("%1 %2, #%3").arg(tr("New")).arg(title).arg(ui->tab->count()));
}

int Working::ordersCount()
{
    C5Database db(C5Config::dbParams());
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_datecash"] = QDate::currentDate();
    db[":f_hall"] = C5Config::defaultHall();
    db.exec("select count(f_id) + 1 from o_header where f_datecash=:f_datecash and f_state=:f_state and f_hall=:f_hall");
    if (db.nextRow()) {
        return db.getInt(0) + ui->tab->count();
    } else {
        return ui->tab->count();
    }
}

void Working::timeout()
{
    fTimerCounter++;
    if (__c5config.rdbReplica()) {
        if (fTimerCounter % 20 == 0 && fUpFinished && __c5config.rdbReplica()) {
//            fUpFinished = false;
//            auto *r = new C5Replication();
//            connect(r, SIGNAL(finished()), this, SLOT(uploadDataFinished()));
//            r->start(SLOT(uploadToServer()));
        }
//        if (fTimerCounter % 120 == 0) {
//            auto *r = new C5Replication();
//            connect(r, SIGNAL(haveChanges(bool)), this, SLOT(haveChanges(bool)));
//            r->start(SLOT(downloadFromServer()));
//        }

    }
//    if (fTimerCounter % 180 == 0) {
//        getGoodsList();
//    }
    if (fHaveChanges) {
        QString style = "background: green;";
        switch (fTimerCounter % 4) {
        case 1:
            style = "background: red;";
            break;
        case 2:
            style = "background: blue";
            break;
        case 3:
            style = "background: white;";
            break;
        }
        WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
        if (!w) {
            return;
        }
        w->changeIshmarColor(style);
    }
}

void Working::uploadDataFinished()
{
    fUpFinished = true;
}

void Working::escape()
{
    if (ui->tblGoods->isVisible()) {
        ui->wGoods->setVisible(!ui->wGoods->isVisible());
        ui->tab->setVisible(!ui->wGoods->isVisible());
    }
    ui->leCode->clear();
}

void Working::shortcutF1()
{
    if (__c5config.shopDenyF1()) {
        return;
    }
    newSale(SALE_RETAIL);
}

void Working::shortcutF2()
{
    if (__c5config.shopDenyF2()) {
        return;
    }
    newSale(SALE_WHOSALE);
}

void Working::shortcutF3()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    w->focusCash();
}

void Working::shortcutF4()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    w->focusCard();
}

void Working::shortcutF5()
{
    on_btnShowGoodsList_clicked();
}

void Working::shortcurF6()
{
    ui->lePartner->setFocus();
}

void Working::shortcutF7()
{
    ui->leCode->setFocus();
}

void Working::shortcutF8()
{

}

void Working::shortcutF9()
{
    on_btnSaveOrderNoTax_clicked();
}

void Working::shortcutF10()
{
    on_btnItemBack_clicked();
}

void Working::shortcutF11()
{
    on_btnStoreInput_clicked();
}

void Working::shortcutF12()
{
    on_btnSaveOrder_clicked();
}

void Working::shortcutDown()
{
    if (ui->tblGoods->isVisible()) {
        bool stop = false;
        do {
            if (ui->tblGoods->currentRow() == ui->tblGoods->rowCount() - 1) {
                break;
            }
            ui->tblGoods->setCurrentCell(ui->tblGoods->currentRow() + 1, 0);
            if (!ui->tblGoods->isRowHidden(ui->tblGoods->currentRow())) {
                stop = true;
            }
        } while (!stop);
    } else {
        WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
        if (!w) {
            return;
        }
        w->nextRow();
    }
}

void Working::shortcutUp()
{
    if (ui->tblGoods->isVisible()) {
        bool stop = false;
        do {
            if (ui->tblGoods->currentRow() == 0) {
                break;
            }
            ui->tblGoods->setCurrentCell(ui->tblGoods->currentRow() - 1, 0);
            if (!ui->tblGoods->isRowHidden(ui->tblGoods->currentRow())) {
                stop = true;
            }
        } while (!stop);
    } else {
        WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
        if (!w) {
            return;
        }
        w->prevRow();
    }
}

void Working::haveChanges(bool v)
{
    if (!fHaveChanges) {
        fHaveChanges = v;
    }
}

void Working::on_btnNewOrder_clicked()
{
    newSale(SALE_RETAIL);
}

void Working::on_btnConnection_clicked()
{
    if (C5Config::fDBPassword.length() > 0) {
        QString password = QInputDialog::getText(this, tr("Password"), tr("Password"), QLineEdit::Password);
        if (C5Config::fDBPassword != password) {
            C5Message::error(tr("Access denied"));
            return;
        }
    }
    const QStringList dbParams;
    C5Connection *cnf = new C5Connection(dbParams);
    cnf->exec();
    delete cnf;
}

void Working::on_leCode_returnPressed()
{
    if (ui->tblGoods->isVisible()) {
        int row = ui->tblGoods->currentRow();
        if (row > -1) {
            QString code = ui->tblGoods->item(row, 1)->data(Qt::EditRole).toString();
            if (!code.isEmpty()) {
                addGoods(code);
            }
        }
    } else {
        QString code = ui->leCode->text();
        if (code.mid(0, 2) == "23") {
            if (code.length() != 13) {
                addGoods(code);
                return;
            }
            QString code2 = QString("%1").arg(code.mid(2, 5).toInt());
            QString qtyStr = code.mid(7,5);
            addGoods(code2);
            WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
            if (!w) {
                return;
            }
            int row = w->lastRow();
            if (row < 0) {
                return;
            }
            w->setQtyOfRow(row, qtyStr.toDouble() / 1000);
        } else {
            addGoods(code);
        }
    }
}

void Working::on_btnSaveOrder_clicked()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    C5LogSystem::writeEvent(QString("F12"));
    if (!w->writeOrder()) {
        return;
    }
    ui->tab->removeTab(ui->tab->currentIndex());
    if (ui->tab->count() == 0) {
        on_btnNewOrder_clicked();
    }
    w->deleteLater();
    ui->leCode->setFocus();
    ui->lePartner->clear();
}

void Working::on_btnSaveOrderNoTax_clicked()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    C5LogSystem::writeEvent(QString("F9"));
    if (!w->writeOrder(false)) {
        return;
    }
    ui->tab->removeTab(ui->tab->currentIndex());
    if (ui->tab->count() == 0) {
        on_btnNewOrder_clicked();
    }
    w->deleteLater();
    ui->leCode->setFocus();
    ui->lePartner->clear();
}

void Working::on_btnExit_clicked()
{
    qApp->quit();
}

void Working::on_btnShowGoodsList_clicked()
{
    ui->wGoods->setVisible(!ui->wGoods->isVisible());
    ui->tab->setVisible(!ui->wGoods->isVisible());
    __s.setValue("goodslist", false);
}

void Working::on_tblGoods_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    QString code = item->data(Qt::UserRole).toString();
    if (code.isEmpty()) {
        return;
    }
    ui->leCode->setText(code);
    on_leCode_returnPressed();
}

void Working::on_btnDuplicateReceipt_clicked()
{
    if (!getAdministratorRights()) {
        return;
    }
    Sales::showSales(this);
}

void Working::on_leCode_textChanged(const QString &arg1)
{
    if (!ui->tblGoods->isVisible()) {
        return;
    }
    if (arg1 == "+") {
        return;
    }
    if (arg1 == "-") {
        return;
    }
    if (arg1 == "*") {
        return;
    }
    bool selectRow = false;
    for (int r = 0; r < ui->tblGoods->rowCount(); r++) {
        for (int c = 1; c < ui->tblGoods->columnCount(); c++) {
            QTableWidgetItem *item = ui->tblGoods->item(r, c);
            if (item->data(Qt::EditRole).toString().contains(arg1, Qt::CaseInsensitive)) {
                ui->tblGoods->setRowHidden(r, false);
                if (!selectRow) {
                    selectRow = true;
                    ui->tblGoods->setCurrentCell(r, 0);
                }
                goto C;
            }
        }
        ui->tblGoods->setRowHidden(r, true);
        C:
        continue;
    }
}

void Working::on_btnNewWhosale_clicked()
{
    newSale(SALE_WHOSALE);
}

void Working::on_lePartner_returnPressed()
{
    if (ui->lePartner->text().isEmpty()) {
        return;
    }
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    C5Database db(__c5config.dbParams());
    db[":f_taxcode"] = ui->lePartner->text();
    db.exec("select f_id, f_taxname from c_partners where f_taxcode=:f_taxcode");
    if (db.nextRow()) {
        w->setPartner(ui->lePartner->text(), db.getInt("f_id"), db.getString(1));
    } else {
        db[":f_taxcode"] = ui->lePartner->text();
        int pid = db.insert("c_partners");
        w->setPartner(ui->lePartner->text(), pid, "");
    }
    ui->lePartner->clear();
}

void Working::on_tab_tabCloseRequested(int index)
{
    QWidget *w = ui->tab->widget(index);
    ui->tab->removeTab(index);
    w->deleteLater();
    C5LogSystem::writeEvent(QString("%1 #%2").arg(tr("Window closed")).arg(index + 1));
    if (ui->tab->count() == 0) {
        newSale(SALE_RETAIL);
    }
}

void Working::on_btnItemBack_clicked()
{
    if (!getAdministratorRights()) {
        return;
    }
    Sales::showSales(this);
}

void Working::on_btnStoreInput_clicked()
{
    if (!getAdministratorRights()) {
        return;
    }
    StoreInput *si = new StoreInput(this);
    si->showMaximized();
    si->exec();
    si->deleteLater();
}

void Working::on_tab_currentChanged(int index)
{
    C5LogSystem::writeEvent(QString("%1 #%2").arg(tr("Current window")).arg(index + 1));
}
