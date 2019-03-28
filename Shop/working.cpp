#include "working.h"
#include "ui_working.h"
#include "worder.h"
#include "c5database.h"
#include "goods.h"
#include "c5config.h"
#include "c5connection.h"
#include <QShortcut>
#include <QInputDialog>

static QMap<QString, Goods> fGoods;

Working::Working(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Working)
{
    ui->setupUi(this);
    QShortcut *sF1 = new QShortcut(QKeySequence(Qt::Key_F1), this);
    QShortcut *sF2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    QShortcut *sF3 = new QShortcut(QKeySequence(Qt::Key_F3), this);
    QShortcut *sF4 = new QShortcut(QKeySequence(Qt::Key_F4), this);
    QShortcut *sF12 = new QShortcut(QKeySequence(Qt::Key_F12), this);
    QShortcut *sDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
    QShortcut *sUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
    connect(sF1, SIGNAL(activated()), this, SLOT(shortcutF1()));
    connect(sF2, SIGNAL(activated()), this, SLOT(shortcutF2()));
    connect(sF3, SIGNAL(activated()), this, SLOT(shortcutF3()));
    connect(sF4, SIGNAL(activated()), this, SLOT(shortcutF4()));
    connect(sDown, SIGNAL(activated()), this, SLOT(shortcutDown()));
    connect(sUp, SIGNAL(activated()), this, SLOT(shortcutUp()));
    connect(sF12, SIGNAL(activated()), this, SLOT(on_btnSaveOrder_clicked()));
    getGoodsList();
    on_btnNewOrder_clicked();
}

Working::~Working()
{
    delete ui;
}

void Working::getGoodsList()
{
    fGoods.clear();
    C5Database db(C5Config::dbParams());
    db.exec("select gs.f_code, gg.f_id, gg.f_name, gu.f_name, gg.f_saleprice, "
            "gr.f_taxdept, gr.f_adgcode "
            "from c_goods_scancode gs "
            "left join c_goods gg on gg.f_id=gs.f_goods "
            "left join c_groups gr on gr.f_id=gg.f_group "
            "left join c_units gu on gu.f_id=gg.f_unit ");
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
    }
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

void Working::shortcutDown()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    w->nextRow();
}

void Working::shortcutUp()
{
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    w->prevRow();
}

void Working::on_btnNewOrder_clicked()
{
    WOrder *w = new WOrder(this);
    ui->tab->addTab(w, "");
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
    QString code = ui->leCode->text();
    ui->leCode->clear();
    ui->leCode->setFocus();
    WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
    if (!w) {
        return;
    }
    if (!fGoods.contains(code)) {
        return;
    }
    w->addGoods(fGoods[code]);
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

void Working::on_leCode_textChanged(const QString &arg1)
{
    if (arg1 == "+") {
        ui->leCode->clear();
        WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
        if (!w) {
            return;
        }
        w->changeQty();
        return;
    }
    if (arg1 == "*") {
        ui->leCode->clear();
        WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
        if (!w) {
            return;
        }
        w->changePrice();
    }
    if (arg1 == "-") {
        ui->leCode->clear();
        WOrder *w = static_cast<WOrder*>(ui->tab->currentWidget());
        if (!w) {
            return;
        }
        w->removeRow();
    }
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
}
