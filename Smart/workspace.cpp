#include "workspace.h"
#include "ui_workspace.h"
#include "dlgpassword.h"
#include "menudish.h"
#include "c5user.h"
#include "dishpackage.h"
#include "c5printing.h"
#include "logwriter.h"
#include "printtaxn.h"
#include "bhistory.h"
#include "c5utils.h"
#include "c5airlog.h"
#include "nloadingdlg.h"
#include "dlgqty.h"
#include "dlgservicevalues.h"
#include "change.h"
#include "menudialog.h"
#include "dlglistofdishcomments.h"
#include "c5config.h"
#include "customerinfo.h"
#include "dlgmemoryread.h"
#include "ndataprovider.h"
#include "wcustomerdisplay.h"
#include "c5message.h"
#include "scopeguarde.h"
#include <QScreen>
#include <QScrollBar>
#include <QInputDialog>
#include <QScreen>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QColor>
#include <QTimer>
#include <QSettings>
#include <QPainter>
#include <QFile>
#include <QJsonParseError>

QHash<QString, QString> fPrinterAliases;
Workspace* Workspace::fWorkspace;
static int ordcount = 1;

class TableItemDelegate : public QItemDelegate
{
protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        if(!index.isValid()) {
            QItemDelegate::paint(painter, option, index);
            return;
        }

        painter->save();
        //green
        painter->setBrush(QColor::fromRgb(0, 255, 100));

        //if no order, change background to white
        if(index.data(Qt::UserRole + 1).toString().isEmpty()) {
            painter->setBrush(Qt::white);
        }

        //but if selected table , background must be yellow
        if(index.data(Qt::UserRole + 2).toBool()) {
            painter->setBrush(QColor::fromRgb(252, 186, 0));
        }

        painter->setPen(QColor::fromRgb(100, 100, 100));
        painter->drawRect(option.rect);
        QTextOption to(Qt::AlignCenter);
        painter->drawText(option.rect, index.data(Qt::DisplayRole).toString(), to);
        painter->restore();
    }
};

Workspace::Workspace() :
    C5Dialog(),
    ui(new Ui::Workspace),
    fLoadingDlg(nullptr)
{
    ui->setupUi(this);
    //setWindowFlags(Qt::WindowStaysOnTopHint);
    fTypeFilter = -1;
    QRect r = qApp->screens().at(0)->geometry();

    switch(r.width()) {
    case 1280:
        ui->tblDishes->setColumnCount(3);
        break;

    case 1024:
        ui->tblDishes->setColumnCount(2);
        break;

    default:
        ui->tblDishes->setColumnCount(r.width() / 300);
        break;
    }

    configFiscalButton();
    ui->tblTables->setItemDelegate(new TableItemDelegate());
    resetOrder();
    fTimer = new QTimer(this);
    connect(fTimer, &QTimer::timeout, this, [ = ]() {
        ui->leReadCode->setFocus();
    });
    fTimer->start(1000);
    ui->tblOrder->setColumnWidth(0, 135);
    ui->tblOrder->setColumnWidth(1, 70);
    ui->tblOrder->setColumnWidth(2, 70);
    ui->tblOrder->setColumnWidth(3, 70);
    ui->tblOrder->setColumnWidth(4, 30);
    ui->btnSetCardExternal->setVisible(__c5config.getValue(param_choose_external_pos).toInt() == 1);
    fWorkspace = this;
    fCustomerDisplay = nullptr;
    QSettings d(qApp->applicationDirPath() + "/discount.conf", QSettings::IniFormat);
    QStringList sections = d.childGroups();

    for(const QString &s : sections) {
        d.beginGroup(s);
        QStringList keys = d.allKeys();

        for(const QString &k : keys) {
            fAutoDiscounts[s][k] = d.value(k);
        }

        d.endGroup();
    }

    ui->tblTables->setVisible(__c5config.getRegValue("tables").toBool());
    mRejectEnabled = true;
}

Workspace::~Workspace()
{
    delete ui;
}

bool Workspace::login()
{
    QString pin;

    if(!DlgPassword::getPassword(tr("ENTER"), pin)) {
        accept();
        return false;
    }

    fHttp->fErrorObject = this;
    fHttp->fErrorSlot = (char*)    SLOT(slotHttpError(const QString&));
    createHttpRequest("/engine/login.php", QJsonObject{{"method", 2}, {"pin", pin}},
    SLOT(loginResponse(QJsonObject)),
    "login");
    return true;
}

void Workspace::reject()
{
    if(mRejectEnabled) {
        if(C5Message::question(tr("Confirm to close application")) == QDialog::Accepted) {
            C5Dialog::reject();
        } else {
            return;
        }
    }

    qApp->quit();
}

void Workspace::setQty(double qty, int mode, int rownum, const QString &packageuuid)
{
    int row = rownum;

    if(row < 0) {
        row = ui->tblOrder->currentRow();
    }

    if(row < 0) {
        return;
    }

    Dish d = ui->tblOrder->item(row, 0)->data(Qt::UserRole).value<Dish>();

    if(d.state != DISH_STATE_OK) {
        return;
    }

    if(d.id == C5Config::fMainJson["service_item_code"].toInt()
            || d.id == C5Config::fMainJson["discount_item_code"].toInt()) {
        return;
    }

    QString uuid = packageuuid;

    if(uuid.isEmpty()) {
        uuid = ui->tblOrder->item(row, 0)->data(Qt::UserRole + 1).toString();
    }

    if(d.qty2 > 0.01 || d.qty2 < -0.01) {
        C5Message::error(tr("Not editable"));
        return;
    }

    switch(mode) {
    case 1:
        d.qty = qty;
        break;

    case 2:
        d.qty += qty;
        break;

    case 3:
        if(d.qty - qty < 0.1) {
            on_btnVoid_clicked();
            return;
        } else {
            d.qty -= qty;
        }

        break;
    }

    createHttpRequest("/engine/smart/change-qty.php", QJsonObject{{"row", row}, {"qty", d.qty}, {"price", d.price}, {"obodyid", d.obodyId}},
    SLOT(changeQtyResponse(QJsonObject)));
}

void Workspace::removeDish(int rownum, const QString &packageuuid)
{
    Q_UNUSED(packageuuid);
    int row = rownum;

    if(row < 0) {
        row = ui->tblOrder->currentRow();
    }

    if(row < 0) {
        return;
    }

    Dish d = ui->tblOrder->item(row, 0)->data(Qt::UserRole).value<Dish>();

    if(d.id == C5Config::fMainJson["service_item_code"].toInt()
            || d.id == C5Config::fMainJson["discount_item_code"].toInt()) {
        return;
    }

    if(d.qty2 > 0.01) {
        if(C5Message::question(tr("Confirm to remove printed row")) != QDialog::Accepted) {
            return;
        }
    }

    createHttpRequest("/engine/smart/removedish.php", QJsonObject{{"obodyid", d.obodyId}, {"header", fOrderUuid}}, SLOT(
        removeDishResponse(QJsonObject)));
}

void Workspace::filter(const QString &name)
{
    ui->tblDishes->clear();
    ui->tblDishes->setRowCount(0);
    int row = 0, col = 0;

    foreach(Dish *d, fDishes) {
        if(row > ui->tblDishes->rowCount() - 1) {
            ui->tblDishes->setRowCount(row + 1);

            for(int i = 0; i < ui->tblDishes->columnCount(); i++) {
                ui->tblDishes->setItem(row, i, new QTableWidgetItem());
            }
        }

        if(fTypeFilter > 0) {
            if(d->typeId != fTypeFilter) {
                continue;
            }
        }

        if(fTypeFilter == -1 && name.isEmpty()) {
            if(d->quick == 0) {
                continue;
            }
        }

        if(!name.isEmpty()) {
            if(!d->name.contains(name, Qt::CaseInsensitive)) {
                continue;
            }
        }

        MenuDish *m = new MenuDish(d, !name.isEmpty());

        if(d->color != -1) {
            m->setStyleSheet("background-color:" + QColor::fromRgb(d->color).name());
        }

        ui->tblDishes->setCellWidget(row, col, m);
        col++;

        if(col > ui->tblDishes->columnCount() - 1) {
            col = 0;
            row++;
        }
    }
}

void Workspace::countTotal()
{
    double total = 0;
    double service = 0;
    double serviceValue = C5Config::serviceFactor().toDouble();

    if(fAutoDiscounts.count() > 0) {
        QMap<QString, QList<QString> > dishgroups;
        QMap<QString, double> dishamounts;

        for(const QString &k : fAutoDiscounts.keys()) {
            dishgroups[k] = fAutoDiscounts[k]["items"].toStringList();
        }

        for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
            Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();

            if(d.state != DISH_STATE_OK) {
                continue;
            }

            for(QMap<QString, QList<QString> >::const_iterator it = dishgroups.constBegin(); it != dishgroups.constEnd(); it++) {
                if(it.value().contains(QString::number(d.id))) {
                    dishamounts[it.key()] = dishamounts[it.key()] + (d.qty * d.price);
                }
            }
        }

        bool previousdiscount = fDiscountMode == -100;

        if(previousdiscount) {
            fDiscountMode = 0;
        }

        bool cancelDiscount = false;

        for(QMap<QString, double>::const_iterator ia = dishamounts.constBegin(); ia != dishamounts.constEnd(); ia++) {
            if(fAutoDiscounts[ia.key()]["amount"].toDouble() <= ia.value()) {
                fDiscountMode = -100;

                for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
                    Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();

                    if(fAutoDiscounts[ia.key()]["items"].toStringList().contains(QString::number(d.id))) {
                        d.discount = fAutoDiscounts[ia.key()]["discount"].toDouble() / 100;
                        ui->tblOrder->item(i, 0)->setData(Qt::UserRole, QVariant::fromValue(d));
                    }
                }
            }

            if(fDiscountMode == -100) {
                break;
            } else {
                cancelDiscount = true;
            }
        }

        if(cancelDiscount && previousdiscount) {
            fDiscountMode = 0;

            for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
                Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
                d.discount = 0;
                ui->tblOrder->item(i, 0)->setData(Qt::UserRole, QVariant::fromValue(d));
            }
        }
    }

    fDiscountAmount = 0;

    for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();

        if(d.state != DISH_STATE_OK) {
            continue;
        }

        if(d.id == C5Config::fMainJson["service_item_code"].toInt()
                || d.id == C5Config::fMainJson["discount_item_code"].toInt()) {
            continue;
        }

        switch(fDiscountMode) {
        case CARD_TYPE_DISCOUNT:
            total += (d.price - (d.price * fDiscountValue)) * d.qty;
            fDiscountAmount += (d.price * fDiscountValue) * d.qty;
            break;

        case CARD_TYPE_SPECIAL_DISHES:
            total += (d.price - (d.price * d.discount)) * d.qty;
            fDiscountAmount += (d.price * d.discount) * d.qty;
            break;

        default:
            total += d.price * d.qty;
            break;
        }
    }

    if(fDiscountMode > 0) {
        ui->wDiscount->setVisible(true);

        switch(fDiscountMode) {
        case CARD_TYPE_DISCOUNT:
            ui->leDiscount->setText(QString("-%1% (%2)").arg(fDiscountValue * 100).arg(fDiscountAmount));
            break;

        case CARD_TYPE_SPECIAL_DISHES:
            ui->leDiscount->setText(QString("%1").arg(fDiscountAmount));
            break;
        }
    }

    if(serviceValue > 0) {
        service = total * serviceValue;
        total += service;
    }

    ui->leServiceValue->setText("");

    if(service > 0) {
        ui->leServiceValue->setText(QString("%1 %2% - %3").arg(tr("Service"), float_str(serviceValue * 100, 2),
                                    float_str(service,
                                              2)));

        for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
            Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();

            if(d.id == C5Config::fMainJson["service_item_code"].toInt()) {
                d.price = service;
                d.qty2 = 1;
                ui->tblOrder->item(i, 0)->setData(Qt::UserRole, QVariant::fromValue(d));
                updateInfo(i);
                createHttpRequest("/engine/smart/change-qty.php", QJsonObject{{"row", 1000}, {"qty", d.qty}, {"price", d.price}, {"obodyid", d.obodyId}},
                SLOT(changeQtyResponse(QJsonObject)), "donothing");
                break;
            }
        }
    }

    ui->leTotal->setDouble(total);

    if(ui->btnSetCard->isChecked() || ui->btnSetCardExternal->isChecked()) {
        ui->leCard->setDouble(ui->leTotal->getDouble());
    }

    if(ui->leReceived->getDouble() > 0.01) {
        ui->leChange->setDouble(ui->leReceived->getDouble() - ui->leTotal->getDouble());
    }

    ui->tblOrder->resizeRowsToContents();

    if(fCustomerDisplay) {
        fCustomerDisplay->clear();

        for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
            Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
            fCustomerDisplay->addRow(d.name, float_str(d.qty, 2), float_str(d.price, 2), float_str(d.price * d.qty, 2));
        }

        fCustomerDisplay->setTotal(ui->leTotal->text());
    }
}

void Workspace::resetOrder()
{
    for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
        d.discount = 0;
        d.specialDiscount = 0;
    }

    ui->leCard->clear();
    ui->tblDishes->setEnabled(true);
    ui->wQty->setEnabled(true);
    fOrderUuid.clear();
    ui->leInfo->setVisible(false);
    ui->wDiscount->setVisible(false);
    ui->leTaxpayerId->clear();
    fSupplierName = "";
    fCustomer = 0;
    fDiscountAmount = 0;
    fDiscountMode = 0;
    fDiscountValue = 0;
    fDiscountId = 0;
    fSupplierName = "";
    fPhone = "";
    ui->tblOrder->clearContents();
    ui->tblOrder->setRowCount(0);
    ui->leTotal->setDouble(0);
    ui->leReceived->setDouble(0);
    ui->leChange->setDouble(0);
    ui->leInfo->setVisible(false);
    fDiscountMode = 0;
    fDiscountValue = 0;
    fDiscountAmount = 0;
    ui->btnSetCash->click();

    if(__c5config.getValue(param_tax_print_always_offer).toInt() == 0) {
        ui->btnFiscal->setChecked(false);
    }

    setCustomerPhoneNumber("");
    ui->tblTables->viewport()->update();
    ui->leReadCode->setFocus();
    ui->btnFlagTakeAway->setChecked(false);
}

void Workspace::stretchTableColumns(QTableWidget *t)
{
    int hscrollwidth = 0;

    if(t->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOn) {
        hscrollwidth = 20;
    }

    int freeSpace = t->width() - (t->columnCount() * t->horizontalHeader()->defaultSectionSize()) - 6 - hscrollwidth;
    int delta = freeSpace / t->columnCount();
    t->horizontalHeader()->setDefaultSectionSize(delta + t->horizontalHeader()->defaultSectionSize());
    t->horizontalHeader()->setMinimumSectionSize(t->horizontalHeader()->defaultSectionSize());
}

void Workspace::scrollTable(QTableWidget *t, int direction, int rows)
{
    t->verticalScrollBar()->setValue((t->verticalScrollBar()->value() + (t->verticalHeader()->defaultSectionSize() *rows))
                                     *direction);
}

void Workspace::updateInfo(int row)
{
    Dish d = ui->tblOrder->item(row, 0)->data(Qt::UserRole).value<Dish>();
    QFont f(ui->tblOrder->font());

    if(d.state != DISH_STATE_OK) {
        f.setStrikeOut(true);
    }

    ui->tblOrder->item(row, 0)->setFont(f);
    ui->tblOrder->item(row, 0)->setText(d.name + (d.modificator.isEmpty() ? "" : "\r\n" + d.modificator));
    auto *item = new QTableWidgetItem(float_str(d.qty, 2));
    item->setTextAlignment(Qt::AlignRight);
    f.setItalic(d.qty2 > 0.001);
    f.setBold(d.qty2 > 0.001);
    item->setFont(f);
    ui->tblOrder->setItem(row, 1, item);
    item = new QTableWidgetItem(float_str(d.price - (d.price * d.discount), 2));
    item->setTextAlignment(Qt::AlignRight);
    item->setFont(f);
    ui->tblOrder->setItem(row, 2, item);
    item = new QTableWidgetItem(float_str((d.price - (d.price * d.discount)) *d.qty, 2));
    item->setTextAlignment(Qt::AlignRight);
    item->setFont(f);
    ui->tblOrder->setItem(row, 3, item);

    if(d.f_emarks.isEmpty()) {
        ui->tblOrder->removeCellWidget(row, 4);
    } else {
        QPixmap p(":/qrcode.png");
        auto *l = new QLabel();
        l->setPixmap(p.scaled(QSize(25, 25)));
        ui->tblOrder->setCellWidget(row, 4, l);
    }

    ui->tblOrder->resizeRowsToContents();
}

void Workspace::createHttpRequest(const QString &route, QJsonObject params, const char* responseOkSlot,
                                  const QVariant &marks,
                                  const char* responseErrorSlot)
{
    params["sessionkey"] = __c5config.getRegValue("sessionkey").toString();
    params["host"] = hostinfo;
    params["config_id"] = __c5config.fSettingsName;
    params["user_session"] = __c5config.getRegValue("session").toString();
    params["sessionid"] = __c5config.getRegValue("cashsession").toInt();
    auto np = new NDataProvider(this);
    np->setProperty("marks", marks);
    connect(np, SIGNAL(error(QString)), this, responseErrorSlot);
    connect(np, SIGNAL(done(QJsonObject)), this, responseOkSlot);
    connect(np, &NDataProvider::started, this, &Workspace::httpQueryLoading);
    np->getData(route, params);
}

bool Workspace::service()
{
    for(int i = 0 ; i < ui->tblOrder->rowCount(); i++) {
    }

    return false;
}

void Workspace::on_tblPart2_itemClicked(QTableWidgetItem *item)
{
    if(!item) {
        return;
    }

    fTypeFilter = item->data(Qt::UserRole).toInt();
    filter("");
}

void Workspace::on_btnCheckout_clicked()
{
    ui->btnCheckout->setEnabled(false);
    auto _ = qScopeExit([this] {
        ui->btnCheckout->setEnabled(true);
    });

    if(ui->leTaxpayerId->text().isEmpty() == false) {
        if(ui->leTaxpayerId->text().length() < 8) {
            C5Message::error(tr("Invalid taxpayer id"));
            return;
        }
    }

    bool empty = true;
    bool haveremoved = false;

    for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
        haveremoved = true;
        Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();

        if(d.state == DISH_STATE_OK) {
            empty = false;
            break;
        }
    }

    if(empty) {
        if(haveremoved) {
            fHttp->createHttpQuery("/engine/smart/remove-order.php", QJsonObject{{"id", fOrderUuid}}, SLOT(removeOrderResponse(
                        QJsonObject)));
        }

        return;
    }

    if(ui->btnSetCard->isChecked() || ui->btnSetCardExternal->isChecked()) {
        ui->btnFiscal->setChecked(true);
    }

    if(__c5config.getValue(param_tax_print_if_amount_less).toInt() > 0) {
        if(!ui->btnFiscal->isChecked()) {
            ui->btnFiscal->setChecked(ordcount % __c5config.getValue(param_tax_print_if_amount_less).toInt()  == 0);
        }
    }

    double  cardAmount = 0, idramAmount = 0, otherAmount = 0;

    if(ui->btnSetCash->isChecked()) {
        //cashAmount = ui->leTotal->getDouble();
    } else if(ui->btnSetCard->isChecked() || ui->btnSetCardExternal->isChecked()) {
        cardAmount = ui->leTotal->getDouble();
    } else if(ui->btnSetIdram->isChecked()) {
        idramAmount = ui->leTotal->getDouble();
    } else {
        otherAmount = ui->leTotal->getDouble();
    }

    cardAmount = ui->leCard->getDouble();

    for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();

        if(d.f_emarks.isEmpty() == false) {
            if(__c5config.getValue(param_simple_fiscal).toInt() == 0) {
                ui->btnFiscal->setChecked(true);
            }

            break;
        }
    }

    if(ui->btnFiscal->isChecked() && !ui->btnSetOther->isChecked()) {
        int result;

        do {
            result = printTax(cardAmount, idramAmount);
        } while(result == 0);

        if(result == 2) {
            ui->tblDishes->setEnabled(true);
            ui->wQty->setEnabled(true);
            return;
        }
    }

    saveOrder(ORDER_STATE_CLOSE, true);
    C5Airlog::write(hostinfo, fUser->fullName(), 1, "", fOrderUuid, "", tr("Order saved"), "", "");

    if(ui->btnSetIdram->isChecked()) {
    }
}

void Workspace::on_btnClearFilter_clicked()
{
    fTypeFilter = 0;
    filter("");
}

void Workspace::on_btnPartUp_clicked()
{
    scrollTable(ui->tblPart2, -1, 2);
}

void Workspace::on_btnPartDown_clicked()
{
    scrollTable(ui->tblPart2, 1, 2);
}

void Workspace::on_btnDishUp_clicked()
{
    scrollTable(ui->tblDishes, -1, 5);
}

void Workspace::on_btnDishDown_clicked()
{
    scrollTable(ui->tblDishes, 1, 5);
}

void Workspace::on_btnAny_clicked()
{
    double newQty = 0;

    if(DlgQty::getQty(newQty, "")) {
        setQty(newQty, 1, -1, "");
    }
}

void Workspace::on_btnShowPackages_clicked()
{
    int h = ui->wDishPart->height() - ui->btnShowPackages->height() - ui->btnPartUp->height() - 6;
    QPropertyAnimation *p1 = new QPropertyAnimation(ui->lstCombo, "minimumHeight", this);
    QPropertyAnimation *p2 = new QPropertyAnimation(ui->lstCombo, "maximumHeight", this);
    QPropertyAnimation *p3 = new QPropertyAnimation(ui->tblPart2, "minimumHeight", this);
    QPropertyAnimation *p4 = new QPropertyAnimation(ui->tblPart2, "maximumHeight", this);
    p1->setStartValue(0);
    p1->setEndValue(h - 100);
    p2->setStartValue(0);
    p2->setEndValue(h);
    p3->setStartValue(h - 100);
    p3->setEndValue(0);
    p4->setStartValue(h);
    p4->setEndValue(0);
    p1->setDuration(100);
    p2->setDuration(100);
    p3->setDuration(100);
    p4->setDuration(100);
    QParallelAnimationGroup *ag = new QParallelAnimationGroup(this);
    ag->addAnimation(p1);
    ag->addAnimation(p2);
    ag->addAnimation(p3);
    ag->addAnimation(p4);
    ag->start();
    ui->btnShowPackages->setVisible(false);
    ui->btnShowDishes->setVisible(true);
}

void Workspace::on_btnShowDishes_clicked()
{
    int h = ui->wDishPart->height() - ui->btnShowDishes->height() - ui->btnPartUp->height() - 6;
    QPropertyAnimation *p1 = new QPropertyAnimation(ui->tblPart2, "minimumHeight", this);
    QPropertyAnimation *p2 = new QPropertyAnimation(ui->tblPart2, "maximumHeight", this);
    QPropertyAnimation *p3 = new QPropertyAnimation(ui->lstCombo, "minimumHeight", this);
    QPropertyAnimation *p4 = new QPropertyAnimation(ui->lstCombo, "maximumHeight", this);
    p1->setStartValue(0);
    p1->setEndValue(h - 100);
    p2->setStartValue(0);
    p2->setEndValue(h);
    p3->setStartValue(h - 100);
    p3->setEndValue(0);
    p4->setStartValue(h);
    p4->setEndValue(0);
    p1->setDuration(100);
    p2->setDuration(100);
    p3->setDuration(100);
    p4->setDuration(100);
    QParallelAnimationGroup *ag = new QParallelAnimationGroup(this);
    ag->addAnimation(p1);
    ag->addAnimation(p2);
    ag->addAnimation(p3);
    ag->addAnimation(p4);
    ag->start();
    ui->btnShowPackages->setVisible(true);
    ui->btnShowDishes->setVisible(false);
}

void Workspace::on_lstCombo_itemClicked(QListWidgetItem *item)
{
    const QList<DishPackageMember>& p = DishPackageDriver::fPackageDriver.fPackage[item->data(Qt::UserRole).toInt()];
    //    int row = ui->tblOrder->rowCount();
    //    ui->tblOrder->setRowCount(row + 1);
    //    ui->tblOrder->setSpan(row, 0, 1, 2);
    //    ui->tblOrder->setRowHeight(row, 30);
    //    auto *packageItem = new QTableWidgetItem(item->text());
    //    packageItem->setData(Qt::UserRole + 99, item->data(Qt::UserRole));
    //    packageItem->setData(Qt::UserRole + 100, -1);
    //    packageItem->setData(Qt::UserRole + 101, 1);
    //    packageItem->setData(Qt::UserRole + 102, item->data(Qt::UserRole + 1));
    //    ui->tblOrder->setItem(row, 0, packageItem);
    QString uuid = C5Database::uuid();

    for(const DishPackageMember &dm : p) {
        Dish nd;
        nd.id = dm.fDish;
        nd.adgCode = dm.fAdgCode;
        nd.name = dm.fName;
        nd.price = dm.fPrice;
        nd.store = dm.fStore;
        nd.printer = dm.fPrinter;
        nd.package = dm.fPackage;
        nd.qty = dm.fQty;
        int row = ui->tblOrder->rowCount();
        ui->tblOrder->setRowCount(row + 1);
        ui->tblOrder->setItem(row, 0, new QTableWidgetItem());
        ui->tblOrder->setItem(row, 1, new QTableWidgetItem());
        ui->tblOrder->setItem(row, 2, new QTableWidgetItem());
        ui->tblOrder->setItem(row, 3, new QTableWidgetItem());
        ui->tblOrder->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(nd));
        ui->tblOrder->item(row, 0)->setData(Qt::UserRole + 1, uuid);
        updateInfo(row);
        //        nd.packageName = item->data(Qt::DisplayRole).toString();
        //        row = ui->tblOrder->rowCount();
        //        ui->tblOrder->setRowCount(row + 1);
        //        ui->tblOrder->setItem(row, 0, new QTableWidgetItem(nd.name));
        //        ui->tblOrder->item(row, 0)->setData(Qt::UserRole, qVariantFromValue(nd));
        //        ui->tblOrder->item(row, 0)->setData(Qt::UserRole + 98, -1);
    }

    //    row = ui->tblOrder->rowCount();
    //    ui->tblOrder->setRowCount(row + 1);
    //    ui->tblOrder->setSpan(row, 0, 1, 2);
    //    ui->tblOrder->setRowHeight(row, 10);
    //    packageItem = new QTableWidgetItem(item->text());
    //    packageItem->setData(Qt::UserRole + 100, -2);
    //    ui->tblOrder->setItem(row, 0, packageItem);
    countTotal();
}

void Workspace::on_leReadCode_returnPressed()
{
    QString hya("էթփձջւևրչճԷԹՓՁՋՒևՐՉՃ");
    QString num("12345678901234567890");
    QString oldcode = ui->leReadCode->text();
    ui->leReadCode->clear();

    if(oldcode == " ") {
    }

    QString newcode;

    for(int i = 0; i < oldcode.length(); i++) {
        if(hya.contains(oldcode.at(i))) {
            newcode += num.at(hya.indexOf(oldcode.at(i)));
        } else {
            newcode += oldcode.at(i);
        }
    }

    if(newcode.isEmpty()) {
        return;
    }

    if(newcode.at(0) == "c") {
        double cardvalue = str_float(newcode.right(newcode.length() - 1));

        if(cardvalue <= ui->leTotal->getDouble()) {
            ui->leCard->setDouble(cardvalue);
        }
    }

    QString code = newcode;

    if(!code.isEmpty()) {
        if(code.startsWith("?")) {
            code.remove(0, 1);

            if(code.endsWith(";")) {
                code.remove(code.length() - 1, 1);
            }
        }
    }

    code = code.replace("\'", "\\\'");
    QString emarks;

    if(code.length() >= 29) {
        QString barcode;

        if(code.mid(0, 6) == "000000") {
            barcode = code.mid(6, 8);
        } else if(code.mid(0, 8) == "01000000") {
            barcode = code.mid(8, 8);
        } else if(code.mid(0, 3) == "010") {
            barcode = code.mid(3, 13);
        } else {
            barcode = code.mid(1, 8);

            if(!fDishesBarcode.contains(barcode)) {
                barcode.clear();
            }

            if(barcode.isEmpty()) {
                barcode = code.mid(1, 13);
            }

            if(!fDishesBarcode.contains(barcode)) {
                barcode = "";
            }
        }

        emarks = code;
        code = barcode;

        if(barcode.isEmpty()) {
            C5Message::error(tr("Invalid emarks"));
            return;
        }
    }

    if(fDishesBarcode.contains(code)) {
        Dish dish(fDishesBarcode[code]);
        dish.f_emarks = emarks;
        createAddDishRequest(dish);
        return;
    }

    C5Database db;
    db[":f_code"] = code;
    db.exec("select * from b_cards_discount where f_code=:f_code");

    if(db.nextRow()) {
        if(fDiscountMode > 0) {
            C5Message::error(tr("Discount already exists"));
            return;
        }

        fDiscountCard = db.getInt("f_id");
        fDiscountMode = db.getInt("f_mode");
        fDiscountValue = db.getDouble("f_value");

        switch(fDiscountMode) {
        case CARD_TYPE_DISCOUNT:
            fDiscountValue /= 100;
            break;

        case CARD_TYPE_SPECIAL_DISHES:
            for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
                Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
                d.discount = d.specialDiscount;
                ui->tblOrder->item(i, 0)->setData(Qt::UserRole, QVariant::fromValue(d));
                updateInfo(i);
            }

            break;
        }

        countTotal();
    }
}

void Workspace::on_btnCostumer_clicked()
{
    if(fTable == 0) {
        C5Message::error(tr("Select table"));
        return;
    }

    if(ui->lbCostumerPhone->isVisible()) {
        if(C5Message::question(tr("Confirm to remove customer informaion")) == QDialog::Accepted) {
            fCustomer = 0;
            ui->lbCostumerPhone->setVisible(false);
            return;
        }
    }

    QString phone, address, name;
    int id;

    if(CustomerInfo::getCustomer(id, name, phone, address)) {
        fCustomer = id;
        ui->lbCostumerPhone->setText(QString("%1\r\n%2\r\n%3").arg(phone, name, address));
        ui->lbCostumerPhone->setVisible(true);
    }

    if(!fOrderUuid.isEmpty()) {
        createHttpRequest("/engine/smart/set-customer.php", QJsonObject{
            {"header", fOrderUuid},
            {"customer", fCustomer}}, SLOT(
            voidResponse(QJsonObject)));
    }
}

void Workspace::setCustomerPhoneNumber(const QString &number)
{
    ui->lbCostumerPhone->setText(number);
    ui->lbCostumerPhone->setVisible(number.isEmpty() == false);
}

void Workspace::on_tblDishes_cellClicked(int row, int column)
{
    if(fTable == 0) {
        C5Message::error(tr("Select table"));
        return;
    }

    MenuDish *md = static_cast<MenuDish*>(ui->tblDishes->cellWidget(row, column));

    if(md == nullptr) {
        return;
    }

    Dish *d = md->fDish;

    if(!d) {
        return;
    }

    createAddDishRequest(Dish(d));
}

void Workspace::on_btnP1_clicked()
{
    setQty(1, 2, -1, "");
}

void Workspace::on_btnM1_clicked()
{
    setQty(1, 3, -1, "");
}

void Workspace::on_btnP05_clicked()
{
    setQty(0.5, 1, -1, "");
}

void Workspace::on_btnVoid_clicked()
{
    removeDish(-1, "");
}

void Workspace::on_btnSetCash_clicked()
{
    ui->btnSetCash->setChecked(true);
    ui->btnSetCard->setChecked(false);
    ui->btnSetIdram->setChecked(false);
    ui->btnSetOther->setChecked(false);
    ui->btnSetCardExternal->setChecked(false);
    ui->leCard->clear();
    configFiscalButton();
}

void Workspace::on_btnSetCard_clicked()
{
    ui->btnSetCash->setChecked(false);
    ui->btnSetCard->setChecked(true);
    ui->btnSetCardExternal->setChecked(false);
    ui->btnSetIdram->setChecked(false);
    ui->btnSetOther->setChecked(false);
    ui->btnFiscal->setChecked(true);
    ui->leCard->setDouble(ui->leTotal->getDouble());
}

void Workspace::on_btnSetIdram_clicked()
{
    ui->btnSetCash->setChecked(false);
    ui->btnSetCard->setChecked(false);
    ui->btnSetIdram->setChecked(true);
    ui->btnSetOther->setChecked(false);
    ui->btnSetCardExternal->setChecked(false);
    ui->btnFiscal->setChecked(true);
    ui->leCard->clear();
    //    if (ui->tblOrder->rowCount() == 0) {
    //        on_btnSetCash_clicked();
    //        C5Message::info(tr("Empty order"));
    //        return;
    //    }
    //    if (fOrderUuid.isEmpty() && C5Message::question(tr("Order will saved, continue?")) != QDialog::Accepted) {
    //        on_btnSetCash_clicked();
    //        return;
    //    }
    //    if (fOrderUuid.isEmpty()) {
    //        saveOrder();
    //    }
    //    ui->btnSetCash->setChecked(false);
    //    ui->btnSetCard->setChecked(false);
    //    ui->btnSetIdram->setChecked(true);
    //    ui->btnSetOther->setChecked(false);
    //    if (__c5config.getValue(param_idram_id).length() > 0 && !__c5config.localReceiptPrinter().isEmpty()) {
    //        QFont font(qApp->font());
    //        font.setPointSize(24);
    //        C5Printing p;
    //        p.setSceneParams(650, 2800, QPageLayout::Portrait);
    //        p.setFont(font);
    //        p.setFontBold(true);
    //        p.ctext(fOrderUuid);
    //        p.br();
    //        p.setFontBold(false);
    //        p.line(3);
    //        p.br(3);
    //        p.br();
    //        p.br();
    //        p.ctext(tr("Amount to paid"));
    //        p.br();
    //        p.ctext(ui->leTotal->text());
    //        p.br();
    //        p.br();
    //        p.ctext(QString::fromUtf8("Վճարեք Idram-ով"));
    //        p.br();
    //        int levelIndex = 1;
    //        int versionIndex = 0;
    //        bool bExtent = true;
    //        int maskIndex = -1;
    //        QString encodeString = QString("%1;%2;%3;%4|%5;%6;%7")
    //                .arg(__c5config.getValue(param_idram_name))
    //                .arg(__c5config.getValue(param_idram_id)) //IDram ID
    //                .arg(str_float(ui->leTotal->text()))
    //                .arg(fOrderUuid)
    //                .arg(__c5config.getValue(param_idram_phone))
    //                .arg(__c5config.getValue(param_idram_tips).toInt() == 1 ? "1" : "0")
    //                .arg(__c5config.getValue(param_idram_tips).toInt() == 1 ? __c5config.getValue(param_idram_tips_wallet) : "");
    //        CQR_Encode qrEncode;
    //        bool successfulEncoding = qrEncode.EncodeData( levelIndex, versionIndex, bExtent, maskIndex, encodeString.toUtf8().data() );
    //        if (!successfulEncoding) {
    ////            fLog.append("Cannot encode qr image");
    //        }
    //        int qrImageSize = qrEncode.m_nSymbleSize;
    //        int encodeImageSize = qrImageSize + ( QR_MARGIN * 2 );
    //        QImage encodeImage(encodeImageSize, encodeImageSize, QImage::Format_Mono);
    //        encodeImage.fill(1);
    //        for ( int i = 0; i < qrImageSize; i++ ) {
    //            for ( int j = 0; j < qrImageSize; j++ ) {
    //                if ( qrEncode.m_byModuleData[i][j] ) {
    //                    encodeImage.setPixel(i + QR_MARGIN, j + QR_MARGIN, 0);
    //                }
    //            }
    //        }
    //        QPixmap pix = QPixmap::fromImage(encodeImage);
    //        pix = pix.scaled(300, 300);
    //        p.image(pix, Qt::AlignHCenter);
    //        p.br();
    //        /* End QRCode */
    //        p.ltext(QString("%1 %2").arg(tr("Printed:")).arg(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR)), 0);
    //        p.br();
    //        p.ltext("_", 0);
    //        p.br();
    //        p.print(C5Config::localReceiptPrinter(), QPageSize::Custom);
    //    }
}

void Workspace::on_btnSetOther_clicked()
{
    ui->btnSetCash->setChecked(false);
    ui->btnSetCard->setChecked(false);
    ui->btnSetIdram->setChecked(false);
    ui->btnSetOther->setChecked(true);
    ui->btnSetCardExternal->setChecked(false);
    ui->leCard->clear();
    ui->btnFiscal->setChecked(false);
}

void Workspace::on_btnReceived_clicked()
{
    double v = ui->leTotal->getDouble();

    if(Change::getReceived(v)) {
        ui->leReceived->setDouble(v);
    }
}

void Workspace::on_leReceived_textChanged(const QString &arg1)
{
    ui->leChange->setDouble(str_float(arg1) - ui->leTotal->getDouble());
}

bool Workspace::saveOrder(int state, bool printService)
{
    double cashAmount = 0, cardAmount = 0, idramAmount = 0, otherAmount = 0;

    if(ui->btnSetCash->isChecked()) {
        cashAmount = ui->leTotal->getDouble() - ui->leCard->getDouble();
    } else if(ui->btnSetCard->isChecked() || ui->btnSetCardExternal->isChecked()) {
        cardAmount = ui->leCard->getDouble();
    } else if(ui->btnSetIdram->isChecked()) {
        idramAmount = ui->leTotal->getDouble();
    } else {
        otherAmount = ui->leTotal->getDouble();
    }

    QJsonObject header;
    header["id"] = fOrderUuid;
    header["state"] = state;
    header["hall"] = __c5config.defaultHall();
    header["table"] = fTable;
    header["phone"] = fPhone;
    header["amounttotal"] = ui->leTotal->getDouble();
    header["amountcash"] = cashAmount;
    header["amountcard"] = cardAmount;
    header["amountother"] = otherAmount;
    header["amountidram"] = idramAmount;
    header["amountdiscount"] = fDiscountAmount;
    header["discountfactor"] = fDiscountValue;
    header["partner"] = fCustomer;
    header["taxpayertin"] = ui->leTaxpayerId->text();
    header["sessionid"] = __c5config.getRegValue("sessionid").toInt();
    QJsonObject bhistory;
    bhistory["card"] = fDiscountCard;
    bhistory["type"] = fDiscountMode;
    bhistory["value"] = fDiscountValue;
    bhistory["data"] = fDiscountAmount;
    QJsonObject flags;
    flags["f1"] = fCustomer == 0 ? 0 :  1;
    //flags["f2"] = 0;
    flags["f3"] = ui->btnFlagTakeAway->isChecked() ? 1 : 0;
    flags["f4"] = 0;
    flags["f5"] = 0;
    double disc = discountValue();
    QJsonArray dishes;

    for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
        Dish d = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();
        QJsonObject jo;
        jo["obodyid"] = d.obodyId;
        jo["state"] = d.state;
        jo["dishid"] = d.id;
        jo["qty"] = d.qty;
        jo["qty2"] = d.qty2;
        jo["price"] = d.price;
        jo["total"] = (d.qty * d.price) - (d.qty * d.price * disc);
        jo["service"] = __c5config.serviceFactor().toDouble();
        jo["discount"] = d.discount;
        jo["store"] = d.store;
        jo["print1"] = d.printer;
        jo["print2"] = d.printer2;
        jo["comment"] = d.modificator;
        jo["remind"] = 0;
        jo["adgcode"] = d.adgCode;
        jo["removereason"] = 0;
        jo["timeorder"] = 0;
        jo["package"] = 0;
        jo["row"] = i;
        jo["emarks"] = d.f_emarks.isEmpty() ? QJsonValue::Null : QJsonValue(d.f_emarks);
        dishes.append(jo);
    }

    QJsonObject marks;

    if(state == ORDER_STATE_CLOSE || printService) {
        marks["printservice"] = true;
        marks["printserviceid"] = fOrderUuid;
        marks["printreceipt"] = true;
        marks["header"] = fOrderUuid;
    }

    createHttpRequest("/engine/smart/save.php", QJsonObject{
        {"header", header},
        {"bhistory", bhistory},
        {"dishes", dishes},
        {"flags", flags}},
    SLOT(saveResponse(QJsonObject)), marks);
    return true;
}

int Workspace::printTax(double cardAmount, double idramAmount)
{
    QElapsedTimer et;
    et.start();
    C5Database db;
    db[":f_header"] = fOrderUuid;
    db[":f_state"] = DISH_STATE_OK;
    db.exec("select b.f_adgcode, b.f_dish, d.f_name, b.f_price, b.f_discount, b.f_qty1,"
            "b.f_emarks, d.f_part, "
            "p2.f_taxdept "
            "from o_body b "
            "left join d_dish d on d.f_id=b.f_dish "
            "left join d_part2 p2 on p2.f_id=d.f_part "
            "where b.f_header=:f_header and b.f_state=:f_state");
    QString useExtPos = C5Config::taxUseExtPos();

    if(idramAmount > 0.01) {
        cardAmount = idramAmount;
        useExtPos = "true";
    }

    if(ui->btnSetCardExternal->isChecked()) {
        useExtPos = "true";
    }

    QTableWidgetItem *citem = ui->tblTables->currentItem();

    if(!citem) {
        C5Message::error(tr("Program error QTableWidgetItem *citem = ui->tblTables->currentItem();"));
        return false;
    }

    QJsonObject jc = citem->data(Qt::UserRole + 5).toJsonObject();
    PrintTaxN pt(jc["tax_ip"].toString(),
                 jc["tax_port"].toString().toInt(),
                 jc["tax_password"].toString(), useExtPos,
                 jc["tax_op"].toString(),
                 jc["tax_pin"].toString(), this);

    if(ui->leTaxpayerId->text().isEmpty() == false) {
        pt.fPartnerTin = ui->leTaxpayerId->text();
    }

    while(db.nextRow()) {
        if(db.getDouble("f_price") < 0.01) {
            continue;
        }

        if(!db.getString("f_emarks").isEmpty()) {
            pt.fEmarks.append(db.getString("f_emarks"));
        }

        if(__c5config.getValue(param_simple_fiscal).toInt() > 0) {
            continue;
        }

        switch(fDiscountMode) {
        case CARD_TYPE_DISCOUNT:
            pt.addGoods(db.getInt("f_taxdept") == 0
                        ? jc["tax_dept"].toString().toInt()
                        : db.getInt("f_taxdept"),
                        db.getString("f_adgcode"), QString::number(db.getInt("f_dish")),
                        db.getString("f_name"), db.getDouble("f_price"),
                        db.getDouble("f_qty1"), fDiscountValue * 100);
            break;

        case CARD_TYPE_SPECIAL_DISHES:
            pt.addGoods(db.getInt("f_taxdept") == 0
                        ? jc["tax_dept"].toString().toInt()
                        : db.getInt("f_taxdept"),
                        db.getString("f_adgcode"), db.getString("f_dish"),
                        db.getString("f_name"), db.getDouble("f_price"),
                        db.getDouble("f_qty1"), db.getDouble("f_discount") * 100);
            break;

        default:
            pt.addGoods(db.getInt("f_taxdept") == 0
                        ? jc["tax_dept"].toString().toInt()
                        : db.getInt("f_taxdept"),
                        db.getString("f_adgcode"), db.getString("f_dish"),
                        db.getString("f_name"), db.getDouble("f_price"),
                        db.getDouble("f_qty1"), db.getDouble("f_discount") * 100);
            break;
        }
    }

    QString jsonIn, jsonOut, err;
    int result = 0;

    if(__c5config.getValue(param_simple_fiscal).toInt() == 0) {
        result = pt.makeJsonAndPrint(cardAmount, 0, jsonIn, jsonOut, err);
    } else {
        pt.fJsonHeader["paidAmount"] = ui->leTotal->getDouble();
        result = pt.makeJsonAndPrintSimple(jc["tax_dept"].toString().toInt(), cardAmount, 0, jsonIn, jsonOut, err);
    }

#ifdef QT_DEBUG

    if(result != 0) {
        result = 0;
        jsonOut = "{\"address\": \"ԿԵՆՏՐՈՆ ԹԱՂԱՄԱՍ Հյուսիսային պողոտա 1/1 \", "
                  "\"change\": 0, \"crn\": \"53241359\", \"fiscal\": \"81326142\", "
                  "\"lottery\": \"\", \"prize\": 0, \"rseq\": 7174, \"sn\": \"00022164315\", "
                  "\"taxpayer\": \"«ՍՄԱՐՏ ՊԱՐԿԻՆԳ»\", \"time\": 1733052744891, \"tin\": \"02853428\", \"total\": 1}";
    }

#else

    if(result != 0) {
        LogWriter::write(LogWriterLevel::errors, "Fiscal request", jsonIn);
        LogWriter::write(LogWriterLevel::errors, "Fiscal response", jsonOut);
    }

#endif
    QJsonObject jtax;
    QJsonParseError jsonErr;
    jtax["f_order"] = fOrderUuid;
    jtax["f_elapsed"] = et.elapsed();
    jtax["f_in"] = QJsonDocument::fromJson(jsonIn.replace("'", "''").toUtf8(), &jsonErr).object();
    jtax["f_out"] = QJsonDocument::fromJson(jsonOut.toUtf8()).object();
    jtax["f_err"] = err;
    jtax["f_result"] = result;
    jtax["f_state"] = result == pt_err_ok ? 1 : 0;

    if(jsonErr.error != QJsonParseError::NoError) {
        C5Message::error(jsonIn);
    }

    QString sql = QString(QJsonDocument(jtax).toJson(QJsonDocument::Compact));
    sql.replace("\\\"", "\\\\\"");
    sql.replace("'", "\\'");
    db.exec(QString("call sf_create_shop_tax('%1')").arg(sql));

    if(result != pt_err_ok) {
        switch(C5Message::question(err, tr("Try again"), tr("Do not print fiscal"), tr("Return to editing"))) {
        case QDialog::Rejected:
            C5Airlog::write(hostinfo, fUser->fullName(), 1, "", fOrderUuid, "", tr("Fiscal fail, continue without fiscal"), "", "");
            return 1;

        case QDialog::Accepted:
            C5Airlog::write(hostinfo, fUser->fullName(), 1, "", fOrderUuid, "", tr("Fiscal fail, try again"), "", "");
            return 0;

        case 2:
            C5Airlog::write(hostinfo, fUser->fullName(), 1, "", fOrderUuid, "", tr("Return to edit"), "", "");
            return 2;
        }
    }

    return -1;
}

bool Workspace::printReceipt(const QString &id, bool printSecond, bool precheck)
{
    C5Database db;
    QFont font(qApp->font());
    font.setFamily(__c5config.getValue(param_receipt_print_font_family));
    int basefont = __c5config.getValue(param_receipt_print_font_size).toInt();
    font.setPointSize(basefont);
    font.setBold(true);
    C5Printing p;
    p.setSceneParams(650, 2800, QPageLayout::Portrait);
    p.setFont(font);
    QMap<int, QMap<QString, QVariant> > packages;
    QJsonObject jtax, jin;
    db[":f_order"] = id;
    db.exec("select f_in, f_out from o_tax_log where f_order=:f_order and f_state=1");

    if(db.nextRow()) {
        jtax = QJsonDocument::fromJson(db.getString("f_out").toUtf8()).object();
        jin = QJsonDocument::fromJson(db.getString("f_in").toUtf8()).object();
    }

    if(QFile::exists("./logo_receipt.png")) {
        p.image("./logo_receipt.png", Qt::AlignHCenter);
        p.br();
    }

    db[":f_header"] = id;
    db.exec("select p.f_id, d.f_name, p.f_qty, p.f_price "
            "from o_package p "
            "inner join d_package d on d.f_id=p.f_package "
            "where p.f_header=:f_header");

    while(db.nextRow()) {
        QMap<QString, QVariant> p;

        for(int i = 0; i < db.columnCount(); i++) {
            p[db.columnName(i)] = db.getValue(i);
        }

        packages[db.getInt("f_id")] = p;
    }

    BHistory bh;
    bh.id = id;
    bh.getRecord(db);
    db[":f_id"] = id;
    db.exec("select o.f_prefix, o.f_hallid, t.f_firmname, t.f_address, t.f_dept, t.f_hvhh, t.f_devnum, "
            "t.f_serial, t.f_fiscal, t.f_receiptnumber, t.f_time as f_taxtime, "
            "concat(left(u.f_first, 1), '. ', u.f_last) as f_staff, "
            "o.f_amountcash, o.f_amountcard, o.f_amountidram, o.f_amountother, o.f_amounttotal, o.f_print, "
            "o.f_comment, ht.f_name as f_tablename, "
            "p.f_contact, p.f_phone, p.f_address, o.f_taxpayertin "
            "from o_header o "
            "left join o_tax t on t.f_id=o.f_id "
            "left join s_user u on u.f_id=o.f_staff "
            "left join h_tables ht on ht.f_id=o.f_table "
            "left join c_partners p on p.f_id=o.f_partner "
            "where o.f_id=:f_id");

    if(!db.nextRow()) {
        C5Message::error(tr("Invalid order number"));
        return false;
    }

    p.setFontBold(true);
    p.ctext(tr("Receipt #") + QString("%1%2").arg(db.getString("f_prefix"), QString::number(db.getInt("f_hallid"))));
    p.br();

    //p.setFontBold(false);
    if(!jtax.isEmpty()) {
        p.ltext(jtax["taxpayer"].toString(), 0);
        p.br();
        p.ltext(jtax["address"].toString(), 0);
        p.br();
        p.ltext(tr("Taxpayer tin"), 0);
        p.rtext(jtax["tin"].toString());
        p.br();
        p.ltext(tr("Device number"), 0);
        p.rtext(jtax["crn"].toString());
        p.br();
        p.ltext(tr("Serial"), 0);
        p.rtext(jtax["sn"].toString());
        p.br();
        p.ltext(tr("Fiscal"), 0);
        p.rtext(jtax["fiscal"].toString());
        p.br();
        p.ltext(tr("Receipt number"), 0);
        p.rtext(QString::number(jtax["rseq"].toInt()));
        p.br();

        if(jin["taxpayerTin"].toString().isEmpty() == false) {
            p.ltext(tr("Partner tin"), 0);
            p.rtext(jin["taxpayerTin"].toString());
            p.br();
        }

        p.ltext(tr("(F)"), 0);
        p.br();
    }

    p.ltext(tr("Cashier"), 0);
    p.rtext(db.getString("f_staff"));
    p.br();
    p.ltext(tr("Table"), 0);
    p.rtext(db.getString("f_tablename"));
    p.br();
    p.line(3);
    p.br(2);
    p.setFontBold(true);
    p.ctext(tr("Class | Name | Qty | Price | Total"));
    //p.setFontBold(false);
    p.br();
    p.line(3);
    p.br(3);
    QMap<double, QString> discs;
    C5Database dd;
    dd[":f_header"] = id;
    dd[":f_state"] = DISH_STATE_OK;
    dd.exec("select b.f_adgcode, b.f_dish, d.f_name, b.f_price as f_clearprice, "
            "b.f_price - (b.f_price * b.f_discount) as f_price, b.f_qty1, "
            "b.f_package, b.f_comment, b.f_discount "
            "from o_body b "
            "left join d_dish d on d.f_id=b.f_dish "
            "where b.f_header=:f_header and b.f_state=:f_state "
            "order by b.f_package, b.f_row ");
    int package = 0;
    double clearTotal = 0;
    double service = 0;

    while(dd.nextRow()) {
        if(dd.getInt("f_dish") != C5Config::fMainJson["service_item_code"].toInt()) {
            clearTotal += dd.getDouble("f_qty1") * dd.getDouble("f_clearPrice");
        } else {
            service = dd.getDouble("f_qty1") * dd.getDouble("f_clearPrice");
        }

        if(dd.getInt("f_package") > 0) {
            if(package != dd.getInt("f_package")) {
                if(package > 0) {
                    p.br();
                    p.line();
                    p.br(2);
                    package = 0;
                }

                p.setFontBold(true);
                p.setFontSize(basefont);
                package = dd.getInt("f_package");
                p.ltext(packages[package]["f_name"].toString(), 0);
                p.br();
                p.ltext(QString("%1 x %2 = %3")
                        .arg(float_str(packages[package]["f_qty"].toDouble(), 2))
                        .arg(packages[package]["f_price"].toDouble(), 2)
                        .arg(packages[package]["f_qty"].toDouble() *packages[package]["f_price"].toDouble()), 0);
                p.br();
                //p.setFontBold(false);
                p.setFontSize(basefont);
            }

            p.ltext(QString("*** %1 %2, %3 x%4").arg(tr("Class:"), dd.getString("f_adgcode"), dd.getString("f_name"),
                    float_str(dd.getDouble("f_qty1"), 2)), 0);
            p.br();
        } else {
            if(package > 0) {
                p.br();
                p.line();
                p.br(2);
                package = 0;
            }

            QString discstr;

            if(dd.getDouble("f_discount") > 0.001) {
                if(discs.contains(dd.getDouble("f_discount"))) {
                    discstr = discs[dd.getDouble("f_discount")];
                } else {
                    int l = 0;

                    for(QMap<double, QString>::const_iterator it = discs.constBegin(); it != discs.constEnd(); it++) {
                        l = it.value().length() > l ? it.value().length() : l;
                    }

                    l++;

                    for(int i = 0; i < l; i++) {
                        discstr += "*";
                    }

                    discs[dd.getDouble("f_discount")] = discstr;
                }
            }

            if(dd.getString(0).isEmpty()) {
                p.ltext(QString("%1 %2").arg(dd.getString("f_name"), discstr), 0);
            } else {
                p.ltext(QString("%1, %2 %3").arg(dd.getString("f_adgcode"), dd.getString("f_name"), discstr), 0);
            }

            if(__c5config.getValue(param_print_modificators_on_receipt).toInt() == 1) {
                if(!dd.getString("f_comment").isEmpty()) {
                    p.br();
                    p.ltext(dd.getString("f_comment"), 0);
                }
            }

            p.br();
            p.ltext(QString("%1 x %2 = %3").arg(float_str(dd.getDouble("f_qty1"), 2)).arg(dd.getDouble("f_price"),
                    2).arg(float_str(dd.getDouble("f_qty1") *dd.getDouble("f_price"), 2)), 0);
            p.br();
            p.line();
            p.br(2);
        }
    }

    p.line(4);
    p.br(3);
    p.setFontBold(true);

    if(bh.type > 0) {
        p.ltext(QString("%1").arg(tr("Total")), 0);
        p.rtext(float_str(clearTotal, 2));
        p.br();
        p.ltext(QString("%1 %2%").arg(tr("Discount"), float_str(bh.value * 100, 2)), 0);
        p.rtext(float_str(bh.data, 2));
        p.br();
    }

    p.ltext(tr("Need to pay"), 0);
    p.rtext(float_str(clearTotal  + service, 2));
    p.br();
    p.br();
    p.line();
    p.br();

    for(QMap<double, QString>::const_iterator it = discs.constBegin(); it != discs.constEnd(); it++) {
        p.ltext(QString("%1 %2 %3%").arg(it.value(), tr("Discount"), QString::number(it.key() * 100)), 0);
        p.br();
    }

    p.br();

    if(db.getDouble("f_amountcash") > 0.001) {
        p.ltext(tr("Payment, cash"), 0);
        p.rtext(float_str(db.getDouble("f_amountcash"), 2));
    }

    if(db.getDouble("f_amountcard") > 0.001) {
        p.ltext(tr("Payment, card"), 0);
        p.rtext(float_str(db.getDouble("f_amountcard"), 2));
    }

    if(db.getDouble("f_amountidram") > 0.001) {
        p.ltext(tr("Payment, Idram"), 0);
        p.rtext(float_str(db.getDouble("f_amountidram"), 2));
    }

    if(db.getDouble("f_amountother") > 0.001) {
        p.ltext(tr("Payment, other"), 0);
        p.rtext(float_str(db.getDouble("f_amountother"), 2));
    }

    if(db.getString("f_phone").length() > 0) {
        p.br();
        p.br();
        p.ltext(tr("Customer"), 0);
        p.br();
        p.ltext(db.getString("f_phone"), 0);
        p.br();

        if(!db.getString("f_address").isEmpty()) {
            p.ltext(db.getString("f_address"), 0);
            p.br();
        }

        if(!db.getString("f_contact").isEmpty()) {
            p.ltext(db.getString("f_contact"), 0);
            p.br();
        }
    }

    p.setFontSize(basefont - 2);
    p.setFontBold(true);
    p.br();
    p.ltext(__c5config.getValue(param_recipe_footer_text), 0);
    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.ltext(QString("%1: %2").arg(tr("Sample")).arg(db.getInt("f_print") + 1), 0);
    p.br();
    p.ltext(".", 0);
    p.print(C5Config::localReceiptPrinter(), QPageSize::Custom);
    db[":f_print"] = db.getInt("f_print") + 1;
    db.update("o_header", "f_id", id);

    if(printSecond) {
        printReceipt(id, false, precheck);
    }

    return true;
}

void Workspace::printService(const QString &printerName, const QJsonObject &h, const QJsonArray &d,
                             const QJsonObject &jf, int side)
{
    Q_UNUSED(jf);
    QFont font(qApp->font());
    font.setFamily(__c5config.getValue(param_service_print_font_family));
    int basesize = __c5config.getValue(param_service_print_font_size).toInt();
    font.setPointSize(basesize);
    C5Printing p;
    p.setSceneParams(650, 2800, QPageLayout::Portrait);
    p.setFont(font);
    p.setFontBold(true);
    p.ctext(tr("Receipt #") + QString("%1%2").arg(h["f_prefix"].toString(), QString::number(h["f_hallid"].toInt())));
    p.br();
    p.lrtext(tr("Cashier"), h["f_staff"].toString());
    p.br();
    p.ctext(printerName);
    p.br();

    if(h["f_partner"].toInt() > 0) {
        p.ctext(QString("*%1*").arg(tr("Delivery")));
        p.br();
    }

    if(h["modified"].toBool()) {
        p.ctext(QString("*%1*").arg(tr("Edited")));
        p.br();
    }

    if(jf["f_3"].toInt() > 0) {
        p.setFontSize(basesize + 4);
        p.setFontBold(true);
        p.ctext(tr("TAKE AWAY"));
        p.br();
    }

    //p.setFontBold(false);
    p.line(3);
    p.br(3);
    p.br();

    for(int i = 0; i < d.size(); i++) {
        const QJsonObject &j = d.at(i).toObject();
        p.setFontSize(basesize);
        p.setFontBold(true);
        p.setFontStrike(j["f_state"].toInt() != 1);
        p.ltext(j["f_name"].toString(), 0);
        p.setFontSize(basesize + 8);
        p.setFontBold(true);
        p.rtext(float_str(j["f_qty1"].toDouble(), 2));
        p.br();

        if(j["f_comment"].toString().isEmpty() == false) {
            p.setFontSize(basesize - 4);
            p.ltext(j["f_comment"].toString(), 0);
            p.br();
        }

        p.line();
        p.br();
    }

    p.setFontStrike(false);
    p.setFontSize(basesize - 4);
    // p.setFontBold(false);
    p.ltext(QString("%1 %2 [%3]").arg(tr("[1] Printed:"), QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR),
                                      QString::number(side)), 0);
    p.br();
    p.br();
    p.ltext("_", 0);
    p.br();
    QString finalPrinter = printerName;

    if(fPrinterAliases.contains(printerName)) {
        finalPrinter = fPrinterAliases[printerName];
    }

    p.print(finalPrinter, QPageSize::Custom);
}

void Workspace::showCustomerDisplay()
{
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);

    if(fCustomerDisplay) {
        s.setValue("customerdisplay", false);
        fCustomerDisplay->deleteLater();
        fCustomerDisplay = nullptr;
    } else {
        s.setValue("customerdisplay", true);
        fCustomerDisplay = new WCustomerDisplay();

        if(qApp->screens().count() > 1) {
            fCustomerDisplay->move(qApp->primaryScreen()->geometry().x(), qApp->primaryScreen()->geometry().y());
            fCustomerDisplay->showMaximized();
            fCustomerDisplay->showFullScreen();
        } else {
            fCustomerDisplay->showFullScreen();
        }
    }
}

void Workspace::httpQueryLoading()
{
    if(!fLoadingDlg) {
        fLoadingDlg = new NLoadingDlg(this);
    }

    fLoadingDlg->resetSeconds();
    fLoadingDlg->open();
}

void Workspace::httpStop(QObject *sender)
{
    if(fLoadingDlg) {
        fLoadingDlg->hide();
    }

    sender->deleteLater();
}

void Workspace::slotHttpError(const QString &err)
{
    QVariant marks = sender()->property("marks");
    httpStop(sender());
    C5Message::error(err);

    if(marks == "init" || marks == "login") {
        mRejectEnabled = false;
        qApp->quit();
    }
}

void Workspace::loginResponse(const QJsonObject &jdoc)
{
    if(jdoc["status"].toInt() == 0) {
        C5Message::error(jdoc["data"].toString());
        return;
    }

    QJsonObject jo = jdoc["data"].toObject();
    __c5config.setRegValue("sessionkey", jo["sessionkey"].toString());
    NDataProvider::sessionKey = jo["sessionkey"].toString();
    jo = jdoc["data"].toObject()["user"].toObject();
    __c5config.setRegValue("username", QString("%1 %2").arg(jo["f_last"].toString(), jo["f_first"].toString()));
    __c5config.setRegValue("json_config_id",
                           jo["f_config"].toInt() > 0 ? jo["f_config"].toInt() : __c5config.fJsonConfigId);
    __c5config.setRegValue("session", C5Database::uuid());
    __c5config.setRegValue("cashsession", jdoc["data"].toObject()["cashsession"].toObject()["f_id"].toInt());
    fUser = new C5User(jo["f_id"].toInt());
    httpStop(sender());
    QJsonArray jsessions = jdoc["data"].toObject()["sessions"].toArray();
    createHttpRequest("/engine/smart/init.php",
    QJsonObject{{"config_id", __c5config.fSettingsName}},
    SLOT(initResponse(QJsonObject)), "init");
}

void Workspace::initResponse(const QJsonObject &jdoc)
{
    if(jdoc["status"].toInt() == 0) {
        C5Message::error(jdoc["data"].toString());
        return;
    }

    QJsonObject jo = jdoc["data"].toObject();
    QJsonArray jta = jo["printeraliases"].toArray();

    for(int i = 0; i  < jta.size(); i++) {
        QJsonObject j = jta.at(i).toObject();
        fPrinterAliases[j["f_alias"].toString()] = j["f_printer"].toString();
    }

    LogWriter::write(LogWriterLevel::verbose, "init", "part2");
    jta = jo["part2"].toArray();
    int row = 0, col = 0;
    ui->tblPart2->setRowCount(1);
    QTableWidgetItem *itemAll = new QTableWidgetItem(tr("POPULAR"));
    itemAll->setData(Qt::UserRole, -1);
    itemAll->setData(Qt::BackgroundRole, -1);
    ui->tblPart2->setItem(row, col, itemAll);
    col++;

    if(col == ui->tblPart2->columnCount()) {
        row++;
        col = 0;
    }

    for(int i = 0; i < jta.size(); i++) {
        QJsonObject j = jta.at(i).toObject();

        if(row > ui->tblPart2->rowCount() - 1) {
            ui->tblPart2->setRowCount(row + 1);
        }

        QTableWidgetItem *item = new QTableWidgetItem(j["f_name"].toString());
        item->setData(Qt::UserRole, j["f_id"].toInt());
        item->setData(Qt::BackgroundRole, j["f_color"].toInt());
        ui->tblPart2->setItem(row, col, item);
        col++;

        if(col == ui->tblPart2->columnCount()) {
            row++;
            col = 0;
        }
    }

    LogWriter::write(LogWriterLevel::verbose, "init", "menu");
    jMenu = jo["menu"].toArray();
    LogWriter::write(LogWriterLevel::verbose, "init", "package");
    jta = jo["package"].toArray();

    for(int i = 0; i < jta.size(); i++) {
        const QJsonObject &j = jta.at(i).toObject();
        DishPackageDriver::fPackageDriver.addMember(j["f_package"].toString().toInt(), j["f_dish"].toString().toInt(),
                                         j["f_name"].toString(),
                                         j["f_price"].toDouble(), j["f_qty"].toDouble(),
                                         j["f_adgcode"].toString(), j["f_store"].toString().toInt(), j["f_printer"].toString());
    }

    jta = jo["packagenames"].toArray();

    for(int i = 0; i < jta.size(); i++) {
        const QJsonObject &j = jta.at(i).toObject();
        QListWidgetItem *item = new QListWidgetItem(ui->lstCombo);
        item->setText(j["f_name"].toString());
        item->setData(Qt::UserRole, j["f_id"].toString().toInt());
        item->setData(Qt::UserRole + 1, j["f_price"].toDouble());
        item->setSizeHint(QSize(ui->lstCombo->width() - 5,
                                DishPackageDriver::fPackageDriver.itemHeight(j["f_id"].toString().toInt(),
                                        ui->lstCombo->width(), item->text())));
        ui->lstCombo->addItem(item);
        ui->lstCombo->setItemDelegate(new DishMemberDelegate());
    }

    ui->btnShowPackages->setVisible(ui->lstCombo->count() > 0);
    filter("");
    stretchTableColumns(ui->tblDishes);
    stretchTableColumns(ui->tblPart2);
    on_btnShowDishes_clicked();
    QSettings s(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);

    if(s.value("customerdisplay").toBool() && qApp->screens().count() > 1) {
        showCustomerDisplay();
    }

    LogWriter::write(LogWriterLevel::verbose, "init", "tables");
    jta = jo["tables"].toArray();

    for(int i = 0; i < jta.size(); i++) {
        const QJsonObject &j = jta.at(i).toObject();
        int c = ui->tblTables->columnCount();
        ui->tblTables->setColumnCount(c + 1);
        QTableWidgetItem *item = new QTableWidgetItem(j["f_name"].toString());
        item->setData(Qt::UserRole, j["f_id"].toInt());
        item->setData(Qt::UserRole + 5, QJsonDocument::fromJson(j["f_config"].toString().toUtf8()).object());
        ui->tblTables->setItem(0, c, item);
    }

    LogWriter::write(LogWriterLevel::verbose, "init", "opentables");
    jta = jo["opentables"].toArray();

    for(int i = 0; i < jta.size(); i++) {
        const QJsonObject &j = jta.at(i).toObject();

        for(int i = 0; i < ui->tblTables->columnCount(); i++) {
            QTableWidgetItem *item = ui->tblTables->item(0, i);

            if(item->data(Qt::UserRole).toInt() == j["f_table"].toInt()) {
                item->setData(Qt::UserRole + 1, j["f_id"].toString());
                break;
            }
        }
    }

    LogWriter::write(LogWriterLevel::verbose, "init", "before stop");
    httpStop(sender());
    LogWriter::write(LogWriterLevel::verbose, "init", "after stop");

    if(jo["welcome"].toObject()["ok"].toBool()) {
        __c5config.setRegValue("sessionid", jo["welcome"].toObject()["sessionid"].toInt());
        C5Message::info(tr("Welcome") + "<br>" + __c5config.getRegValue("username").toString());
    } else {
        C5Message::info(tr("Please close session of") + "<br>" + jo["welcome"].toObject()["who"].toString());
        qApp->quit();
        return;
    }

    if(ui->tblTables->columnCount() > 0) {
        LogWriter::write(LogWriterLevel::verbose, "init", "first table click");
        on_tblTables_itemClicked(ui->tblTables->item(0, 0));
        LogWriter::write(LogWriterLevel::verbose, "init", "first table click");
    } else {
        C5Message::info(tr("Please, configure hall"));
        qApp->exit();
    }
}

void Workspace::saveResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    QJsonObject marks = sender()->property("marks").toJsonObject();
    httpStop(sender());

    if(marks["printservice"].toBool()) {
        createHttpRequest("/engine/smart/printservice.php", QJsonObject{{"header", jo["header"].toString()}, {"mode", 3}}, SLOT(
            printServiceResponse(QJsonObject)), marks);
    }

    if(jo["state"].toInt() == ORDER_STATE_CLOSE) {
        for(int i = 0; i < ui->tblTables->columnCount(); i++) {
            if(ui->tblTables->item(0, i)->data(Qt::UserRole + 1).toString() == jo["header"].toString()) {
                ui->tblTables->item(0, i)->setData(Qt::UserRole + 1, "");
            }
        }

        resetOrder();
    }
}

void Workspace::addGoodsResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();

    if(fOrderUuid.isEmpty()) {
        fOrderUuid = jo["header"].toString();
        QTableWidgetItem *item = ui->tblTables->currentItem();

        if(item) {
            item->setData(Qt::UserRole + 1, fOrderUuid);
        }
    }

    Dish d;
    d.obodyId = jo["obodyid"].toString();
    d.menuid = jo["menuid"].toInt();
    d.id = jo["f_id"].toInt();
    d.state = DISH_STATE_OK;
    d.typeId = jo["f_part"].toInt();
    d.name = jo["f_name"].toString();
    d.printer = jo["f_print1"].toString();
    d.printer2 = jo["f_print2"].toString();
    d.price = jo["f_price"].toDouble();
    d.store = jo["f_store"].toInt();
    d.adgCode = jo["f_adgt"].toString().isEmpty() ? jo["f_adgcode"].toString() : jo["f_adgt"].toString();
    d.color = jo["f_color"].toInt();
    d.netWeight = jo["f_netweight"].toDouble();
    d.cost = jo["f_cost"].toDouble();
    d.quick = jo["f_recent"].toInt();
    d.barcode = jo["f_barcode"].toString();
    d.typeName = jo["f_groupname"].toString();
    d.specialDiscount = jo["f_specialdiscount"].toDouble() / 100;
    d.qrRequired = jo["f_qr"].toInt();
    d.f_emarks = jo["f_emarks"].toString();
    addDishToOrder(&d);
    httpStop(sender());
}

void Workspace::printServiceResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    QJsonObject jheader = jo["header"].toObject();
    QJsonObject jflags = jo["flags"].toObject();
    QJsonArray jdishes1 = jo["dishes1"].toArray();
    QJsonArray jdishes2 = jo["dishes2"].toArray();

    if(!jdishes1.isEmpty()) {
        QSet<QString> prn;

        for(int i = 0; i < jdishes1.size(); i++) {
            const QJsonObject &j = jdishes1.at(i).toObject();
            prn.insert(j["f_print1"].toString());
        }

        for(const QString &p : prn) {
            QJsonArray ja;

            for(int i = 0; i < jdishes1.size(); i++) {
                const QJsonObject &j = jdishes1.at(i).toObject();

                if(j["f_print1"].toString() == p) {
                    ja.append(j);
                }
            }

            printService(p, jheader, ja, jflags, 1);
        }
    }

    if(!jdishes2.isEmpty()) {
        QSet<QString> prn;

        for(int i = 0; i < jdishes1.size(); i++) {
            const QJsonObject &j = jdishes1.at(i).toObject();
            prn.insert(j["f_print2"].toString());
        }

        for(const QString &p : prn) {
            QJsonArray ja;

            for(int i = 0; i < jdishes1.size(); i++) {
                const QJsonObject &j = jdishes1.at(i).toObject();

                if(j["f_print2"].toString() == p) {
                    ja.append(j);
                }
            }

            printService(p, jheader, ja, jflags, 2);
        }
    }

    jo = sender()->property("marks").toJsonObject();
    jo["opentable"] = true;
    httpStop(sender());
    createHttpRequest("/engine/smart/printservice.php", QJsonObject{{"header", jheader["f_id"].toString()}, {"mode", 2}},
    SLOT(
        voidResponse(QJsonObject)), jo);
}

void Workspace::receiptResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    qDebug() << jo;
}

void Workspace::serviceValuesResponse(const QJsonObject &jdoc)
{
    httpStop(sender());
    DlgServiceValues d;
    d.config(jdoc);

    if(d.exec() == QDialog::Accepted) {
    }
}

void Workspace::voidResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    QJsonObject marks = sender()->property("marks").toJsonObject();
    httpStop(sender());

    if(marks["printreceipt"].toBool()) {
        //createHttpRequest("/engine/smart/bill.php", QJsonObject{{"header", marks["header"].toString()}}, SLOT(receiptResponse(
        //            QJsonObject)));
        printReceipt(marks["header"].toString(), false, false);
    }

    if(marks["opentable"].toBool()) {
        createHttpRequest("/engine/smart/opentable.php", QJsonObject{{"table", fTable}}, SLOT(
            openTableResponse(QJsonObject)));
    }
}

void Workspace::removeDishResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    httpStop(sender());
    createHttpRequest("/engine/smart/opentable.php", QJsonObject{{"table", fTable}}, SLOT(
        openTableResponse(QJsonObject)));
}

void Workspace::openTableResponse(const QJsonObject &jdoc)
{
    resetOrder();
    QJsonObject jo = jdoc["data"].toObject();
    fTable = jo["table"].toInt();

    if(!jo["locked"].toString().isEmpty()) {
        httpStop(sender());
        fTable = 0;
        C5Message::error(tr("Table locked by") + "<br>" + jo["locked"].toString());
        return;
    }

    QTableWidgetItem *currentItem = nullptr;

    for(int i = 0; i < ui->tblTables->columnCount(); i++) {
        if(fTable == ui->tblTables->item(0, i)->data(Qt::UserRole).toInt()) {
            currentItem = ui->tblTables->item(0, i);
            currentItem->setData(Qt::UserRole + 2, true);
            ui->tblTables->setCurrentItem(currentItem);
            break;
        }
    }

    currentItem->setData(Qt::UserRole + 1, "");
    QJsonObject jconfig = currentItem->data(Qt::UserRole + 5).toJsonObject();
    fTableMenu = jconfig["menu"].toInt();
    initMenu();

    if(!jo["empty"].toBool()) {
        QJsonObject jt = jo["header"].toObject();
        currentItem->setData(Qt::UserRole + 1, jt["f_id"].toString());
        fOrderUuid = jt["f_id"].toString();
        jt = jo["customer"].toObject();
        fCustomer = jt["f_id"].toInt();

        if(fCustomer > 0) {
            ui->lbCostumerPhone->setText(QString("%1\r\n%2\r\n%3").arg(jt["f_phone"].toString(), jt["f_contact"].toString(),
                                         jt["f_address"].toString()));
            ui->lbCostumerPhone->setVisible(true);
        }

        jt = jo["bhistory"].toObject();
        fDiscountMode = jt["f_type"].toInt();
        fDiscountCard = jt["f_card"].toInt();
        fDiscountValue = jt["f_value"].toDouble();
        jt = jo["flags"].toObject();
        ui->btnFlagTakeAway->setChecked(jt["f_3"].toInt() > 0);
        QJsonArray ja = jo["dishes"].toArray();
        int serviceItemCode = C5Config::fMainJson["service_item_code"].toInt();
        C5Config::fMainJson["service_item_code"] = 0;

        for(int i = 0; i < ja.size(); i++) {
            jt = ja.at(i).toObject();
            Dish d;
            d.obodyId = jt["f_id"].toString();
            d.id = jt["f_dish"].toInt();
            d.adgCode = jt["f_adgcode"].toString();
            d.name = jt["f_name"].toString();
            d.qty = jt["f_qty1"].toDouble();
            d.qty2 = jt["f_qty2"].toDouble();
            d.price = jt["f_price"].toDouble();
            d.modificator = jt["f_comment"].toString();
            d.printer = jt["f_print1"].toString();
            d.printer2 = jt["f_print2"].toString();
            d.store = jt["f_store"].toInt();
            d.f_emarks = jt["f_emarks"].toString();
            d.state = jt["f_state"].toInt();
            addDishToOrder(&d);
        }

        C5Config::fMainJson["service_item_code"] = serviceItemCode;
        ui->tblTables->viewport()->update();
    }

    httpStop(sender());
}

void Workspace::changeQtyResponse(const QJsonObject &jdoc)
{
    if(sender()->property("marks") == "donothing") {
        httpStop(sender());
        return;
    }

    QJsonObject jo = jdoc["data"].toObject();
    int row = jo["row"].toInt();

    if(row == 1000) {
        row = ui->tblOrder->rowCount() - 1;
    }

    Dish d = ui->tblOrder->item(row, 0)->data(Qt::UserRole).value<Dish>();
    d.qty = jo["qty"].toDouble();
    ui->tblOrder->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(d));
    ui->tblOrder->setCurrentCell(row, 0);
    updateInfo(row);
    countTotal();
    httpStop(sender());
}

void Workspace::updateDishResponse(const QJsonObject &jdoc)
{
    QJsonObject jo = jdoc["data"].toObject();
    int row = jo["row"].toInt();
    Dish d = ui->tblOrder->item(row, 0)->data(Qt::UserRole).value<Dish>();
    d.modificator = jo["comment"].toString();
    ui->tblOrder->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(d));
    ui->tblOrder->setCurrentCell(row, 0);
    updateInfo(row);
    countTotal();
    httpStop(sender());
}

void Workspace::removeOrderResponse(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
    fHttp->httpQueryFinished(sender());
    createHttpRequest("/engine/smart/opentable.php", QJsonObject{{"table", fTable}}, SLOT(
        openTableResponse(QJsonObject)));
}

void Workspace::focusTaxIn()
{
    fTimer->stop();
}

void Workspace::focusLineIn()
{
    fTimer->start(1000);
}

void Workspace::on_btnAppMenu_clicked()
{
    MenuDialog m(this, fUser);
    m.exec();

    if(fUser->property("session_close").toBool()) {
        qApp->quit();
    }
}

void Workspace::on_btnP3_clicked()
{
    setQty(3, 1, -1, "");
}

void Workspace::on_btnP4_clicked()
{
    setQty(4, 1, -1, "");
}

void Workspace::on_btnP10_clicked()
{
    setQty(10, 2, -1, "");
}

void Workspace::on_btnReprintLastCheck_clicked()
{
    if(fPreviouseUuid.isEmpty()) {
        return;
    }

    printReceipt(fPreviouseUuid, false, false);
}

void Workspace::on_leReadCode_textChanged(const QString &arg1)
{
    if(arg1 == " ") {
        ui->leReadCode->clear();

        if(ui->tblOrder->rowCount() == 0) {
            return;
        }

        double v = ui->leTotal->getDouble();

        if(Change::getReceived(v)) {
            ui->leReceived->setDouble(v);
            ui->btnCheckout->click();
        }

        return;
    }

    if(arg1 == "+") {
        ui->leReadCode->clear();
        setQty(1, 2, -1, "");
        return;
    }

    if(arg1 == "-") {
        ui->leReadCode->clear();
        setQty(1, 3, -1, "");
        return;
    }

    if(!arg1.isEmpty()) {
        if(arg1.at(0) == "." || arg1.at(0) == "․" || arg1.at(0) == ".") {
            if(arg1.length() > 2) {
                fTypeFilter = 0;
                filter(QString(arg1).remove(0, 1));
            }
        }
    }
}

void Workspace::addDishToOrder(Dish *d)
{
    int row = 0;
    row = ui->tblOrder->rowCount();
    ui->tblOrder->setRowCount(row + 1);
    ui->tblOrder->setItem(row, 0, new QTableWidgetItem(""));
    ui->tblOrder->setItem(row, 1, new QTableWidgetItem(""));
    ui->tblOrder->setItem(row, 2, new QTableWidgetItem(""));
    ui->tblOrder->setItem(row, 3, new QTableWidgetItem(""));
    Dish *dd = new Dish(d);

    switch(fDiscountMode) {
    case CARD_TYPE_SPECIAL_DISHES:
        dd->discount = dd->specialDiscount;
        break;
    }

    if(C5Config::fMainJson["service_item_code"].toInt() > 0 && ui->tblOrder->rowCount() > 2) {
        Dish d =  ui->tblOrder->item(row - 1, 0)->data(Qt::UserRole).value<Dish>();
        ui->tblOrder->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(d));
        ui->tblOrder->item(row - 1, 0)->setData(Qt::UserRole, QVariant::fromValue(*dd));
        updateInfo(row - 1);
        updateInfo(row);
    } else {
        ui->tblOrder->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(*dd));
        updateInfo(row);
    }

    ui->tblOrder->setCurrentCell(row, 0);

    if(ui->btnSetCard->isChecked() || ui->btnSetCardExternal->isChecked()) {
        ui->leCard->setDouble(ui->leTotal->getDouble());
    }

    if(d->qrRequired > 0 && d->f_emarks.isEmpty()) {
        on_btnEmarks_clicked();
    }

    if(d->f_emarks.isEmpty() == false) {
        //ui->btnFiscal->setChecked(true);
    }

    if(C5Config::serviceFactor().toDouble() > 0 && C5Config::fMainJson["service_item_code"].toInt() > 0) {
        bool servicefound = false;

        for(int i = 0; i < ui->tblOrder->rowCount(); i++) {
            auto ds = ui->tblOrder->item(i, 0)->data(Qt::UserRole).value<Dish>();

            if(ds.id == C5Config::fMainJson["service_item_code"].toInt()) {
                servicefound = true;
            }
        }

        if(!servicefound) {
            Dish d;
            d.id = C5Config::fMainJson["service_item_code"].toInt();
            createAddDishRequest(d);
            return;
        }
    }

    countTotal();
}

double Workspace::discountValue()
{
    switch(fDiscountMode) {
    case CARD_TYPE_DISCOUNT:
        return fDiscountValue;

    default:
        break;
    }

    return 0;
}

void Workspace::on_btnComment_clicked()
{
    int row = ui->tblOrder->currentRow();

    if(row < 0) {
        return;
    }

    Dish d = ui->tblOrder->item(row, 0)->data(Qt::UserRole).value<Dish>();

    if(d.qty2 > 0.001) {
        C5Message::error(tr("Not editable"));
        return;
    }

    QString comment;

    if(DlgListOfDishComments::getComment(d.name, comment)) {
        createHttpRequest("/engine/smart/update-dish.php", QJsonObject{
            {"obodyid", d.obodyId},
            {"row", row},
            {"comment", comment}
        }, SLOT(updateDishResponse(QJsonObject)));
    }
}

void Workspace::on_btnMRead_clicked()
{
    ui->tblTables->setVisible(!ui->tblTables->isVisible());
    __c5config.setRegValue("tables", ui->tblTables->isVisible());
}

void Workspace::on_btnHistoryOrder_clicked()
{
    DlgMemoryRead::sessionHistory();
}

void Workspace::on_btnSetCardExternal_clicked()
{
    ui->btnSetCash->setChecked(false);
    ui->btnSetCard->setChecked(false);
    ui->btnSetCardExternal->setChecked(true);
    ui->btnSetIdram->setChecked(false);
    ui->btnSetOther->setChecked(false);
    ui->btnFiscal->setChecked(true);
    ui->leCard->setDouble(ui->leTotal->getDouble());
}

void Workspace::on_tblTables_itemClicked(QTableWidgetItem *item)
{
    if(!item) {
        return;
    }

    if(item->data(Qt::UserRole).toInt() == fTable) {
        return;
    }

    for(int i = 0; i < ui->tblTables->columnCount(); i++) {
        auto *ti = ui->tblTables->item(0, i);

        if(ti) {
            ti->setData(Qt::UserRole + 2, false);
        }
    }

    //item->setData(Qt::UserRole + 2, true);
    createHttpRequest("/engine/smart/opentable.php", QJsonObject{{"table", item->data(Qt::UserRole).toInt()}}, SLOT(
        openTableResponse(QJsonObject)));
}

void Workspace::on_btnSaveAndPrecheck_clicked()
{
    if(ui->tblOrder->rowCount() == 0) {
        return;
    }

    C5Airlog::write(hostinfo, fUser->fullName(), 1, "", fOrderUuid, "", tr("Print precheck"),
                    QString::number(fTable),  "");
    printReceipt(fOrderUuid, false, true);
}

void Workspace::on_leCard_textChanged(const QString &arg1)
{
    if(str_float(arg1) > 0) {
        if(!ui->btnFiscal->isChecked()) {
            ui->btnFiscal->setChecked(true);
        }
    }
}

void Workspace::on_leTaxpayerId_textEdited(const QString &arg1)
{
    if(arg1.length() > 0) {
        ui->btnFiscal->setChecked(true);
    }
}

void Workspace::on_btnSetTaxpayer_clicked()
{
    bool ok = false;
    QString tp = QInputDialog::getText(this, tr("Taxpayer ID"), "", QLineEdit::Normal, "", &ok);

    if(ok) {
        ui->leTaxpayerId->setText(tp);
    }

    if(tp.length() > 0) {
        ui->btnFiscal->setChecked(true);
    }
}

void Workspace::on_btnEmarks_clicked()
{
    int row = -1;

    if(row < 0) {
        row = ui->tblOrder->currentRow();
    }

    if(row < 0) {
        return;
    }

    Dish d = ui->tblOrder->item(row, 0)->data(Qt::UserRole).value<Dish>();
    bool ok = false;
    QString qr = QInputDialog::getText(this, tr("Emark"), tr("Emark"), QLineEdit::Normal, d.f_emarks, &ok);

    if(!ok) {
        return;
    }

    if(qr.length() < 20 && qr.length() > 0) {
        C5Message::error("Invalid eMarks");
        return;
    }

    ui->btnFiscal->setChecked(true);
    QString hya("էթփձջւևրչճ-ժ"
                "քոեռտըւիօպխծշ"
                "ասդֆգհյկլ;՛"
                "զղցվբնմ,․/"
                "ԷԹՓՁՋՒևՐՉՃ-Ժ"
                "ՔՈԵՌՏԸՒԻՕՊԽծՇ"
                "ԱՍԴՖԳՀՅԿԼ;՛"
                "ԶՂՑՎԲՆՄ,․/");
    QString ru("1234567890-="
               "йцукенгшщзхъ\\"
               "фывапролджэ"
               "ячсмитьбю."
               "1234567890_+"
               "ЙЦУКЕНГШЩЗХЪ/"
               "ФЫВАПРОЛДЖЭ"
               "ЯЧСМИТЬБЮ,");
    QString num("1234567890-="
                "QWERTYUIOP[]\\"
                "ASDFGHJKL;՛"
                "ZXCVBNM,./"
                "1234567890_+"
                "QWERTYUIOP[]/"
                "ASDFGHJKL;'"
                "ZXCVBNM,./");
    QString oldcode = qr;
    QString newcode;

    for(int i = 0; i < oldcode.length(); i++) {
        if(hya.contains(oldcode.at(i))) {
            newcode += num.at(hya.indexOf(oldcode.at(i)));
        } else {
            newcode += oldcode.at(i);
        }
    }

    oldcode = newcode;
    newcode.clear();

    for(int i = 0; i < oldcode.length(); i++) {
        if(ru.contains(oldcode.at(i))) {
            newcode += num.at(ru.indexOf(oldcode.at(i)));
        } else {
            newcode += oldcode.at(i);
        }
    }

    d.f_emarks = newcode;
    ui->tblOrder->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue(d));
    updateInfo(row);
}

void Workspace::on_printOrder_clicked()
{
    if(fOrderUuid.isEmpty()) {
        return;
    }

    createHttpRequest("/engine/smart/printservice.php", QJsonObject{{"header", fOrderUuid},
        {"mode", 1}}, SLOT(
        printServiceResponse(QJsonObject)), QVariant());
}

void Workspace::on_btnFiscal_clicked(bool checked)
{
    if(checked) {
        if(ui->btnSetOther->isChecked()) {
            ui->btnFiscal->setChecked(false);
        }
    }
}

void Workspace::configFiscalButton()
{
    if(__c5config.getValue(param_tax_print_always_offer).toInt() != 0) {
        ui->btnFiscal->setEnabled(false);
        ui->btnFiscal->setChecked(true);
    }
}

void Workspace::createAddDishRequest(const Dish &dish)
{
    int row = ui->tblOrder->rowCount() + 1;

    if(C5Config::fMainJson["service_item_code"].toInt() > 0) {
        if(C5Config::fMainJson["service_item_code"].toInt() == dish.id) {
            row = 1000;
        }
    }

    QJsonObject jdish;
    jdish["header"] = fOrderUuid;
    jdish["menuid"] = dish.menuid;
    jdish["dishid"] = dish.id;
    jdish["menucode"] = C5Config::defaultMenu();
    jdish["f_emarks"] = dish.f_emarks.isEmpty() ? QJsonValue::Null : QJsonValue(dish.f_emarks);
    jdish["table"] = fTable;
    jdish["row"] =  row;
    jdish["mark"] = "addgoods";
    QJsonObject bhistory;
    bhistory["card"] = fDiscountCard;
    bhistory["type"] = fDiscountMode;
    bhistory["value"] = fDiscountValue;
    bhistory["data"] = fDiscountAmount;
    QJsonObject flags;
    flags["f1"] = fCustomer == 0 ? 0 :  1;
    flags["f2"] = 0;
    flags["f3"] = ui->btnFlagTakeAway->isChecked() ? 1 : 0;
    flags["f4"] = 0;
    flags["f5"] = 0;
    jdish["flags"] = flags;
    jdish["bhistory"] = bhistory;
    jdish["customer"] = fCustomer;
    jdish["sessionid"] = __c5config.getRegValue("sessionid").toInt();
    createHttpRequest("/engine/smart/adddish.php", jdish, SLOT(addGoodsResponse(QJsonObject)));
}

void Workspace::on_btnFlagTakeAway_clicked(bool checked)
{
    if(fOrderUuid.isEmpty()) {
        return;
    }

    createHttpRequest("/engine/smart/take-away-flag.php", QJsonObject{{"header", fOrderUuid}, {"flag", checked ? 1 : 0}},
    SLOT(voidResponse(QJsonObject)));
}

void Workspace::on_btnService_clicked()
{
    if(service()) {
        if(C5Message::question(tr("Remove service fee?")) == QDialog::Accepted) {
        } else {
            return;
        }
    }

    createHttpRequest("/engine/smart/get-service-values.php", QJsonObject(), SLOT(serviceValuesResponse(QJsonObject)));
}

void Workspace::initMenu()
{
    fDishes.clear();
    fDishesBarcode.clear();

    for(int i = 0; i < jMenu.size(); i++) {
        const QJsonObject &j = jMenu.at(i).toObject();

        if(j["f_id"].toInt() == C5Config::fMainJson["service_item_code"].toInt()
                || j["f_id"].toInt() == C5Config::fMainJson["discount_item_code"].toInt()) {
            continue;
        }

        if(j["f_menu"].toInt() != fTableMenu) {
            continue;
        }

        Dish *d = new Dish();
        d->menuid = j["menuid"].toInt();
        d->id = j["f_id"].toInt();
        d->state = DISH_STATE_OK;
        d->typeId = j["f_part"].toInt();
        d->name = j["f_name"].toString();
        d->printer = j["f_print1"].toString();
        d->printer2 = j["f_print2"].toString();
        d->price = j["f_price"].toDouble();
        d->store = j["f_store"].toInt();
        d->adgCode = j["f_adgt"].toString().isEmpty() ? j["f_adgcode"].toString() : j["f_adgt"].toString();
        d->color = j["f_color"].toInt();
        d->netWeight = j["f_netweight"].toDouble();
        d->cost = j["f_cost"].toDouble();
        d->quick = j["f_recent"].toInt();
        d->barcode = j["f_barcode"].toString();
        d->typeName = j["f_groupname"].toString();
        d->specialDiscount = j["f_specialdiscount"].toDouble() / 100;
        d->qrRequired = j["f_qr"].toString().toInt();
        fDishes.append(d);

        if(d->barcode.isEmpty() == false) {
            QStringList barcodes = d->barcode.split(",", Qt::SkipEmptyParts);

            for(const QString &b : barcodes) {
                fDishesBarcode[b] = d;
            }
        }
    }

    QTableWidgetItem *tblPart2Item = ui->tblPart2->currentItem();
    on_tblPart2_itemClicked(tblPart2Item);
}
