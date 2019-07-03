#include "working.h"
#include "ui_working.h"
#include "worder.h"
#include "c5database.h"
#include "goods.h"
#include "c5config.h"
#include "c5connection.h"
#include <QShortcut>
#include <QInputDialog>
#include <QSettings>

static QMap<QString, Goods> fGoods;
static QMap<int, QString> fGoodsCodeForPrint;
static QSettings __s(QString("%1\\%2\\%3").arg(_ORGANIZATION_).arg(_APPLICATION_).arg(_MODULE_));

Working::Working(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Working)
{
    ui->setupUi(this);
    QShortcut *sF1 = new QShortcut(QKeySequence(Qt::Key_F1), this);
    QShortcut *sF2 = new QShortcut(QKeySequence(Qt::Key_F2), this);
    QShortcut *sF3 = new QShortcut(QKeySequence(Qt::Key_F3), this);
    QShortcut *sF4 = new QShortcut(QKeySequence(Qt::Key_F4), this);
    QShortcut *sF6 = new QShortcut(QKeySequence(Qt::Key_F6), this);
    QShortcut *sF11 = new QShortcut(QKeySequence(Qt::Key_F11), this);
    QShortcut *sF12 = new QShortcut(QKeySequence(Qt::Key_F12), this);
    QShortcut *sDown = new QShortcut(QKeySequence(Qt::Key_Down), this);
    QShortcut *sUp = new QShortcut(QKeySequence(Qt::Key_Up), this);
    connect(sF1, SIGNAL(activated()), this, SLOT(shortcutF1()));
    connect(sF2, SIGNAL(activated()), this, SLOT(shortcutF2()));
    connect(sF3, SIGNAL(activated()), this, SLOT(shortcutF3()));
    connect(sF4, SIGNAL(activated()), this, SLOT(shortcutF4()));
    connect(sF6, SIGNAL(activated()), this, SLOT(on_btnRefund_clicked()));
    connect(sF11, SIGNAL(activated()), this, SLOT(on_btnNewOrder_clicked()));
    connect(sF12, SIGNAL(activated()), this, SLOT(on_btnSaveOrder_clicked()));
    connect(sDown, SIGNAL(activated()), this, SLOT(shortcutDown()));
    connect(sUp, SIGNAL(activated()), this, SLOT(shortcutUp()));
    getGoodsList();
    on_btnNewOrder_clicked();
    ui->leCode->installEventFilter(this);
    ui->tab->installEventFilter(this);
    fTimerTimeout = 0;
    connect(&fTimer, SIGNAL(timeout()), this, SLOT(timeout()));
    fTimer.start(1000);
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
    return QDialog::eventFilter(watched, event);
}

QString Working::goodsCode(int code) const
{
    if (fGoodsCodeForPrint.contains(code)) {
        return fGoodsCodeForPrint[code] + " ";
    } else {
        return "";
    }
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
    db.exec("select f_code, f_goods from c_goods_scancode where f_receipt=1");
    while (db.nextRow()) {
        fGoodsCodeForPrint[db.getInt("f_goods")] = db.getString("f_code");
    }
    ui->wGoods->setVisible(__s.value("goodslist", false).toBool());
    if (__s.value("goodslist", false).toBool()) {
        makeWGoods();
    }
}

void Working::makeWGoods()
{
    ui->tblGoods->clearContents();
    ui->tblGoods->setRowCount(0);
    int col = 0;
    int row = 0;
    C5Database db(C5Config::dbParams());
    db.exec("select gs.f_code, gg.f_id, gg.f_name as f_goodsname, gu.f_name, gg.f_saleprice, "
            "gr.f_taxdept, gr.f_adgcode "
            "from c_goods_scancode gs "
            "left join c_goods gg on gg.f_id=gs.f_goods "
            "left join c_groups gr on gr.f_id=gg.f_group "
            "left join c_units gu on gu.f_id=gg.f_unit "
            "order by gr.f_name, gg.f_name ");
    while (db.nextRow()) {
        if (ui->tblGoods->rowCount() - 1 < row) {
            ui->tblGoods->setRowCount(ui->tblGoods->rowCount() + 1);
        }
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setText(db.getString("f_goodsname"));
        item->setData(Qt::UserRole, db.getString("f_code"));
        ui->tblGoods->setItem(row, col++, item);
        if (col == 3) {
            col = 0;
            row++;
        }
    }
}

void Working::timeout()
{
    //ui->leCode->setFocus();
    fTimerTimeout++;
    if (fTimerTimeout == 5) {
        if (ui->wGoods->isVisible()) {
            ui->tblGoods->horizontalHeader()->setDefaultSectionSize(ui->tblGoods->width() / 3);
        }
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
    QObjectList ol = w->children();
    for (QObject *o: ol) {
        auto wd = dynamic_cast<QWidget*>(o);
        if (wd) {
            wd->installEventFilter(this);
        }
    }
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

void Working::on_btnExit_clicked()
{
    accept();
}

void Working::on_btnShowGoodsList_clicked()
{
    ui->wGoods->setVisible(!ui->wGoods->isVisible());
    __s.setValue("goodslist", ui->wGoods->isVisible());
    if (ui->wGoods->isVisible()) {
        makeWGoods();
    }
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
