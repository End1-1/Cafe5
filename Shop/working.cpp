#include "working.h"
#include "ui_working.h"
#include "worder.h"
#include "c5database.h"
#include "goods.h"
#include "printtaxn.h"
#include "sales.h"
#include "c5replication.h"
#include "c5config.h"
#include "c5connection.h"
#include "storeinput.h"
#include <QShortcut>
#include <QInputDialog>
#include <QTimer>
#include <QSettings>

QMap<QString, Goods> Working::fGoods;
QHash<int, QString> Working::fGoodsCodeForPrint;
static QSettings __s(QString("%1\\%2\\%3").arg(_ORGANIZATION_).arg(_APPLICATION_).arg(_MODULE_));

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
    getGoodsList();
    ui->leCode->installEventFilter(this);
    ui->tab->installEventFilter(this);
    on_btnNewOrder_clicked();
    PrintTaxN::fTaxCashier = __c5config.taxCashier();
    PrintTaxN::fTaxPin = __c5config.taxPin();
    ui->wGoods->setVisible(false);
    QTimer *timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->start(20000);
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

void Working::getGoodsList()
{
    makeWGoods();
}

void Working::makeWGoods()
{
    QList<int> cw;
    //cw << 0 << 100 << 200 << 200 << 300 << 80 << 80 << 80 << 100 << 100 << 100 << 100 << 0 << 0 << 80;
    cw << 0 << 150 << 0 << 200 << 400 << 80 << 80 << 0 << 0 << 0 << 0 << 0 << 0 << 0 << 80;
    for (int i = 0; i < cw.count(); i++) {
        ui->tblGoods->setColumnWidth(i, cw.at(i));
    }
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
                    "gg.f_taxdept, gg.f_adgcode, s.f_qty  "
                    "from a_store_temp s "
                    "inner join c_goods g on g.f_id=s.f_goods "
                    "inner join c_groups gg on gg.f_id=g.f_group "
                    "inner join c_units u on u.f_id=g.f_unit "
                    "left join c_goods_classes gca on gca.f_id=g.f_group1 "
                    "left join c_goods_classes gcb on gcb.f_id=g.f_group2 "
                    "left join c_goods_classes gcc on gcc.f_id=g.f_group3 "
                    "left join c_goods_classes gcd on gcd.f_id=g.f_group4 "
                    "left join c_partners cp on cp.f_id=g.f_supplier "
                    "where s.f_store=:f_store  ");
        } else {
            db[":f_store"] = C5Config::defaultStore();
            db[":f_date"] = QDate::currentDate();
            db.exec("select g.f_id, g.f_scancode, cp.f_taxname, gg.f_name as f_groupname, g.f_name as f_goodsname, "
                    "g.f_saleprice, g.f_saleprice2, u.f_name as f_unitname, "
                    "gca.f_name as group1, gcb.f_name group2, gcc.f_name as group3, gcd.f_name as group4, "
                    "gg.f_taxdept, gg.f_adgcode, sum(s.f_qty*s.f_type) as f_qty "
                    "from a_store_draft s "
                    "inner join c_goods g on g.f_id=s.f_goods "
                    "inner join c_groups gg on gg.f_id=g.f_group "
                    "inner join c_units u on u.f_id=g.f_unit "
                    "left join c_goods_classes gca on gca.f_id=g.f_group1 "
                    "left join c_goods_classes gcb on gcb.f_id=g.f_group2 "
                    "left join c_goods_classes gcc on gcc.f_id=g.f_group3 "
                    "left join c_goods_classes gcd on gcd.f_id=g.f_group4 "
                    "left join c_partners cp on cp.f_id=g.f_supplier "
                    "inner join a_header h on h.f_id=s.f_document "
                    "where h.f_date<=:f_date and s.f_store=:f_store  "
                    "group by g.f_id,gg.f_name,g.f_name,g.f_lastinputprice,g.f_saleprice ");
        }
    } else {
        db.exec("select gs.f_id, gs.f_scancode, cp.f_taxname, gr.f_name as f_groupname, gs.f_name as f_goodsname,  "
                "gs.f_saleprice, gs.f_saleprice2, gu.f_name as f_unitname, "
                "gca.f_name as group1, gcb.f_name group2, gcc.f_name as group3, gcd.f_name as group4, "
                "gr.f_taxdept, gr.f_adgcode, 0 as f_qty "
                "from c_goods gs "
                "left join c_groups gr on gr.f_id=gs.f_group "
                "left join c_units gu on gu.f_id=gs.f_unit "
                "left join c_partners cp on cp.f_id=gs.f_supplier "
                "left join c_goods_classes gca on gca.f_id=gs.f_group1 "
                "left join c_goods_classes gcb on gcb.f_id=gs.f_group2 "
                "left join c_goods_classes gcc on gcc.f_id=gs.f_group3 "
                "left join c_goods_classes gcd on gcd.f_id=gs.f_group4 "
                "where gs.f_enabled=1 "
                "order by gr.f_name, gca.f_name ");
    }
    ui->tblGoods->setRowCount(db.rowCount());
    while (db.nextRow()) {
        Goods g;
        g.fScanCode = db.getString("f_scancode");
        g.fCode = db.getString("f_id");
        g.fName = db.getString("f_goodsname");
        g.fUnit = db.getString("f_unitname");
        g.fRetailPrice = db.getDouble("f_saleprice");
        g.fWhosalePrice = db.getDouble("f_saleprice2");
        g.fTaxDept = db.getInt("f_taxdept");
        g.fAdgCode = db.getString("f_adgcode");
        g.fQty = db.getDouble("f_qty");
        fGoods[g.fScanCode] = g;
        fGoodsCodeForPrint[g.fCode.toInt()] = g.fScanCode;
        for (int i = 0; i < db.columnCount(); i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::EditRole, db.getValue(i));
            ui->tblGoods->setItem(row, i, item);
        }
        row++;
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
    if (!fGoods.contains(code)) {
        return;
    }
    w->addGoods(fGoods[code]);
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
    QString title = type == SALE_RETAIL ? tr("Retail") : tr("Whosale");
    ui->tab->addTab(w, QString("%1 #%2").arg(title).arg(ordersCount()));
    ui->tab->setCurrentIndex(ui->tab->count() - 1);
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
    if (__c5config.rdbReplica()) {
        C5Replication *r = new C5Replication(this);
        r->start();
    }
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
    newSale(SALE_RETAIL);
}

void Working::shortcutF2()
{
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
        addGoods(code);
    }
}

void Working::on_btnSaveOrder_clicked()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    if (!w->writeOrder()) {
        return;
    }
    getGoodsList();
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
    if (!w->writeOrder(false)) {
        return;
    }
    getGoodsList();
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
