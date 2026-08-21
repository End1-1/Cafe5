#include "dlggoodslist.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QKeyEvent>
#include <QTimer>
#include "c5user.h"
#include "ninterface.h"
#include "struct_workstationitem.h"
#include "ui_dlggoodslist.h"

DlgGoodsList::DlgGoodsList(C5User *user)
    : C5ShopDialog(user)
    , ui(new Ui::DlgGoodsList)
{
    ui->setupUi(this);
    setWindowState(windowState() | Qt::WindowMaximized);
    ui->leSearch->installEventFilter(this);
    ui->tbl->installEventFilter(this);
    ui->tbl->setRowCount(0);
    ui->leTotalRetail->setDouble(0);

    mSearchTimer = new QTimer(this);
    mSearchTimer->setSingleShot(true);
    mSearchTimer->setInterval(400);
    connect(mSearchTimer, &QTimer::timeout, this, &DlgGoodsList::runSearch);
    connect(ui->chName, &QCheckBox::toggled, this, &DlgGoodsList::onSearchCheckboxChanged);
    connect(ui->chScancode, &QCheckBox::toggled, this, &DlgGoodsList::onSearchCheckboxChanged);

    ui->leSearch->setFocus();
}

DlgGoodsList::~DlgGoodsList()
{
    delete ui;
}

void DlgGoodsList::clearResults()
{
    ui->tbl->setRowCount(0);
    ui->leTotalRetail->setDouble(0);
}

void DlgGoodsList::fillResults(const QJsonObject &jo)
{
    const QJsonArray rows = jo.value(QStringLiteral("rows")).toArray();
    ui->tbl->setRowCount(rows.size());
    int row = 0;
    double totalRetail = 0;

    for(const QJsonValue &jv : rows) {
        const QJsonObject o = jv.toObject();
        const double qty = o.value(QStringLiteral("f_qty")).toDouble();
        const double price1 = o.value(QStringLiteral("f_price1")).toDouble();
        const double price2 = o.value(QStringLiteral("f_price2")).toDouble();
        ui->tbl->setData(row, 0, o.value(QStringLiteral("f_goods")).toInt());
        ui->tbl->setData(row, 1, o.value(QStringLiteral("f_groupname")).toString());
        ui->tbl->setData(row, 2, o.value(QStringLiteral("f_scancode")).toString());
        ui->tbl->setData(row, 3, o.value(QStringLiteral("f_name")).toString());
        ui->tbl->setData(row, 4, qty);
        ui->tbl->setData(row, 5, price1);
        ui->tbl->setData(row, 6, price2);
        ui->tbl->setData(row, 7, o.value(QStringLiteral("f_draftqty")).toDouble());
        ui->tbl->setData(row, 8, o.value(QStringLiteral("f_reserve")).toDouble());
        totalRetail += price1 * qty;
        ++row;
    }

    ui->leTotalRetail->setDouble(totalRetail);
    ui->tbl->resizeColumnsToContents();
    if(ui->tbl->rowCount() > 0) {
        ui->tbl->setCurrentCell(0, 0);
    }
}

void DlgGoodsList::runSearch()
{
    const QString q = ui->leSearch->text().trimmed();
    if(q.length() <= 1) {
        clearResults();
        return;
    }
    if(!ui->chName->isChecked() && !ui->chScancode->isChecked()) {
        clearResults();
        return;
    }

    const int gen = ++mSearchGen;
    NInterface::query1(QStringLiteral("/engine/v2/shop/goods-list/get"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("store"), mWorkStation.defaultStoreId()},
                        {QStringLiteral("currency"), 1},
                        {QStringLiteral("q"), q},
                        {QStringLiteral("search_name"), ui->chName->isChecked()},
                        {QStringLiteral("search_scancode"), ui->chScancode->isChecked()}},
                       [this, gen](const QJsonObject &jo) {
                           if(gen != mSearchGen) {
                               return;
                           }
                           fillResults(jo);
                       });
}

void DlgGoodsList::loadAllStock()
{
    mSearchTimer->stop();
    const int gen = ++mSearchGen;
    NInterface::query1(QStringLiteral("/engine/v2/shop/goods-list/get"),
                       mUser->mSessionKey,
                       this,
                       {{QStringLiteral("store"), mWorkStation.defaultStoreId()},
                        {QStringLiteral("currency"), 1},
                        {QStringLiteral("all"), true}},
                       [this, gen](const QJsonObject &jo) {
                           if(gen != mSearchGen) {
                               return;
                           }
                           fillResults(jo);
                       });
}

void DlgGoodsList::onSearchCheckboxChanged()
{
    if(ui->leSearch->text().trimmed().length() > 1) {
        mSearchTimer->start();
    }
}

void DlgGoodsList::acceptCurrentRow()
{
    int r = ui->tbl->currentRow();
    if(r < 0 && ui->tbl->rowCount() > 0) {
        r = 0;
        ui->tbl->setCurrentCell(0, 0);
    }
    if(r < 0) {
        return;
    }

    fGoodsId = ui->tbl->getInteger(r, 0);
    const QString scancode = ui->tbl->getString(r, 2);
    const double stockQty = ui->tbl->getDouble(r, 4);
    emit getGoods(fGoodsId, scancode, stockQty);
    accept();
}

bool DlgGoodsList::handleNavigationKey(int key)
{
    switch(key) {
    case Qt::Key_F5:
        loadAllStock();
        return true;
    case Qt::Key_Up: {
        int r = ui->tbl->currentRow() - 1;
        if(r < 0) {
            r = 0;
        }
        if(ui->tbl->rowCount() > 0) {
            ui->tbl->setCurrentCell(r, 0);
        }
        return true;
    }
    case Qt::Key_Down: {
        int r = ui->tbl->currentRow() + 1;
        if(r < 0) {
            r = 0;
        }
        if(r > ui->tbl->rowCount() - 1) {
            r = ui->tbl->rowCount() - 1;
        }
        if(ui->tbl->rowCount() > 0) {
            ui->tbl->setCurrentCell(r, 0);
        }
        return true;
    }
    case Qt::Key_Enter:
    case Qt::Key_Return:
        acceptCurrentRow();
        return true;
    default:
        return false;
    }
}

bool DlgGoodsList::eventFilter(QObject *watched, QEvent *event)
{
    if((watched == ui->leSearch || watched == ui->tbl)
       && event->type() == QEvent::KeyPress) {
        auto *ke = static_cast<QKeyEvent*>(event);
        if(handleNavigationKey(ke->key())) {
            return true;
        }
    }
    return C5ShopDialog::eventFilter(watched, event);
}

void DlgGoodsList::keyPressEvent(QKeyEvent *event)
{
    if(handleNavigationKey(event->key())) {
        return;
    }
    C5ShopDialog::keyPressEvent(event);
}

void DlgGoodsList::on_leSearch_textChanged(const QString &arg1)
{
    ++mSearchGen;
    if(arg1.trimmed().length() <= 1) {
        mSearchTimer->stop();
        clearResults();
        return;
    }
    mSearchTimer->start();
}

void DlgGoodsList::on_btnExit_clicked()
{
    reject();
}

void DlgGoodsList::on_btnMinimize_clicked()
{
    showMinimized();
}
