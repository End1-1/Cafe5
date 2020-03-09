#include "working.h"
#include "ui_working.h"
#include "worder.h"
#include "c5database.h"
#include "goods.h"
#include "printtaxn.h"
#include "sales.h"
#include "c5config.h"
#include "c5connection.h"
#include <QShortcut>
#include <QInputDialog>
#include <QSettings>

static QMap<QString, Goods> fGoods;
static QMap<int, QString> fGoodsCodeForPrint;
static QSettings __s(QString("%1\\%2\\%3").arg(_ORGANIZATION_).arg(_APPLICATION_).arg(_MODULE_));
static int fOrderCount = 0;

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
    QShortcut *sF11 = new QShortcut(QKeySequence(Qt::Key_F11), this);
    QShortcut *sF12 = new QShortcut(QKeySequence(Qt::Key_F12), this);
    QShortcut *sDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
    QShortcut *sUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
    QShortcut *sEsc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(sF1, SIGNAL(activated()), this, SLOT(shortcutF1()));
    connect(sF2, SIGNAL(activated()), this, SLOT(shortcutF2()));
    connect(sF3, SIGNAL(activated()), this, SLOT(shortcutF3()));
    connect(sF4, SIGNAL(activated()), this, SLOT(shortcutF4()));
    connect(sF6, SIGNAL(activated()), this, SLOT(on_btnRefund_clicked()));
    connect(sF7, SIGNAL(activated()), this, SLOT(shortcutF7()));
    connect(sF11, SIGNAL(activated()), this, SLOT(on_btnNewOrder_clicked()));
    connect(sF12, SIGNAL(activated()), this, SLOT(on_btnSaveOrder_clicked()));
    connect(sDown, SIGNAL(activated()), this, SLOT(shortcutDown()));
    connect(sUp, SIGNAL(activated()), this, SLOT(shortcutUp()));
    connect(sEsc, SIGNAL(activated()), this, SLOT(escape()));
    getGoodsList();
    ui->leCode->installEventFilter(this);
    ui->tab->installEventFilter(this);
    C5Database db(C5Config::dbParams());
    db[":f_state"] = ORDER_STATE_CLOSE;
    db[":f_datecash"] = QDate::currentDate();
    db[":f_hall"] = C5Config::defaultHall();
    db.exec("select count(f_id) from o_header where f_datecash=:f_datecash and f_state=:f_state and f_hall=:f_hall");
    if (db.nextRow()) {
        fOrderCount = db.getInt(0);
    }
    on_btnNewOrder_clicked();
    PrintTaxN::fTaxCashier = __c5config.taxCashier();
    PrintTaxN::fTaxPin = __c5config.taxPin();
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

void Working::markDiscount(const QString &customer)
{
    ui->leDiscount->setText(customer);
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
    fGoods.clear();
    C5Database db(C5Config::dbParams());
    db.exec("select gg.f_scancode, gg.f_id, gg.f_name, gu.f_name, gg.f_saleprice, "
            "gr.f_taxdept, gr.f_adgcode "
            "from c_goods gg "
            "left join c_groups gr on gr.f_id=gg.f_group "
            "left join c_units gu on gu.f_id=gg.f_unit "
            "where f_enabled=1");
    while (db.nextRow()) {
        Goods g;
        g.fScanCode = db.getString(0);
        g.fCode = db.getString(1);
        g.fName = db.getString(2);
        g.fUnit = db.getString(3);
        g.fPrice = db.getDouble(4);
        g.fTaxDept = db.getInt(5);
        g.fAdgCode = db.getString(6);
        fGoods[g.fScanCode] = g;
        fGoodsCodeForPrint[g.fCode.toInt()] = g.fScanCode;
    }
    ui->wGoods->setVisible(__s.value("goodslist", false).toBool());
    makeWGoods();
}

void Working::makeWGoods()
{
    QList<int> cw;
    //cw << 0 << 100 << 200 << 200 << 300 << 80 << 80 << 80 << 100 << 100 << 100 << 100 << 0 << 0;
    cw << 0 << 150 << 0 << 200 << 400 << 80 << 0 << 0 << 200 << 0 << 0 << 0 << 0 << 0;
    for (int i = 0; i < cw.count(); i++) {
        ui->tblGoods->setColumnWidth(i, cw.at(i));
    }
    ui->tblGoods->verticalHeader()->setDefaultSectionSize(30);
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    int row = 0;
    C5Database db(C5Config::dbParams());
    db.exec("select gs.f_id, gs.f_scancode, cp.f_taxname, gr.f_name as f_groupname, gs.f_name as f_goodsname,  "
            "gs.f_saleprice, gs.f_saleprice2, gu.f_name as f_unitname, "
            "gca.f_name as group1, gcb.f_name group2, gcc.f_name as group3, gcd.f_name as group4, "
            "gr.f_taxdept, gr.f_adgcode "
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
    ui->tblGoods->setRowCount(db.rowCount());
    while (db.nextRow()) {
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

}

void Working::shortcutF2()
{
    ui->leCode->setFocus();
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

void Working::shortcutF7()
{

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
    WOrder *w = new WOrder(this);
    QObjectList ol = w->children();
    for (QObject *o: ol) {
        auto wd = dynamic_cast<QWidget*>(o);
        if (wd) {
            wd->installEventFilter(this);
        }
    }
    ui->tab->addTab(w, QString::number(++fOrderCount));
    ui->tab->setCurrentIndex(ui->tab->count() - 1);
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
    ui->tab->removeTab(ui->tab->currentIndex());
    if (ui->tab->count() == 0) {
        on_btnNewOrder_clicked();
    }
    w->deleteLater();
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
    ui->tab->removeTab(ui->tab->currentIndex());
    if (ui->tab->count() == 0) {
        on_btnNewOrder_clicked();
    }
    w->deleteLater();
    ui->leDiscount->clear();
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

void Working::on_btnRefund_clicked()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    w->refund();
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
