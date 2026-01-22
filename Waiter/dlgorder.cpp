#include "dlgorder.h"
#include "ui_dlgorder.h"
#include "c5user.h"
#include "dlgsearchinmenu.h"
#include "dlgtables.h"
#include "dlgpassword.h"
#include "dlgqty.h"
#include "dlglistofdishcomments.h"
#include "customerinfo.h"
#include "c5permissions.h"
#include "dlgreceiptlanguage.h"
#include "c5utils.h"
#include "c5translator.h"
#include "struct_workstationitem.h"
#include "dlgsplitorder.h"
#include "dlglistdishspecial.h"
#include "dlgdishremovereason.h"
#include "dlgcl.h"
#include "dlgguest.h"
#include "dlgguests.h"
#include "dlglist.h"
#include "dlgtext.h"
#include "printtaxn.h"
#include "dlgprecheckoptions.h"
#include "ninterface.h"
#include "format_date.h"
#include "goodsgroupbutton.h"
#include "waiterdishwidget.h"
#include "waitermodificatorwidget.h"
#include "waiterguestwidget.h"
#include "idram.h"
#include "c5printing.h"
#include "qdishbutton.h"
#include "c5message.h"
#include "dlgstoplistoption.h"
#include "dlgviewstoplist.h"
#include "dict_goods_types.h"
#include "dict_payment_type.h"
#include "dict_currency.h"
#include "dlgsimleoptions.h"
#include "nloadingdlg.h"
#include <QToolButton>
#include <QCloseEvent>
#include <QScreen>
#include <QScrollBar>
#include <QInputDialog>
#include <QPaintEvent>
#include <QJsonObject>
#include <QPainter>
#include <QSettings>
#include <QPointer>
#include <QFile>
#include <QThread>
#include <QPrinterInfo>

#define PART2_COL_WIDTH 150
#define PART2_ROW_HEIGHT 60
#define PART3_ROW_HEIGHT 80
#define PART4_ROW_HEIGHT 80

DlgOrder::DlgOrder(C5User *user, HallItem h, TableItem t, const QVector<GoodsGroupItem*>* groups, const QVector<DishAItem*>* dishes) :
    C5WaiterDialog(user),
    ui(new Ui::DlgOrder),
    mHall(h),
    mTable(t),
    mGroups(groups),
    mDishes(dishes)
{
    ui->setupUi(this);
    installEventFilter(this);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    if(mTable.specialConfigId > 0 && mTable.specialConfigId != mHall.configId) {
        mHall.data = mTable.data;
    }

    ui->lbStaff->setText(user->fullName());
    ui->wqtypaneldown->setVisible(false);
    fTimerCounter = 0;
    connect(&fTimer, &QTimer::timeout, this, &DlgOrder::timeout);
    fTimer.start(1000);
    ui->wpayment->setVisible(false);
    ui->wmenua->setVisible(false);
#ifndef QT_DEBUG
    //ui->btnFillIdram->setVisible(false);
#endif
    setRoomComment();
    setDiscountComment();
    setComplimentary();
    setSelfcost();
    ui->lbAmount->setText("");
    fMenuID = mUser->fConfig["default_menu"].toInt();
    fPart2Parent = 0;
    fStoplistMode = false;
    createScrollButtons();
    connect(ui->orderScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
        updateScrollButtonPositions();
    });
    connect(ui->groupsScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
        updateScrollButtonPositions();
    });
    connect(ui->dishScrollArea->verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
        updateScrollButtonPositions();
    });
    createPaymentButtons();
    configBtnNum();
    configOtherButtons();
}

DlgOrder::~DlgOrder()
{
    delete ui;
}

void DlgOrder::setOrderId(const QString &id)
{
    NInterface::query1("/engine/v2/waiter/order/open-order", mUser->mSessionKey, this,
    {{"id", id}}, [this](const QJsonObject & jdoc) {
        parseOrder(jdoc);
    });
}

void DlgOrder::disableForCheckall(bool v)
{
    ui->wqtypanelup->setEnabled(!v);
    ui->wqtypaneldown->setVisible(v);
    ui->wpaneldown->setEnabled(!v);
    ui->wmenua->setEnabled(!v);
    ui->wappmenu->setEnabled(!v);

    for(int i = 0, count = ui->vlDishes->count(); i < count; i++) {
        QLayoutItem *l = ui->vlDishes->itemAt(i);
        WaiterDishWidget *d = dynamic_cast<WaiterDishWidget*>(l->widget());

        if(d) {
            d->setCheckMode(v);
        }
    }
}

void DlgOrder::setStoplistmode()
{
    fStoplistMode = !fStoplistMode;
}

bool DlgOrder::stoplistMode()
{
    return fStoplistMode;
}

void DlgOrder::viewStoplist()
{
    DlgViewStopList v(mUser);
    v.exec();
    fHttp->createHttpQuery("/engine/waiter/stoplist.php", QJsonObject{{"action", "get"}}, SLOT(handleStopList(
                QJsonObject)));
}

void DlgOrder::accept()
{
    fHttp->createHttpQueryLambda("/engine/v2/waiter/order/unlock-table", {
        {"table", mTable.id},
        {"locksrc", hostinfo},
        {"id", mOrder.id},
        {"empty_order", mOrder.isEmpty()}
    },
    [this](const QJsonObject & jdoc) {
        Q_UNUSED(jdoc);
        C5WaiterDialog::accept();
    }, [this](const QJsonObject & jerr) {
        Q_UNUSED(jerr);
        C5WaiterDialog::accept();
    });
}

void DlgOrder::reject()
{
    accept();
}

bool DlgOrder::eventFilter(QObject *o, QEvent *e)
{
    if((e->type() == QEvent::Resize || e->type() == QEvent::Show)) {
        updateScrollButtonPositions();
    }

    if(e->type() == QEvent::KeyPress) {
        auto *k = static_cast<QKeyEvent*>(e);

        if(k->key() == Qt::Key_Return || k->key() == Qt::Key_Enter) {
            confirmStringBuffer();
            return true;
        }

        if(k->key() == Qt::Key_Backspace && !mStringBuffer.isEmpty()) {
            mStringBuffer.chop(1);
            return true;
        }

        if(!k->text().isEmpty()) {
            mStringBuffer += k->text();
            return true;
        }
    }

    return C5WaiterDialog::eventFilter(o, e);
}

void DlgOrder::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);

    if(mTable.id > 0) {
        fMenuID = mHall.defaultMenu();
        makeGroups(0, 0);
        fHttp->createHttpQueryLambda("/engine/v2/waiter/order/open-table", {
            {"table", mTable.id},
            {"locksrc", hostinfo}
        },
        [this](const QJsonObject  & jdoc) {
            parseOrder(jdoc);
        }, [this](const QJsonObject & jerr) {
            reject();
        });
    }

    ui->vlDishes->addStretch();
}

void DlgOrder::makeFavorites()
{
    makeDishes(0, true);
}

void DlgOrder::makeGroups(int parent, int dept)
{
    while(ui->glGroups->itemAt(0)) {
        ui->glGroups->itemAt(0)->widget()->deleteLater();
        ui->glGroups->removeItem(ui->glGroups->itemAt(0));
    }

    QRect scr = qApp->screens().at(mScreen < 0 ? 0 : mScreen)->geometry();
    int dcolCount = scr.width() > 1024 ? 5 : 4;
    int col = 0;
    int row = 0;
    const QVector<GoodsGroupItem*>* groups = mGroups;
    QVector<GoodsGroupItem*> filteredGroups;

    if(parent > 0) {
        auto findGroup = [&](auto&& self, GoodsGroupItem * node, int id) -> GoodsGroupItem* {
            if(!node)
                return nullptr;

            if(node->id == id)
                return node;

            for(auto* child : node->children) {
                if(auto* found = self(self, child, id))
                    return found;
            }

            return nullptr;
        };
        GoodsGroupItem* parentGroup = nullptr;

        for(auto* root : *mGroups) {
            if(auto* found = findGroup(findGroup, root, parent)) {
                parentGroup = found;
                break;
            }
        }

        if(parentGroup) {
            filteredGroups = parentGroup->children;
            groups = &filteredGroups;
        }
    }

    if(!groups) {
        return;
    }

    for(auto *group : *groups) {
        if(parent > 0 && group->parentId != parent) {
            continue;
        }

        if(dept > 0 && group->dept != dept) {
            continue;
        }

        auto *btn = new GoodsGroupButton(group->name);

        if(group->color < -1) {
            btn->setColor(group->color);
        }

        connect(btn, &GoodsGroupButton::clicked, this, [this, group]() {
            makeDishes(group->id, false);

            if(!group->children.isEmpty()) {
                mPreviouseParent.push(group->parentId);
                makeGroups(group->id, false);
            }
        });
        ui->glGroups->addWidget(btn, row, col++, 1, 1);

        if(col == dcolCount) {
            col = 0;
            row ++;
        }
    }

    ui->glGroups->setRowStretch(row + 1, 1);

    for(int i = 0; i < dcolCount; i++) {
        ui->glGroups->setColumnStretch(i, 1);
    }

    makeDishes(parent, false);
}

void DlgOrder::makeDishes(int group, int favorite)
{
    while(ui->glDishes->itemAt(0)) {
        ui->glDishes->itemAt(0)->widget()->deleteLater();
        ui->glDishes->removeItem(ui->glDishes->itemAt(0));
    }

    QRect scr = qApp->screens().at(mScreen < 0 ? 0 : mScreen)->geometry();
    int dcolCount = scr.width() > 1024 ? 3 : 2;
    int dcol = 0;
    int drow = 0;

    for(auto *g : *mDishes) {
        if(g->menuId != fMenuID) {
            continue;
        }

        if(group && g->group != group) {
            continue;
        }

        if(favorite && !g->favorite) {
            continue;
        }

        int w = (ui->wdishes->width() - 10)  / (scr.width() > 1024 ? 3 : 2);
        auto *btn = new QDishButton(g, w);
        connect(btn, &QDishButton::clicked, this, [this, btn, g]() {
            addDishToOrder(g, btn);
        });
        ui->glDishes->addWidget(btn, drow, dcol++, 1, 1);

        if(dcol == dcolCount) {
            dcol = 0;
            drow ++;
        }

        ui->glDishes->setRowStretch(drow + 1, 1);
    }
}

void DlgOrder::confirmStringBuffer()
{
    if(mStringBuffer.isEmpty()) {
        return;
    }

    QString code = mStringBuffer.trimmed();
    mStringBuffer = code;
    //Check qr exists
    fHttp->createHttpQuery("/engine/waiter/check-qr.php", QJsonObject{{"action", "checkQr"},
        {"qr", code}}, SLOT(checkQrResponse(QJsonObject)));
}

void DlgOrder::createScrollButtons()
{
    return;
    //ZIZI-PIZI SCROOL ON WIDGETS
    //GROUPS OF DISHES
    mBtnGroupsUp = new QToolButton(ui->wgroups);
    connect(mBtnGroupsUp, &QToolButton::clicked, this, [this]() {
        ui->groupsScrollArea->verticalScrollBar()->setValue(ui->groupsScrollArea->verticalScrollBar()->value() - 100);
    });
    mBtnGroupsUp->setProperty("role", "scrollbutton");
    mBtnGroupsUp->setAutoRaise(true);
    mBtnGroupsUp->setFixedSize(36, 36);
    mBtnGroupsUp->setIconSize(QSize(24, 24));
    mBtnGroupsUp->setIcon(QIcon(":/up-arrow.png"));
    mBtnGroupsUp->show();
    mBtnGroupsDown = new QToolButton(ui->wgroups);
    connect(mBtnGroupsDown, &QToolButton::clicked, this, [this]() {
        ui->groupsScrollArea->verticalScrollBar()->setValue(ui->groupsScrollArea->verticalScrollBar()->value() + 100);
    });
    mBtnGroupsDown->setProperty("role", "scrollbutton");
    mBtnGroupsDown->setAutoRaise(true);
    mBtnGroupsDown->setFixedSize(36, 36);
    mBtnGroupsDown->setIconSize(QSize(24, 24));
    mBtnGroupsDown->setIcon(QIcon(":/down-arrow.png"));
    mBtnGroupsDown->show();
    //DISHES
    mBtnDishUp = new QToolButton(ui->wdishes);
    connect(mBtnDishUp, &QToolButton::clicked, this, [this]() {
        ui->dishScrollArea->verticalScrollBar()->setValue(ui->dishScrollArea->verticalScrollBar()->value() - 300);
    });
    mBtnDishUp->setProperty("role", "scrollbutton");
    mBtnDishUp->setAutoRaise(true);
    mBtnDishUp->setFixedSize(36, 36);
    mBtnDishUp->setIconSize(QSize(24, 24));
    mBtnDishUp->setIcon(QIcon(":/up-arrow.png"));
    mBtnDishUp->show();
    mBtnDishUp->raise();
    mBtnDishDown = new QToolButton(ui->wdishes);
    connect(mBtnDishDown, &QToolButton::clicked, this, [this]() {
        ui->dishScrollArea->verticalScrollBar()->setValue(ui->dishScrollArea->verticalScrollBar()->value() + 300);
    });
    mBtnDishDown->setProperty("role", "scrollbutton");
    mBtnDishDown->setAutoRaise(true);
    mBtnDishDown->setFixedSize(36, 36);
    mBtnDishDown->setIconSize(QSize(24, 24));
    mBtnDishDown->setIcon(QIcon(":/down-arrow.png"));
    mBtnDishDown->show();
    //ORDER
    mBtnOrderUp = new QToolButton(ui->worder);
    connect(mBtnOrderUp, &QToolButton::clicked, this, [this]() {
        ui->orderScrollArea->verticalScrollBar()->setValue(ui->orderScrollArea->verticalScrollBar()->value() - 300);
    });
    mBtnOrderUp->setProperty("role", "scrollbutton");
    mBtnOrderUp->setAutoRaise(true);
    mBtnOrderUp->setFixedSize(36, 36);
    mBtnOrderUp->setIconSize(QSize(24, 24));
    mBtnOrderUp->setIcon(QIcon(":/up-arrow.png"));
    mBtnOrderUp->show();
    mBtnOrderUp->raise();
    mBtnOrderDown = new QToolButton(ui->worder);
    connect(mBtnOrderDown, &QToolButton::clicked, this, [this]() {
        ui->orderScrollArea->verticalScrollBar()->setValue(ui->orderScrollArea->verticalScrollBar()->value() + 300);
    });
    mBtnOrderDown->setProperty("role", "scrollbutton");
    mBtnOrderDown->setAutoRaise(true);
    mBtnOrderDown->setFixedSize(36, 36);
    mBtnOrderDown->setIconSize(QSize(24, 24));
    mBtnOrderDown->setIcon(QIcon(":/down-arrow.png"));
    mBtnOrderDown->show();
}

void DlgOrder::updateScrollButtonPositions()
{
    return;
    QRect r = ui->wgroups->contentsRect();
    mBtnGroupsUp->move(r.right() - mBtnGroupsUp->width(), r.top() + 2);
    mBtnGroupsUp->raise();
    mBtnGroupsDown->move(r.right() - mBtnGroupsDown->width(), r.bottom() - mBtnGroupsDown->height());
    mBtnGroupsDown->raise();
    r = ui->wdishes->contentsRect();
    mBtnDishUp->move(r.right() - mBtnDishUp->width(), r.top() + 2);
    mBtnDishUp->raise();
    mBtnDishDown->move(r.right() - mBtnDishDown->width(), r.bottom() - mBtnDishDown->height());
    mBtnDishDown->raise();
    r = ui->worder->contentsRect();
    mBtnOrderUp->move(r.right() - mBtnOrderUp->width(), r.top() + 2 + ui->wqty->height());
    mBtnOrderUp->raise();
    mBtnOrderDown->move(r.right() - mBtnOrderDown->width(), r.bottom() - mBtnOrderDown->height() - ui->wpaneldown->height());
    mBtnOrderDown->raise();
}

void DlgOrder::restoreStoplistQty(int dish, double qty)
{
    fHttp->createHttpQuery("/engine/waiter/stoplist.php",
    QJsonObject{{"action", "restoreqty"}, {"f_dish", dish}, {"f_qty", qty}},
    SLOT(restoreStoplistQtyResponse(QJsonObject)));
}

void DlgOrder::printPrecheck(const QString &currentStaff)
{
    int bs = 10;
    QFont font(qApp->font());
    font.setPointSize(bs);
    QString printerName = mUser->fConfig["receipt_printer"].toString();
    C5Printing p;
    QPrinterInfo pi = QPrinterInfo::printerInfo(printerName);
    QPrinter printer(pi);
    printer.setPageSize(QPageSize::Custom);
    printer.setFullPage(false);
    QRectF pr = printer.pageRect(QPrinter::DevicePixel);
    constexpr qreal SAFE_RIGHT_MM = 4.0;
    qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
    p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
    p.setFont(font);
    QString logoFile = qApp->applicationDirPath() + "/logo_receipt.png";

    if(QFile::exists(logoFile)) {
        p.image(logoFile, Qt::AlignHCenter);
        p.br();
    }

    switch(mOrder.state) {
    case ORDER_STATE_OPEN:
    case ORDER_STATE_CLOSE:
        p.ltext(tr("Receipt"));
        break;

    case ORDER_STATE_PREORDER:
        p.ltext(tr("Preorder"));
        break;

    default:
        p.ltext(QString::number(mOrder.state) + ": " + tr("Error in state"));
        break;
    }

    p.rtext(mOrder.receiptNumber);
    p.br();
    QJsonObject jtax = mOrder.fiscal();

    if(!jtax.isEmpty() && jtax.value("result") == 0) {
        jtax = QJsonDocument::fromJson(jtax.value("out").toString().toUtf8()).object();
        p.ltext(jtax["taxpayer"].toString(), 0);
        p.br();
        p.ltext(jtax["address"].toString(), 0);
        p.br();
        p.ltext(tr("TIN"), 0);
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
        p.ltext(tr("Date"), 0);
        p.rtext(QDateTime::fromMSecsSinceEpoch(jtax["time"].toDouble()).toString(FORMAT_DATETIME_TO_STR));
        p.br();
        p.ltext(tr("(F)"), 0);
        p.br();
    }

    p.br(1);
    p.ltext(tr("Table"), 0);
    p.rtext(QString("%1/%2").arg(mOrder.hallName, mOrder.tableName));
    p.br();
    p.ltext(tr("Staff"), 0);
    p.rtext(currentStaff);
    p.br();
    p.br(2);
    p.line(2);
    p.br(2);
    //p.setFontSize(bs - 4);
    p.ltext(tr("Name"), 0);
    p.ltext(tr("Qty"), 33);
    p.ltext(tr("Price"), 41);
    p.rtext(tr("Amount"));
    p.br();
    p.br(2);
    p.line();
    p.br(1);
    bool noservice = false, nodiscount = false, complimentary = false;

    for(int i = 0; i < mOrder.precheckDishes.size(); i++) {
        p.setFontSize(bs - 2);
        auto dish = mOrder.precheckDishes.at(i);

        if(dish.state != DISH_STATE_OK) {
            continue;
        }

        if(!dish.adgtCode().isEmpty()) {
            p.ltext(QString("%1: %2").arg(tr("Class"), dish.adgtCode()), 0);
            p.br();
        }

        QString name = dish.translated();

        if(!dish.countService()) {
            noservice = true;
            name += "* ";
        }

        if(!dish.countDiscount()) {
            nodiscount = true;
            name += "** ";
        }

        if(dish.complimentary()) {
            complimentary = true;
            name += "*** ";
        }

        p.ltext(name, 0, 35);
        p.ltext(float_str(dish.qty, 2), 33, 8);
        p.ltext(float_str(dish.price, 2), 41, 12);
        p.rtext(float_str(dish.total(mOrder.state == ORDER_STATE_PREORDER), 2));
        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }

    p.setFontSize(bs  - 2);

    if(noservice) {
        p.ltext(QString("* - %1").arg(tr("No service")).toLower());
        p.br();
    }

    if(nodiscount) {
        p.ltext(QString("** - %1").arg(tr("No discount")).toLower());
        p.br();
    }

    if(complimentary) {
        p.ltext(QString("*** - %1").arg(tr("Complimentary")).toLower());
        p.br();
    }

    p.setFontSize(bs + 2);
    p.ltext(tr("Subtotal"), 0);
    p.rtext(float_str(mOrder.subTotal(), 2));
    p.br();

    if(mOrder.serviceFactor() > 0) {
        //p.ltext(QString("%1 %2%").arg(tr("Service")).arg(mOrder.serviceFactor() * 100), 0);
        //p.rtext(float_str(mOrder.serviceAmount(), 2));
        p.ltext(QString("%1").arg(tr("Service")));
        p.rtext("+" + float_str(mOrder.serviceFactor() * 100, 2) + "%");
        p.br();
    }

    if(mOrder.discountFactor() > 0) {
        // p.ltext(QString("%1 %2%").arg(tr("Discount"), float_str(mOrder.discountFactor() * 100, 2)), 0);
        // p.rtext(float_str(mOrder.discountAmount(), 2));
        p.ltext(QString("%1").arg(tr("Discount")));
        p.rtext("-" + float_str(mOrder.discountFactor() * 100, 2) + "%");
        p.br();
    }

    if(mOrder.prepaidAmount() > 0) {
        p.ltext(tr("Prepaid amount"), 0);
        p.setFontSize(bs);
        p.rtext(float_str(mOrder.prepaidAmount() * -1, 2));
        p.br();
    }

    p.ltext(tr("Total due"));
    p.rtext(float_str(mOrder.totalDue, 2));
    p.br();
    p.br();
    auto printPaymentFunc = [this, &p](int id) {
        if(mOrder.payment(payment_fields[id]) > 0) {
            p.ltext(QCoreApplication::translate("PaymentType", payment_names[id]), 0);
            p.rtext(float_str(mOrder.payment(payment_fields[id]), 2));
            p.br();
        }
    };

    for(auto pt : payment_types) {
        printPaymentFunc(pt);
    }

    if(mOrder.amountPaid() - mOrder.totalDue > 0) {
        p.br();
        p.ltext(tr("Amount paid"), 0);
        p.rtext(float_str(mOrder.amountPaid(), 2));
        p.br();
        p.ltext(tr("Change"), 0);
        p.rtext(float_str(mOrder.amountPaid() - mOrder.totalDue, 2));
        p.br();
    }

    p.br();
    p.setFontSize(bs - 2);
    p.ltext(tr("Thank you for visit!"), 0);
    p.br();

    if(mOrder.state == ORDER_STATE_OPEN || mOrder.state == ORDER_STATE_CLOSE) {
        p.ltext(QString("%1: %2").arg(tr("Sample")).arg(mOrder.printCount()));
    }

    p.br();
    p.ltext(tr("Printed"), 0);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();
    p.print(printer);
}

void DlgOrder::printService(const QJsonObject &jdoc)
{
    QJsonObject printData = jdoc["print_data"].toObject();
    QStringList jp = printData.keys();
    QJsonObject jh = jdoc["header"].toObject();

    if(jp.isEmpty()) {
        return;
    }

    for(auto const &printerName : std::as_const(jp)) {
        QJsonObject jo = printData[printerName].toObject();
        QFont font(qApp->font());
        const int bs = 12;
        font.setPointSize(bs);
        C5Printing p;
        QPrinterInfo pi = QPrinterInfo::printerInfo(printerName);
        QPrinter printer(pi);
        printer.setPageSize(QPageSize::Custom);
        printer.setFullPage(false);
        QRectF pr = printer.pageRect(QPrinter::DevicePixel);
        constexpr qreal SAFE_RIGHT_MM = 4.0;
        qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
        p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
        p.setFont(font);
        p.setFontBold(true);

        if(jdoc["reprint"].toBool()) {
            p.ctext(tr("REPRINT"));
            p.br();
            p.br();
        }

        p.ctext(tr("New order").toUpper());
        p.br();
        p.br();
        p.setFontBold(false);
        p.ltext(tr("Table"), 0);
        p.rtext(jh["f_table_name"].toString());
        p.br();
        p.ltext(tr("Order no"), 0);
        p.rtext(jh["f_prefix"].toString());
        p.br();
        p.ltext(tr("Date"), 0);
        p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ltext(tr("Time"), 0);
        p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
        p.br();
        p.ltext(tr("Staff"), 0);
        p.rtext(mUser->shortFullName());
        p.br(p.fLineHeight + 2);
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(2);
        QSet<QString> storages;

        for(auto const &jdv : jo["dishes"].toArray()) {
            p.setFontSize(bs + 2);
            p.setFontBold(false);
            QJsonObject jd = jdv.toObject();
            p.ltext(QString("%1").arg(jd["f_dish_name"].toString()), 0, 650);
            p.setFontBold(true);
            p.rtext(QString("%1").arg(float_str(jd["f_qty"].toDouble(), 2)));

            if(jd["f_comment"].toString().length() > 0) {
                p.br();
                p.setFontSize(bs - 4);
                p.setFontBold(true);
                p.ltext(jd["f_comment"].toString(), 0, 650);
                p.br();
                p.setFontSize(bs + 2);
            }

            p.br();
            p.line(0, p.fTop, p.fNormalWidth, p.fTop);
            p.br(1);
        }

        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(1);
        p.setFontSize(bs - 6);
        p.ltext(QString("%1 %2").arg(tr("Printer: "), printerName));
        p.setFontBold(true);
        p.rtext(jo["side"].toString());
        p.br();
        QString final = "OK";

        if(!p.print(printer)) {
            final = "FAIL";
        }
    }
}

void DlgOrder::printRemovedDish(const QJsonObject &jdoc)
{
    auto printRemoved = [this, jdoc](const QString & printerName) {
        if(printerName.isEmpty()) {
            return;
        }

        int bs = 12;
        QFont font(qApp->font());
        font.setPointSize(bs);
        C5Printing p;
        QPrinterInfo pi = QPrinterInfo::printerInfo(printerName);
        QPrinter printer(pi);
        printer.setPageSize(QPageSize::Custom);
        printer.setFullPage(false);
        QRectF pr = printer.pageRect(QPrinter::DevicePixel);
        constexpr qreal SAFE_RIGHT_MM = 2.0;
        qreal safePx = SAFE_RIGHT_MM * printer.logicalDpiX() / 25.4;
        p.setSceneParams(pr.width() - safePx, pr.height(), printer.logicalDpiX());
        p.setFont(font);
        p.setFontBold(true);
        p.ctext(tr("REMOVED").toUpper());
        p.br();
        p.br();
        p.setFontBold(false);
        p.ltext(tr("Table"), 0);
        p.rtext(mOrder.tableName);
        p.br();
        p.ltext(tr("Order no"), 0);
        p.rtext(mOrder.receiptNumber);
        p.br();
        p.ltext(tr("Date"), 0);
        p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ltext(tr("Time"), 0);
        p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
        p.br();
        p.ltext(tr("Staff"), 0);
        p.rtext(mUser->fullName());
        p.br(p.fLineHeight + 2);
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(2);
        p.setFontSize(bs + 2);
        p.ltext(QString("%1").arg(jdoc["f_removed_dish_name"].toString()), 0);
        p.setFontBold(true);
        p.rtext(QString("%1").arg(float_str(jdoc["f_removed_qty"].toDouble(), 2)));
        p.setFontBold(false);

        if(jdoc["f_removed_comment"].toString().length() > 0) {
            p.br();
            p.setFontSize(bs - 4);
            p.setFontBold(true);
            p.ltext(jdoc["f_removed_comment"].toString(), 0);
            p.br();
            p.setFontSize(bs);
            p.setFontBold(false);
        }

        p.br();
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(1);
        p.line(0, p.fTop, p.fNormalWidth, p.fTop);
        p.br(1);
        p.setFontSize(bs - 6);
        p.ltext(tr("Printer: ") + printerName, 0);
        p.br();
        QString final = "OK";

        if(!p.print(printer)) {
            final = "FAIL";
        }
    };
    printRemoved(jdoc["print1"].toString());
    printRemoved(jdoc["print2"].toString());
}

void DlgOrder::setDishQty(std::function<double(WaiterDish)> getQty)
{
    if(mOrder.isPrecheckPrinted()) {
        C5Message::error(tr("Order is not editable"));
        return;
    }

    int index = selectedWaiterDishIndex();

    if(index < 0) {
        return;
    }

    WaiterDish d = mOrder.dishes.at(index);

    if(d.type != GOODS_TYPE_GOODS && d.type != GOODS_TYPE_MODIFICATOR) {
        return;
    }

    if(d.state != DISH_STATE_OK) {
        return;
    }

    if(d.isHourlyPayment()) {
        C5Message::error(tr("Cannot add comment to hourly payment"));
        return;
    }

    double qty = getQty(d);

    if(qty <= 0.01) {
        return;
    }

    if(d.isPrinted()) {
        fHttp->createHttpQueryLambda("/engine/v2/waiter/order/add-dish", {
            {"dish", d.dishId},
            {"table", mTable.id},
            {"qty", qty},
            {"type", 1},
            {"row", ((d.row / 100) + 1) * 100},
            {"price", d.price},
            {"store", d.store},
            {"print1", d.printer1()},
            {"print2", d.printer2()},
            {"shift_rows", true}
        },
        [this, d](const QJsonObject  & jdoc) {
            if(jdoc.contains("stoplist")) {
                for(auto *jg : *mDishes) {
                    if(jg->id == d.dishId) {
                        jg->stoplist = jdoc["stoplist"].toDouble();
                    }
                }
            }

            parseOrder(jdoc);
        }, [](const QJsonObject & jerr) {
        });
        return;
    }

    if(!d.emarks.isEmpty()) {
        C5Message::error(tr("The quantity of dishes with remarks cannot be changed"));
        return;
    }

    fHttp->createHttpQueryLambda("/engine/v2/waiter/order/set-dish-qty", {
        {"id", d.id},
        {"remove_emarks", false},
        {"dish", d.dishId},
        {"new_qty", qty},
        {"new_state", d.state},
        {"data", d.data},
        {"order_id", mOrder.id},
        {"restore_stoplist", -1}
    },
    [this](const QJsonObject & jdoc) {
        parseOrder(jdoc);
    }, [](const QJsonObject & jerr) {});
}

void DlgOrder::addDishToOrder(DishAItem *g, QDishButton *btn)
{
    if(g->emarkRequired) {
        C5Message::error(tr("Append only by QR code"));
        return;
    }

    if(fStoplistMode) {
        int max = 999;

        if(!DlgPassword::stopList(max)) {
            return;
        }

        fHttp->createHttpQueryLambda("/engine/v2/waiter/stoplist/set", {{"qty", max}, {"dish", g->id}},
        [g, max, btn](const QJsonObject  & jdoc) {
            Q_UNUSED(jdoc);
            g->stoplist = max;

            if(btn) {
                btn->update();
            }
        }, [](const QJsonObject & jerr) {
        });
    } else {
        fHttp->createHttpQueryLambda("/engine/v2/waiter/order/add-dish", {
            {"dish", g->id},
            {"table", mTable.id},
            {"qty", 1},
            {"type", 1},
            {"row", mOrder.dishes.count() * 100},
            {"price", g->price},
            {"count_service", g->countService()},
            {"count_discount", g->countDiscount()},
            {"store", g->store},
            {"print1", g->print1},
            {"print2", g->print2},
            {"empty_order", mOrder.dishes.empty()},
            {"service_factor", mHall.serviceFactor()}
        },
        [this, g, btn](const QJsonObject  & jdoc) {
            if(jdoc.contains("stoplist")) {
                for(auto *jg : *mDishes) {
                    if(jg->id == g->id) {
                        jg->stoplist = jdoc["stoplist"].toDouble();
                    }
                }
            } else {
                g->stoplist = -1;
            }

            if(btn) {
                btn->update();
            }

            parseOrder(jdoc);
        }, [](const QJsonObject & jerr) {
        });
    }
}

void DlgOrder::funcWithAuth(int permission, const QString &title, std::function<void (C5User*)> function)
{
    if(mUser->check(permission)) {
        function(mUser);
    } else {
        QString pin;

        if(!DlgPassword::getPasswordString(title, pin)) {
            return;
        }

        auto *user = new C5User();
        user->authorize(pin, fHttp, [user, function, permission](const QJsonObject & jdoc) {
            if(user->check(permission)) {
                function(user);
            } else {
                C5Message::error(tr("Permission denied"));
            }

            user->deleteLater();
        }, [user]() {
            user->deleteLater();
        });
    }
}

void DlgOrder::timeout()
{
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
    fTimerCounter++;
}

void DlgOrder::handleStopList(const QJsonObject & obj)
{
    //todo
    //C5TableData::instance()->setStopList(obj["stoplist"].toArray());
    for(QObject *o : ui->wdishes->children()) {
        QWidget *w = dynamic_cast<QWidget*>(o);

        if(w) {
            w->repaint();
        }
    }

    fHttp->httpQueryFinished(sender());
}

void DlgOrder::restoreStoplistQtyResponse(const QJsonObject & jdoc)
{
    //todo
    if(jdoc["ok"].toBool()) {
        //   C5TableData::instance()->mStopList[jdoc["f_dish"].toInt()] = jdoc["f_qty"].toDouble();
    } else {
        //C5TableData::instance()->mStopList.remove(jdoc["f_dish"].toInt());
    }

    fHttp->httpQueryFinished(sender());
}

void DlgOrder::addStopListResponse(const QJsonObject & jdoc)
{
    // C5TableData::instance()->mStopList[jdoc["f_dish"].toInt()] = jdoc["f_qty"].toDouble();
    fHttp->httpQueryFinished(sender());
}

void DlgOrder::qrListResponse(const QJsonObject & obj)
{
    //TODO: CHECK
    int index = -1;

    // for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //     // if(wo->fOrderDriver->dishesValue("f_id", i).toString() == obj["bodyid"].toString()) {
    //     //     index = i;
    //     //     break;
    //     // }
    // }

    if(index > -1) {
        //TODO: CHECK
        //wo->fOrderDriver->setDishesValue("f_emarks", obj["emarks"].toString(), index);
        //updateData();
    }

    fHttp->httpQueryFinished(sender());
}

void DlgOrder::on_btnExit_clicked()
{
    accept();
}

void DlgOrder::on_btnVoid_clicked()
{
    int index = selectedWaiterDishIndex();

    if(index < 0) {
        return;
    }

    WaiterDish d = mOrder.dishes.at(index);

    if(d.type != GOODS_TYPE_GOODS && d.type != GOODS_TYPE_MODIFICATOR) {
        return;
    }

    if(d.state != DISH_STATE_OK) {
        return;
    }

    if(d.isHourlyPayment()) {
        C5Message::error(tr("Cannot add comment to hourly payment"));
        return;
    }

    if(C5Message::question(tr("Do you want to remove this item")) != QDialog::Accepted) {
        return;
    }

    if(d.isPrinted()) {
        QString reason;

        if(!DlgListOfDishComments::getText(tr("Reason of remove"), "/engine/v2/waiter/menu/get-remove-reason", reason)) {
            return;
        }

        QStringList titles = {tr("Mistake"), tr("With store output"), tr("Cancel")};
        QList<int> values = {DISH_STATE_MISTAKE, DISH_STATE_VOID, 0};
        DlgSimleOptions dso(titles, values);
        int result = dso.exec();

        if(result == 0) {
            return;
        }

        QPointer<DlgOrder> self(this);
        auto removePrintedServiceFunc = [self, d, result, reason](const QString & bearer) {
            NInterface::query("/engine/v2/waiter/order/set-dish-qty", bearer, self, {
                {"id", d.id},
                {"order_id", self->mOrder.id},
                {"dish", d.dishId},
                {"new_state", result},
                {"data", d.data},
                {"new_qty", d.qty},
                {"restore_stoplist", d.qty},
                {"remove_reason", reason}
            },
            [self](const QJsonObject & jdoc) {
                self->printRemovedDish(jdoc);
                self->parseOrder(jdoc);
            }, [](const QJsonObject & jerr) {
                return false;
            });
        };

        if(mUser->check(cp_t5_waiter_remove_printed_goods)) {
            removePrintedServiceFunc(mUser->mSessionKey);
            return;
        } else {
            QString pin;

            if(!DlgPassword::getPasswordString(tr("Remove printed dish"), pin)) {
                return;
            }

            auto *user = new C5User();
            user->authorize(pin, self->fHttp, [self, removePrintedServiceFunc, user](const QJsonObject & jdoc) {
                if(user->check(cp_t5_waiter_remove_printed_goods)) {
                    removePrintedServiceFunc(user->mSessionKey);
                } else {
                    C5Message::error(tr("Permission denied"));
                }

                user->deleteLater();
            }, [user]() {
                user->deleteLater();
            });
            return;
        }
    } else {
        d.state = 0;
        fHttp->createHttpQueryLambda("/engine/v2/waiter/order/set-dish-qty", {
            {"id", d.id},
            {"remove_emarks", !d.emarks.isEmpty()},
            {"dish", d.dishId},
            {"new_qty", 0},
            {"new_state", d.state},
            {"data", d.data},
            {"order_id", mOrder.id},
            {"restore_stoplist", d.qty}
        },
        [this](const QJsonObject & jdoc) {
            parseOrder(jdoc);
        }, [](const QJsonObject & jerr) {});
    }
}

void DlgOrder::on_btnComment_clicked()
{
    int index = selectedWaiterDishIndex();

    if(index < 0) {
        return;
    }

    WaiterDish d = mOrder.dishes.at(index);

    if(d.type != GOODS_TYPE_GOODS) {
        return;
    }

    if(d.state != DISH_STATE_OK) {
        return;
    }

    if(d.isHourlyPayment()) {
        C5Message::error(tr("Adding comments to hourly payments is not allowed"));
        return;
    }

    if(d.isPrinted()) {
        C5Message::error(tr("Cannot add modifiers or comments to printed dishes"));
        return;
    }

    QStringList titles = {
        tr("Comment"),
        tr("Modificator"),
        tr("Cancel")
    };
    QList<int> values = {
        1, 2, 0
    };
    DlgSimleOptions dso(titles, values);
    int result = dso.exec();

    switch(result) {
    case 1: {
        QString comment = d.comment();
        QPointer<DlgOrder> self(this);

        if(DlgListOfDishComments::getComment(d.dishName, comment)) {
            NInterface::query("/engine/v2/waiter/order/set-dish-comment", self->mUser->mSessionKey, self, {
                {"id", d.id},
                {"comment", comment}
            },
            [self](const QJsonObject & jdoc) {
                self->parseOrder(jdoc);
            }, [](const QJsonObject & jerr) { return false;});
        }
    }
    break;

    case 2:
        break;

    default:
        return;
    }
}

void DlgOrder::on_btnChangeMenu_clicked()
{
    QPointer<DlgOrder> self(this);
    NInterface::query("/engine/v2/waiter/menu/get-menu-names", self->mUser->mSessionKey, self,
                      {},
    [self](const QJsonObject & jdoc) {
        QStringList names;
        QList<int> values;

        for(int i = 0; i < jdoc["names"].toArray().size(); i++) {
            const QJsonObject &jo = jdoc["names"].toArray().at(i).toObject();
            names.append(jo["f_name"].toString());
            values.append(jo["f_id"].toInt());
        }

        names.append(tr("Cancel"));
        values.append(0);
        QTimer::singleShot(0, [names, values, self]() {
            DlgSimleOptions d(names, values);
            int result = d.exec();

            if(result == 0) {
                return;
            }

            self->fMenuID = result;
            self->makeDishes(0, 0);
        });
    }, [](const QJsonObject & jerr) {
        return false;
    });
}

void DlgOrder::on_btnSearchInMenu_clicked()
{
    auto *d = new DlgSearchInMenu(mDishes, fMenuID, mUser);
    connect(d, SIGNAL(dish(int, QString)), this, SLOT(processMenuID(int, QString)));

    if(d->exec() == QDialog::Accepted) {
        auto *dish = d->mDish;

        if(dish) {
            addDishToOrder(dish, nullptr);
        }
    }

    delete d;
}

void DlgOrder::on_btnPackage_clicked()
{
    // int id;
    // QString name;
    // if(DlgListOfPackages::package(id, name)) {
    //     double max = 100;
    //     if(!DlgPassword::getAmount(name, max)) {
    //         return;
    //     }
    //     QList<int> lst = dbmenupackagelist->listOf(id);
    //     for(int id : lst) {
    //         wo->fOrderDriver->addDish2(id, max);
    //         DbMenuPackageList p(id);
    //         logRecord(mUser->fullName(), worder()->fOrderDriver->headerValue("f_id").toString(), "", "Dish in package",
    //                   QString("%1, %2 x %3").arg(dbdish->name(p.dish()))
    //                   .arg(p.qty())
    //                   .arg(p.price()), "");
    //     }
    // }
    // //TODO MIX FIX
    // updateData();
}

void DlgOrder::on_btnPrintService_clicked()
{
    fHttp->createHttpQueryLambda("/engine/v2/waiter/order/print-service-check", {{"header_id", mOrder.id}}, [this](const QJsonObject & jdoc) {
        parseOrder(jdoc);
        printService(jdoc);
    }, [](const QJsonObject & jerr) {});
}

void DlgOrder::on_btnSit_clicked()
{
    //TODO: CHECK
    // DlgGuests(worder()->fOrderDriver).exec();
    // itemsToTable();
}

void DlgOrder::on_btnChangeStaff_clicked()
{
    //TODO: CHECK
    // WOrder *wo = worder();
    // if(!wo) {
    //     return;
    // }
    // C5User *tmp = mUser;
    // if(!tmp->check(cp_t5_change_staff_of_table)) {
    //     if(!DlgPassword::getUserAndCheck(tr("Change staff of order"), tmp, cp_t5_change_staff_of_table)) {
    //         return;
    //     }
    // }
    // QString code;
    // if(!DlgPassword::getPassword(tr("Change staff"), code)) {
    //     return;
    // }
    // //QString code = QInputDialog::getText(this, tr("Change staff"), tr("Staff password"), QLineEdit::Password, "", &ok);
    // //    if (!ok) {
    // //        return;
    // //    }
    // code = code.replace("?", "");
    // code = code.replace(";", "");
    // code = code.replace(":", "");
    // if(code.isEmpty()) {
    //     return;
    // }
    // C5User u(code);
    // if(!u.isValid()) {
    //     C5Message::error(u.error());
    //     return;
    // }
    // wo->fOrderDriver->setHeader("f_staff", u.id());
    // ui->btnChangeStaff->setText(QString("%1\n%2").arg(tr("Staff")).arg(u.fullName()));
}

void DlgOrder::on_btnDiscount_clicked()
{
    //TODO: FIX FULL LOG
    //TODO: CHECK L4
    // C5User *tmp = mUser;
    // C5Database db;
    // if(wo->fOrderDriver->headerValue("f_discountfactor").toDouble() > 0.001) {
    //     if(!tmp->check(cp_t5_cancel_discount)) {
    //         if(!DlgPassword::getUserAndCheck(tr("Remove discount"), tmp, cp_t5_cancel_discount)) {
    //             return;
    //         }
    //     }
    //     db[":f_id"] = wo->fOrderDriver->currentOrderId();
    //     db.exec("delete from b_history where f_id=:f_id");
    //     wo->fOrderDriver->setHeader("f_discountfactor", 0);
    //     for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //         wo->fOrderDriver->setDishesValue("f_discount", wo->fOrderDriver->headerValue("f_discountfactor"), i);
    //     }
    //     wo->fOrderDriver->amountTotal();
    //     updateData();
    //     wo->fOrderDriver->save();
    //     logRecord(tmp->fullName(), wo->fOrderDriver->currentOrderId(), "", "Discount removed", "", "");
    //     return;
    // }
    // bool ok;
    // QString code = QInputDialog::getText(this, tr("Card"), tr("Card code"), QLineEdit::Password, "", &ok);
    // if(!ok) {
    //     return;
    // }
    // code = code.replace("?", "");
    // code = code.replace(";", "");
    // code = code.replace(":", "");
    // if(code.isEmpty()) {
    //     C5Message::error(tr("Card code is empty"));
    //     return;
    // }
    // discountOrder(tmp, code);
}

void DlgOrder::on_btnTotal_clicked()
{
    QString err;

    if(!mOrder.isReadyForPrecheck()) {
        C5Message::error(tr("Order not ready for precheck") + "<br>" + err);
        return;
    }

    QPointer<DlgOrder> self(this);
    auto repeatPrecheckFunc = [self](const QString & bearer, const QString & currentUserName) {
        if(!self) {
            return;
        }

        self->fHttp->query("/engine/v2/waiter/order/print-precheck",
                           bearer, self,
        {{"id", self->mOrder.id}},
        [self, currentUserName](const QJsonObject & jdoc) {
            self->parseOrder(jdoc);
            self->printPrecheck(currentUserName);
        }, [](const QJsonObject & jerr) {
            Q_UNUSED(jerr);
            return false;
        });
    };

    if(mOrder.isPrecheckPrinted()) {
        int o = DlgPrecheckOptions::precheck(mUser);

        switch(o) {
        case PRECHECK_CANCEL: {
            auto cancelPrecheckFunc = [self](const QString & bearer) {
                if(!self) {
                    return;
                }

                if(C5Message::question(QObject::tr("Confirm to cancel bill")) != QDialog::Accepted) {
                    return;
                }

                self->fHttp->query("/engine/v2/waiter/order/cancel-precheck",
                                   bearer, self,
                {{"id", self->mOrder.id}},
                [self](const QJsonObject & jdoc) {
                    self->parseOrder(jdoc);
                }, [](const QJsonObject & jerr) {
                    Q_UNUSED(jerr);
                    return false;
                });
            };

            if(mUser->check(cp_t5_waiter_cancel_precheck)) {
                cancelPrecheckFunc(mUser->mSessionKey);
            } else {
                QString pin;

                if(!DlgPassword::getPassword(tr("Cancel precheck"), pin)) {
                    return;
                }

                auto *user = new C5User();
                user->authorize(pin, fHttp, [cancelPrecheckFunc, user, self](const QJsonObject & jdoc) {
                    Q_UNUSED(jdoc);

                    if(user->check(cp_t5_waiter_cancel_precheck)) {
                        cancelPrecheckFunc(user->mSessionKey);
                    } else {
                        if(self) {
                            C5Message::error(QObject::tr("Permission denied"));
                        }
                    }

                    user->deleteLater();
                }, [user]() {
                    user->deleteLater();
                });
            }

            break;
        }

        case PRECHECK_REPEAT:
            if(mUser->check(cp_t5_waiter_reprint_prechek)) {
                repeatPrecheckFunc(mUser->mSessionKey, mUser->shortFullName());
            } else {
                QString pin;

                if(!DlgPassword::getPassword(tr("Repeat precheck"), pin)) {
                    return;
                }

                auto *user = new C5User();
                user->authorize(pin, fHttp, [repeatPrecheckFunc, user, self](const QJsonObject & jdoc) {
                    Q_UNUSED(jdoc);

                    if(user->check(cp_t5_waiter_reprint_prechek)) {
                        repeatPrecheckFunc(user->mSessionKey, user->shortFullName());
                    } else {
                        if(self) {
                            C5Message::error(QObject::tr("Permission denied"));
                        }
                    }

                    user->deleteLater();
                }, [user]() {
                    user->deleteLater();
                });
            }
        }
    } else {
        repeatPrecheckFunc(mUser->mSessionKey, mUser->shortFullName());
    }

    // if(wo->fOrderDriver->headerValue("f_precheck").toInt() < 1) {
    //     auto *tmp = mUser;
    //     if(!tmp->check(cp_t5_print_precheck)) {
    //         if(!DlgPassword::getUserAndCheck(tr("Print precheck"), tmp, cp_t5_print_precheck)) {
    //             return;
    //         }
    //     }
    //     int withoutprint = 0;
    //     if(__c5config.getValue(param_waiter_ask_for_precheck).toInt() > 0) {
    //         withoutprint = DlgAskForPrecheck::get();
    //     }
    //     bool empty = true;
    //     for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //         if(wo->fOrderDriver->dishesValue("f_state", i).toInt() != DISH_STATE_OK) {
    //             continue;
    //         }
    //         if(wo->fOrderDriver->dishesValue("f_qty1", i).toDouble() > wo->fOrderDriver->dishesValue("f_qty2", i).toDouble()
    //                 && !(wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_EMPTY
    //                      || wo->fOrderDriver->headerValue("f_state").toInt() == ORDER_STATE_PREORDER_WITH_ORDER)) {
    //             C5Message::error(tr("Order is incomplete"));
    //             return;
    //         }
    //         empty = false;
    //     }
    //     if(empty) {
    //         if(tmp != mUser) {
    //             delete tmp;
    //         }
    //         C5Message::error(tr("Order is incomplete"));
    //         return;
    //     }
    //     wo->fOrderDriver->amountTotal();
    //     wo->fOrderDriver->save();
    //     //TODO: CHECK L14
    //     fHttp->createHttpQueryLambda("/engine/waiter/printreceipt.php", {
    //         {"station", hostinfo},
    //         {"printer", C5Config::localReceiptPrinter()},
    //         {"order", worder()->fOrderDriver->currentOrderId()},
    //         {"language", C5Config::getRegValue("receipt_language").toInt()},
    //         {"receipt_printer", C5Config::fSettingsName},
    //         {"withoutprint", withoutprint},
    //         {"alias", __c5config.getValue(param_force_use_print_alias).toInt()}
    //     }, [this](const QJsonObject & jo) {
    //         auto np = new NDataProvider(this);
    //         np->overwriteHost("http", "127.0.0.1", 8080);
    //         connect(np, &NDataProvider::done, this, [](const QJsonObject & jjo) {
    //         });
    //         connect(np, &NDataProvider::error, this, [](const QString & err) {
    //         });
    //         np->getData("/printjson", jo);
    //     }, [](const QJsonObject & je) {
    //         Q_UNUSED(je);
    //     });
    //     if(tmp != mUser) {
    //         delete tmp;
    //     }
    // } else {
    //
    // }
}

void DlgOrder::on_btnReceiptLanguage_clicked()
{
    int r = DlgReceiptLanguage::receipLanguage(mUser);

    if(r > -1) {
        ui->btnReceiptLanguage->setProperty("receipt_language", r);
        setLangIcon();
    }
}
void DlgOrder::setLangIcon()
{
    switch(ui->btnReceiptLanguage->property("receipt_language").toInt()) {
    case 0:
        ui->btnReceiptLanguage->setIcon(QIcon(":/armenia.png"));
        break;

    case 1:
        ui->btnReceiptLanguage->setIcon(QIcon(":/usa.png"));
        break;

    case 2:
        ui->btnReceiptLanguage->setIcon(QIcon(":/russia.png"));
        break;

    default:
        ui->btnReceiptLanguage->setIcon(QIcon(":/armenia.png"));
        break;
    }
}
void DlgOrder::calcAmount(C5LineEdit * l)
{
    //TODO: CHECK L15
    // WOrder *wo = worder();
    // double max = wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    // if(!DlgPassword::getAmount(l->property("pay").toString(), max)) {
    //     return;
    // }
    // l->setDouble(max);
    // lineEditToHeader();
}
void DlgOrder::lineEditToHeader()
{
    //TODO: CHECK L15
    // wo->fOrderDriver->setHeader("f_amountcash", ui->leCash->getDouble());
    // wo->fOrderDriver->setHeader("f_amountcard", ui->leCard->getDouble());
    // wo->fOrderDriver->setHeader("f_amountbank", ui->leBank->getDouble());
    // wo->fOrderDriver->setHeader("f_amountprepaid", ui->lePrepaid->getDouble());
    // wo->fOrderDriver->setHeader("f_amountother", ui->leOther->getDouble());
    // wo->fOrderDriver->setHeader("f_amountidram", ui->leIDRAM->getDouble());
    // wo->fOrderDriver->setHeader("f_amountpayx", ui->lePayX->getDouble());
    // ui->leRemain->setDouble(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                         - ui->leCash->getDouble()
    //                         - ui->leCard->getDouble()
    //                         - ui->leBank->getDouble()
    //                         - ui->lePrepaid->getDouble()
    //                         - ui->leOther->getDouble()
    //                         - ui->leIDRAM->getDouble()
    //                         - ui->lePayX->getDouble());
}
void DlgOrder::headerToLineEdit()
{
    //TODO: CHECK
    // ui->leCash->setDouble(wo->fOrderDriver->headerValue("f_amountcash").toDouble());
    // ui->leCard->setDouble(wo->fOrderDriver->headerValue("f_amountcard").toDouble());
    // ui->leBank->setDouble(wo->fOrderDriver->headerValue("f_amountbank").toDouble());
    // ui->lePrepaid->setDouble(wo->fOrderDriver->headerValue("f_amountprepaid").toDouble());
    // ui->leOther->setDouble(wo->fOrderDriver->headerValue("f_amountother").toDouble() +
    //                        wo->fOrderDriver->headerValue("f_hotel").toDouble());
    // ui->leIDRAM->setDouble(wo->fOrderDriver->headerValue("f_amountidram").toDouble());
    // ui->lePayX->setDouble(wo->fOrderDriver->headerValue("f_amountpayx").toDouble());
    // if(wo->fOrderDriver->preorder("f_prepaidcash").toDouble() > 0) {
    //     if(wo->fOrderDriver->headerValue("f_amountcash").toDouble() < 1) {
    //         wo->fOrderDriver->setHeader("f_amountcash",
    //                                     wo->fOrderDriver->preorder("f_prepaidcash").toDouble() > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     ? wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     : wo->fOrderDriver->preorder("f_prepaidcash").toDouble());
    //         ui->leCash->setDouble(wo->fOrderDriver->headerValue("f_amountcash").toDouble());
    //     }
    // }
    // if(wo->fOrderDriver->preorder("f_prepaidcard").toDouble() > 0) {
    //     if(wo->fOrderDriver->headerValue("f_amountcard").toDouble() < 1) {
    //         wo->fOrderDriver->setHeader("f_amountcard",
    //                                     wo->fOrderDriver->preorder("f_prepaidcard").toDouble() > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     ? wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     : wo->fOrderDriver->preorder("f_prepaidcard").toDouble());
    //         ui->leCard->setDouble(wo->fOrderDriver->headerValue("f_amountcard").toDouble());
    //     }
    // }
    // if(wo->fOrderDriver->preorder("f_prepaidpayx").toDouble() > 0) {
    //     if(wo->fOrderDriver->headerValue("f_amountpayx").toDouble() < 1) {
    //         wo->fOrderDriver->setHeader("f_amountpayx",
    //                                     wo->fOrderDriver->preorder("f_prepaidpayx").toDouble() > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     ? wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                                     : wo->fOrderDriver->preorder("f_prepaidpayx").toDouble());
    //         ui->lePayX->setDouble(wo->fOrderDriver->headerValue("f_amountpayx").toDouble());
    //     }
    // }
    // ui->leRemain->setDouble(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()
    //                         - ui->leCash->getDouble()
    //                         - ui->leCard->getDouble()
    //                         - ui->lePrepaid->getDouble()
    //                         - ui->leBank->getDouble()
    //                         - ui->leIDRAM->getDouble()
    //                         - ui->lePayX->getDouble()
    //                         - ui->leOther->getDouble());
    // ui->leReceived->clear();
    // ui->leChange->clear();
    // if(wo->fOrderDriver->headerValue("f_cash").toDouble() > 0.01) {
    //     wo->fOrderDriver->setHeader("f_change",
    //                                 wo->fOrderDriver->headerValue("f_cash").toDouble() - wo->fOrderDriver->headerValue("f_amounttotal").toDouble());
    //     ui->leReceived->setDouble(wo->fOrderDriver->headerValue("f_cash").toDouble());
    //     ui->leChange->setDouble(wo->fOrderDriver->headerValue("f_change").toDouble());
    // }
}
void DlgOrder::clearOther()
{
    //TODO: CHECK
    // wo->fOrderDriver->setRoom("", "", "", "");
    // wo->fOrderDriver->setCL("", "");
    // ui->leRoomComment->clear();
    // ui->leRoomComment->setVisible(false);
    // ui->lbRoom->setVisible(false);
}
void DlgOrder::setCLComment()
{
    //TODO: CHECK
    // bool v = !worder()->fOrderDriver->clValue("f_code").toString().isEmpty();
    // ui->leRoomComment->setVisible(v);
    // ui->lbRoom->setVisible(v);
    // if(v) {
    //     ui->leRoomComment->setText(worder()->fOrderDriver->clValue("f_code").toString() + ", " +
    //                                worder()->fOrderDriver->clValue("f_name").toString());
    // }
}
void DlgOrder::setComplimentary()
{
    //TODO: CHECK
    // if(worder()->fOrderDriver->headerValue("f_otherid").toInt() == PAYOTHER_COMPLIMENTARY) {
    //     ui->leRoomComment->setVisible(true);
    //     ui->leRoomComment->setText(tr("Complimentary"));
    //     ui->lbRoom->setVisible(true);
    // }
}
void DlgOrder::setRoomComment()
{
    //TODO: CHECK
    // if(WOrder *wo = worder()) {
    //     bool v = !wo->fOrderDriver->payRoomValue("f_room").toString().isEmpty();
    //     ui->leRoomComment->setVisible(v);
    //     ui->lbRoom->setVisible(v);
    //     if(v) {
    //         ui->leRoomComment->setText(wo->fOrderDriver->payRoomValue("f_room").toString() + ", " +
    //                                    wo->fOrderDriver->payRoomValue("f_guest").toString());
    //     }
    // }
}
void DlgOrder::setSelfcost()
{
    //TODO: CHECK
    // if(w->fOrderDriver->headerValue("f_otherid").toInt() == PAYOTHER_PRIMECOST) {
    //     ui->leRoomComment->setVisible(true);
    //     ui->lbRoom->setVisible(true);
    //     ui->leRoomComment->setText(tr("Prime cost"));
    // }
}
void DlgOrder::setDiscountComment()
{
//TODO: CHECK
    // ui->leDiscountComment->setVisible(false);
    // ui->lbDiscount->setVisible(false);
    //TODO: DISCOUTN COMMENT
    //    bool v = (fOrder->hInt("f_bonusid") > 0);
    //    ui->leDiscountComment->setVisible(v);
    //    if (v) {
    //        ui->leDiscountComment->setText(QString("%1, N%2, %3%, %4")
    //                                       .arg(fOrder->hString("f_bonusname"))
    //                                       .arg(fOrder->hString("f_bonusid"))
    //                                       .arg(fOrder->hDouble("f_bonusvalue"))
    //                                       .arg(fOrder->hString("f_bonusholder")));
    //    }
}
void DlgOrder::discountOrder(C5User * u, const QString & code)
{
    // C5Database db;
    // db[":f_id"] = worder()->fOrderDriver->currentOrderId();
    // db.exec("select * from b_history where f_id=:f_id");
    // if(db.nextRow()) {
    //     C5Message::error(tr("Bonus system alreay exists for this order"));
    //     return;
    // }
    // db[":f_code"] = code;
    // db.exec("select c.f_id, c.f_value, c.f_mode, cn.f_name, p.f_contact "
    //         "from b_cards_discount c "
    //         "left join c_partners p on p.f_id=c.f_client "
    //         "left join b_card_types cn on cn.f_id=c.f_mode "
    //         "where f_code=:f_code");
    // if(db.nextRow()) {
    //     switch(db.getInt("f_mode")) {
    //     case CARD_TYPE_DISCOUNT:
    //         wo->fOrderDriver->setHeader("f_discountfactor", db.getDouble("f_value") / 100.0);
    //         for(int i = 0; i < wo->fOrderDriver->dishesCount(); i++) {
    //             wo->fOrderDriver->setDishesValue("f_discount", wo->fOrderDriver->headerValue("f_discountfactor"), i);
    //         }
    //         break;
    //     case CARD_TYPE_ACCUMULATIVE:
    //         break;
    //     }
    //     db[":f_type"] = db.getInt("f_mode");
    //     db[":f_value"] = db.getDouble("f_value");
    //     db[":f_card"] = db.getInt("f_id");
    //     db[":f_data"] = 0;
    //     db[":f_id"] = wo->fOrderDriver->currentOrderId();
    //     db.insert("b_history");
    //     wo->fOrderDriver->amountTotal();
    //     itemsToTable();
    //     wo->fOrderDriver->save();
    //     logRecord(u->fullName(), wo->fOrderDriver->currentOrderId(), "", "Discount", "", "");
    // } else {
    //     C5Message::error(tr("Cannot find card"));
    //     return;
    // }
}

void DlgOrder::createPaymentButtons()
{
    int row = 0, col = 0;
    QPointer<DlgOrder> self(this);

    for(auto pt : std::as_const(payment_types)) {
        auto *btn = new QToolButton(this);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setMinimumHeight(50);
        btn->setText(QCoreApplication::translate("PaymentType", payment_names[pt]));
        btn->setEnabled(payment_special[pt] ? mUser->check(cp_t5_waiter_special_payment_types) : true);
        connect(btn, &QToolButton::clicked, this, [self, pt]() {
            if(self->ui->lbAmount->property("amount").toDouble() < 0.01) {
                return;
            }

            NInterface::query("/engine/v2/waiter/order/set_amount", self->mUser->mSessionKey, self, {
                {"id", self->mOrder.id},
                {"payment_field", payment_fields[pt]},
                {"amount", self->ui->lbAmount->property("amount").toDouble()}
            },
            [self](const QJsonObject & jdoc) {
                if(!self) {
                    return;
                }

                self->on_btnNumClear_clicked();
                self->parseOrder(jdoc);
            }, [](const QJsonObject & jerr) {return false;});
        });
        ui->glPaymentType->addWidget(btn, row, col);
        col++;

        if(col == 2) {
            col = 0;
            row++;
        }
    }

    ui->glPaymentType->setRowStretch(row + 1, 1);
}

void DlgOrder::configBtnNum()
{
    QVector<QToolButton*> buttons = {
        ui->btnNum0,
        ui->btnNum1,
        ui->btnNum2,
        ui->btnNum3,
        ui->btnNum4,
        ui->btnNum5,
        ui->btnNum6,
        ui->btnNum7,
        ui->btnNum8,
        ui->btnNum9,
        ui->btnNumDot
    };

    for(auto *b : buttons) {
        connect(b, &QToolButton::clicked, this, [ = ]() {
            QString strValue = ui->lbAmount->property("str").toString();
            QString t = b->property("num").toString();

            if(t == "." && strValue.contains(".")) {
                return;
            }

            strValue += t;
            double value = strValue.toDouble();

            if(value > 999999999) {
                value = 999999999;
            }

            ui->lbAmount->setProperty("amount", value);
            ui->lbAmount->setProperty("str", strValue);
            ui->lbAmount->setText(QString("%1 %2").arg(float_str(value, 2), CURRENCY_SHORT));
        });
    }
}

void DlgOrder::configOtherButtons()
{
    ui->btnTotal->setEnabled(mUser->check(cp_t5_waiter_print_precheck));
    ui->btnCloseOrder->setEnabled(mUser->check(cp_t5_waiter_close_order));
}

int DlgOrder::selectedWaiterDishIndex()
{
    WaiterDish wd;
    int index = -1;

    for(int i = 0; i < ui->vlDishes->count(); ++i) {
        QLayoutItem *item = ui->vlDishes->itemAt(i);

        if(!item) {
            continue;
        }

        QWidget *widget = item->widget();

        if(!widget) {
            continue;
        }

        if(auto *w = qobject_cast<WaiterDishWidget*>(widget)) {
            wd = w->mOrderItem;

            if(w->isFocused()) {
                index = i;
                break;
            }
        }
    }

    return index;
}
WaiterOrderItemWidget* DlgOrder::createOrderItemWidget(WaiterDish d)
{
    switch(d.type) {
    case GOODS_TYPE_GOODS:
        return new WaiterDishWidget(d, mShowRemoved);

    default:
        return new WaiterDishWidget(d, mShowRemoved);
    }
}

void DlgOrder::parseOrder(const QJsonObject & jdoc)
{
    QString selectedId;
    mOrder = JsonParser<WaiterOrder>::fromJson(jdoc["order"].toObject());
    ui->lbTableName->setText(mOrder.tableName);
    ui->lbOrderComment->setVisible(!mOrder.comment().isEmpty());
    ui->lbOrderComment->setText(mOrder.comment());
    ui->wgroups->setVisible(!mOrder.isPrecheckPrinted());
    ui->wdishes->setVisible(!mOrder.isPrecheckPrinted());

    for(int i = 0 ; i <  mOrder.dishes.size(); i++) {
        WaiterOrderItemWidget *ow = nullptr;
        WaiterDish w = mOrder.dishes.at(i);

        //2 - becouse we created streach in the showEvent
        if(i > ui->vlDishes->count() - 2) {
            ow = createOrderItemWidget(w);
            ui->vlDishes->insertWidget(i, ow);
        } else {
            auto *layout = ui->vlDishes->itemAt(i);

            if(auto *w = qobject_cast<WaiterOrderItemWidget*>(layout->widget())) {
                if(w->isFocused()) {
                    selectedId = w->mOrderItem.id;
                }
            }

            switch(w.type) {
            case GOODS_TYPE_GOODS:
                ow = qobject_cast<WaiterDishWidget*>(layout->widget());
                qobject_cast<WaiterDishWidget*>(ow)->mShowRemoved = mShowRemoved;
                break;

            case GOODS_TYPE_MODIFICATOR:
                ow = qobject_cast<WaiterModificatorWidget*>(layout->widget());
                break;

            case GOODS_TYPE_GUEST:
                ow = qobject_cast<WaiterGuestWidget*>(layout->widget());
                break;
            }

            if(!ow) {
                layout->widget()->deleteLater();
                delete layout;
                ow = createOrderItemWidget(w);
                ui->vlDishes->insertWidget(i, ow);
            } else {
            }
        }

        ow->updateDish(w);
        ow->setFocused(ow->mOrderItem.id == selectedId);
        connect(ow, &WaiterDishWidget::focused,
                this, &DlgOrder::handleOrderDishClick,
                Qt::UniqueConnection);
        connect(this, &DlgOrder::orderDishClicked,
                ow, &WaiterDishWidget::checkFocus,
                Qt::UniqueConnection);
    }

    /* PAYMENT BUTTON */
    if(mOrder.state == ORDER_STATE_OPEN) {
        ui->lbAmountPaid->setText(QString("%1 %2").arg(float_str(mOrder.amountPaid(), 2), CURRENCY_SHORT));
        ui->lbChange->setText(QString("%1 %2").arg(float_str(mOrder.amountChange(), 2), CURRENCY_SHORT));

        while(auto *l = ui->vlPayment->takeAt(0)) {
            auto *w = l->widget();

            if(w) {
                w->deleteLater();
            }

            delete l;
        }

        QPointer<DlgOrder> self(this);

        for(auto pt : payment_types) {
            if(mOrder.payment(payment_fields[pt]) > 0) {
                auto *b = new QToolButton(self);
                b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                b->setMinimumHeight(50);
                b->setText(QString("%1 %2 %3").arg(QCoreApplication::translate("PaymentType", payment_names[pt]), float_str(mOrder.payment(payment_fields[pt]), 2), CURRENCY_SHORT));
                b->setProperty("method", pt);
                b->setProperty("amount", mOrder.payment(payment_fields[pt]));
                connect(b, &QToolButton::clicked, self, [ = ]() {
                    NInterface::query("/engine/v2/waiter/order/set_amount", self->mUser->mSessionKey, self,
                    {{"id", self->mOrder.id}, {"payment_field", payment_fields[pt]}, {"amount", 0}},
                    [self](const QJsonObject & jdoc) {
                        if(!self) {
                            return;
                        }

                        self->parseOrder(jdoc);
                    }, [](const QJsonObject & jerr) {return false;});
                    b->deleteLater();
                });
                ui->vlPayment->addWidget(b);
            }
        }
    }

    if(jdoc.contains("restore_stoplist_qty")) {
        QVector<DishAItem*> stoplistitems;
        std::copy_if(mDishes->begin(), mDishes->end(), std::back_inserter(stoplistitems), [jdoc](DishAItem * d) {return d && d->id == jdoc["restore_stoplist_dish"].toInt();});

        for(auto *it : stoplistitems) {
            it->stoplist = jdoc["restore_stoplist_qty"].toDouble();
        }

        for(int i = 0; i < ui->glDishes->count(); i++) {
            auto *l = ui->glDishes->itemAt(i);

            if(auto *w = qobject_cast<QDishButton*>(l->widget())) {
                w->update();
            }
        }
    }

    ui->wclosedorder->setVisible(mOrder.state == ORDER_STATE_CLOSE);
    ui->wpayment->setVisible(mOrder.isPrecheckPrinted() && mOrder.state < ORDER_STATE_CLOSE);
    ui->wmenua->setVisible(!mOrder.isPrecheckPrinted() && mOrder.state < ORDER_STATE_CLOSE);
    ui->wappmenu->setEnabled(!mOrder.isPrecheckPrinted());
    ui->wqty->setEnabled(!mOrder.isPrecheckPrinted() && mOrder.state < ORDER_STATE_CLOSE);
    ui->btnService->setEnabled(!mOrder.isPrecheckPrinted() && mOrder.state < ORDER_STATE_CLOSE);
    ui->btnDiscount->setEnabled(!mOrder.isPrecheckPrinted() && mOrder.state < ORDER_STATE_CLOSE);
    ui->btnService->setText(QString("%1\n%2%").arg(tr("Service"), float_str(mOrder.serviceFactor() * 100, 2)));
    ui->btnDiscount->setText(QString("%1\n%2%").arg(tr("Discount"), float_str(mOrder.discountFactor() * 100, 2)));
    ui->btnTotal->setText(QString("%1\n%2 %3").arg(tr("Precheck"), float_str(mOrder.totalDue, 2), "դր․"));
    updateScrollButtonPositions();

    if(jdoc.contains("focused_dish")) {
        emit orderDishClicked(jdoc["focused_dish"].toString());
    }

    ui->lbSubtotal->setText(QString("%1 %2").arg(float_str(mOrder.subTotal(), 2), CURRENCY_SHORT));
    ui->lbServiceFeeName->setText(QString("%1 %2%").arg(tr("Service fee"), float_str(mOrder.serviceFactor() * 100, 2)));
    ui->lbServiceFee->setText(QString("%1 %2").arg(float_str(mOrder.serviceAmount(), 2), CURRENCY_SHORT));
    ui->lbDiscountFeeName->setText(QString("%1 %2%").arg(tr("Discount fee"), float_str(mOrder.discountFactor() * 100, 2)));
    ui->lbDiscount->setText(QString("%1 %2").arg(float_str(mOrder.discountAmount(), 2), CURRENCY_SHORT));
    ui->lbTotalDue->setText(QString("%1 %2").arg(float_str(mOrder.totalDue, 2), CURRENCY_SHORT));
    QDateTime startQuery = QDateTime::fromString(jdoc["query_start"].toString(), "yyyy-MM-dd HH:mm:ss.zzz");
    QJsonObject jtax = mOrder.fiscal();

    if(mOrder.state == ORDER_STATE_CLOSE) {
        ui->btnPrintClosedFiscal->setEnabled(jtax.isEmpty());
    }

    qDebug() << "Parse order" << startQuery.msecsTo(QDateTime::currentDateTime());
}

void DlgOrder::handleOrderDishClick(const QString & id)
{
    emit(orderDishClicked(id));
}

void DlgOrder::setPaymentButtonChecked(bool checked)
{
    auto *btn = qobject_cast<QToolButton*>(sender());

    for(int i = 0; i < ui->lPaymentButtons->count(); i++) {
        auto *l = ui->lPaymentButtons->itemAt(i);
        auto *b = qobject_cast<QToolButton*>(l->widget());

        if(b) {
            if(b == btn) {
                b->setChecked(checked);
            } else {
                b->setChecked(false);
            }
        }
    }
}

void DlgOrder::on_btnReceipt_clicked()
{
    // C5User *tmp = mUser;
    // if(!tmp->check(cp_t5_print_receipt)) {
    //     if(!DlgPassword::getUserAndCheck(tr("Print receipt"), tmp, cp_t5_print_receipt)) {
    //         return;
    //     }
    // }
    // if(C5Message::question(tr("Confirm to close order")) != QDialog::Accepted) {
    //     return;
    // }
    // if(wo->fOrderDriver->headerValue("f_amountother").toDouble() > 0.001) {
    //     if(wo->fOrderDriver->headerValue("f_otherid").toInt() == 0) {
    //         C5Message::error(tr("Other method is not selected"));
    //         return;
    //     }
    // }
    // if(wo->fOrderDriver->headerValue("f_amountcash").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountcard").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountother").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountprepaid").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountbank").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountidram").toDouble()
    //         + wo->fOrderDriver->headerValue("f_hotel").toDouble()
    //         + wo->fOrderDriver->headerValue("f_amountpayx").toDouble() <
    //         wo->fOrderDriver->headerValue("f_amounttotal").toDouble()) {
    //     C5Message::error(tr("Check the all payment methods"));
    //     return;
    // }
    // double a = wo->fOrderDriver->headerValue("f_amountcash").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountcard").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountother").toDouble()
    //            + wo->fOrderDriver->headerValue("f_hotel").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountprepaid").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountbank").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountidram").toDouble()
    //            + wo->fOrderDriver->headerValue("f_amountpayx").toDouble();
    // if(a > wo->fOrderDriver->headerValue("f_amounttotal").toDouble()) {
    //     C5Message::error(QString("%1, %2/%3").arg(tr("Total amount of payments methods greater than total amount"))
    //                      .arg(a)
    //                      .arg(wo->fOrderDriver->headerValue("f_amounttotal").toDouble()));
    //     return;
    // }
    // if(!wo->fOrderDriver->save()) {
    //     C5Message::error(wo->fOrderDriver->error());
    //     return;
    // }
    // ui->btnExit->setEnabled(false);
    // //LOG
    // QString payMethods;
    // QString amounts;
    // if(wo->fOrderDriver->headerValue("f_amountcash").toDouble() > 0.001) {
    //     payMethods += payMethods.isEmpty() ? "Cash" : payMethods + " / Cash";
    //     amounts += amounts.isEmpty() ? float_str(wo->fOrderDriver->headerValue("f_amountcash").toDouble(),
    //                2) : amounts + " / " + float_str(wo->fOrderDriver->headerValue("f_amountcash").toDouble(), 2);
    // }
    // if(wo->fOrderDriver->headerValue("f_amountcard").toDouble() > 0.001) {
    //     payMethods += payMethods.isEmpty() ? "Cash" : payMethods + " / Card";
    //     //amounts += amounts.isEmpty() ? fOrder->hString("f_amountcard") : amounts + " / " + fOrder->hString("f_amountcard");
    //     amounts += amounts.isEmpty() ? float_str(wo->fOrderDriver->headerValue("f_amountcard").toDouble(),
    //                2) : amounts + " / " + float_str(wo->fOrderDriver->headerValue("f_amountcard").toDouble(), 2);
    // }
    // if(wo->fOrderDriver->headerValue("f_amountbank").toDouble() > 0.001) {
    //     payMethods = "Bank";
    //     amounts = float_str(wo->fOrderDriver->headerValue("f_amountbank").toDouble(), 2);
    // }
    // if(wo->fOrderDriver->headerValue("f_amountother").toDouble() > 0.001) {
    //     switch(wo->fOrderDriver->headerValue("f_otherid").toInt()) {
    //     case PAYOTHER_TRANSFER_TO_ROOM:
    //         payMethods = wo->fOrderDriver->payRoomValue("f_room").toString() + ","
    //                      + wo->fOrderDriver->payRoomValue("f_inv").toString() + ","
    //                      + wo->fOrderDriver->payRoomValue("f_guest").toString();
    //         break;
    //     case PAYOTHER_CL:
    //         payMethods = wo->fOrderDriver->clValue("f_code").toString() + "," + wo->fOrderDriver->clValue("f_name").toString();
    //         break;
    //     case PAYOTHER_COMPLIMENTARY:
    //         payMethods = "Complimentary";
    //         break;
    //     case PAYOTHER_PRIMECOST:
    //         payMethods = "Selfcost";
    //         break;
    //     default:
    //         payMethods = QString("%1 (%2)").arg("Unknown").arg(wo->fOrderDriver->headerValue("f_otherid").toInt());
    //         break;
    //     }
    //     amounts = float_str(wo->fOrderDriver->headerValue("f_amountother").toDouble(), 2);
    // }
    // amounts += ui->btnTax->isChecked() ? " Fiscal: yes" : " Fiscal: no";
    // C5LogToServerThread::remember(LOG_WAITER, tmp->fullName(), "", wo->fOrderDriver->currentOrderId(), "", "Receipt",
    //                               payMethods, amounts);
    // C5Database db;
    // db[":f_id"] = wo->fOrderDriver->headerValue("f_id");
    // db.exec("select f_type, f_value, f_card from b_history where f_id=:f_id");
    // if(db.nextRow()) {
    //     int mode = db.getInt("f_mode");
    //     int card = db.getInt("f_card");
    //     double value = db.getDouble("f_value") / 100;
    //     switch(mode) {
    //     case CARD_TYPE_ACCUMULATIVE:
    //         value = value * wo->fOrderDriver->headerValue("f_amounttotal").toDouble();
    //         db[":f_card"] = card;
    //         db[":f_trsale"] = wo->fOrderDriver->headerValue("f_id");
    //         db[":f_amount"] = value;
    //         db.insert("b_gift_card_history", false);
    //         db[":f_data"] = value;
    //         db.update("b_history", "f_id", wo->fOrderDriver->headerValue("f_id"));
    //     }
    // }
    // //TODO: CHECK L16
    // fHttp->createHttpQueryLambda("/engine/waiter/printreceipt", {
    //     { "station", hostinfo},
    //     {"printer", C5Config::localReceiptPrinter()},
    //     {"order", wo->fOrderDriver->currentOrderId()},
    //     {"language", C5Config::getRegValue("receipt_language").toInt()},
    //     {"printtax", ui->btnTax->isChecked() ? 1 : 0},
    //     {"receipt_printer", C5Config::fSettingsName},
    //     {"alias", __c5config.getValue(param_force_use_print_alias).toInt()},
    //     {"close", 1},
    //     {"nofinalreceipt", __c5config.getValue(param_waiter_dontprint_final_receipt).toInt()}
    // },
    // [this](const QJsonObject & jo) {
    //     ui->btnExit->setEnabled(true);
    //     fHttp->httpQueryFinished(sender());
    //     if(jo["reply"].toInt() == 0) {
    //         C5Message::error(jo["msg"].toString());
    //         return;
    //     }
    //     auto *wo = worder();
    //     if(jo["close"].toInt() == 1) {
    //         C5LogToServerThread::remember(LOG_WAITER, mUser->fullName(), "", wo->fOrderDriver->currentOrderId(), "", "Close order",
    //                                       "", "");
    //         if(!wo->fOrderDriver->closeOrder()) {
    //             C5Message::error(wo->fOrderDriver->error());
    //             return;
    //         }
    //         removeWOrder(wo);
    //         if(!worder()) {
    //             setProperty("reprint", jo["order"].toString());
    //             accept();
    //             return;
    //         }
    //     } else {
    //         if(__c5config.getValue(param_waiter_close_order_after_precheck).toInt() == 1) {
    //             ui->btnExit->click();
    //             return;
    //         }
    //         //TODO FUCK
    //         //load(wo->fOrderDriver->headerValue("f_table").toInt());
    //     }
    // }, [](const QJsonObject & je) {
    //     Q_UNUSED(je);
    // });
}

void DlgOrder::on_btnCloseOrder_clicked()
{
    if(!mOrder.paymentCompleted()) {
        C5Message::error(tr("Payment was not completed"));
        return;
    }

    bool hasMixed = false;
    bool hasNoMixed = false;
    int paymentsCount = 0;

    for(auto pt : payment_types) {
        if(mOrder.payment(payment_fields[pt]) > 0.01) {
            paymentsCount++;

            if(payment_mix[pt]) {
                hasMixed = true;
            } else {
                hasNoMixed = true;
            }
        }
    }

    if(paymentsCount > 1 && hasMixed && hasNoMixed) {
        C5Message::error(tr("Combining payment types is not allowed."));
        return;
    }

    QPointer<DlgOrder> self(this);
    auto closeOrderFunc = [self](const QJsonObject & fiscalInfo) {
        if(!self) {
            return;
        }

        NInterface::query("/engine/v2/waiter/order/close-order", self->mUser->mSessionKey, self, {
            {"id", self->mOrder.id},
            {"fiscal", fiscalInfo},
            {"cashbox_id", mWorkStation.cashboxId()}
        },
        [self](const QJsonObject & jdoc) {
            self->parseOrder(jdoc);
            self->printPrecheck(self->mUser->shortFullName());
            self->accept();
        },
        [](const QJsonObject & jerr) {
            return false;
        });
    };

    if(ui->btnPrintFiscal->isChecked()) {
        auto *loading = new NLoadingDlg(tr("Printing fiscal check"), this);
        auto *thread  = new QThread();
        auto *pt = new PrintTaxN(self->mUser->fConfig["tax_ip"].toString(),
                                 self->mUser->fConfig["tax_port"].toString().toInt(),
                                 self->mUser->fConfig["tax_password"].toString(),
                                 self->mUser->fConfig["tax_external_pos"].toString(),
                                 self->mUser->fConfig["tax_op"].toString(),
                                 self->mUser->fConfig["tax_pin"].toString());
        pt->moveToThread(thread);
        auto order = self->mOrder;
        connect(thread, &QThread::started, pt, [ = ]() {
            for(auto d : std::as_const(order.dishes)) {
                if(d.state != DISH_STATE_OK) {
                    continue;
                }

                pt->addGoods(d.fiscalDepartment(),
                             d.adgtCode(),
                             QString::number(d.dishId),
                             d.dishName,
                             d.price,
                             d.qty,
                             d.discountFactor() * 100);
            }

            pt->makeJsonAndPrint(order.paidCash(), order.paidCard(), order.paidPrepaid());
        });
        connect(pt, &PrintTaxN::started, loading, &QDialog::show, Qt::QueuedConnection);
        connect(pt, &PrintTaxN::finished, self, [ = ](const QString & inJson, const QString & outJson, const QString & err, int result) {
            loading->close();
            loading->deleteLater();
            QJsonObject reply{{"in", inJson},
                {"out", outJson},
                {"error", err},
                {"result", result}};

            if(result == 0) {
                closeOrderFunc(reply);
            } else {
                reply.insert("id", self->mOrder.id);
                NInterface::query("/engine/v2/waiter/order/fiscal-log", mUser->mSessionKey, self, reply,
                [](const QJsonObject & jdoc) {},
                [](const QJsonObject & jerr) {return false;});
                C5Message::error(err);
            }

            thread->quit();
        });
        connect(thread, &QThread::finished, pt, &QObject::deleteLater);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);
        thread->start();
    } else {
        closeOrderFunc({});
    }
}

void DlgOrder::on_btnService_clicked()
{
    QPointer<DlgOrder> self(this);
    auto func = [self](C5User * user) {
        NInterface::query1("/engine/v2/waiter/order/get-service-values", user->mSessionKey, self, {},
        [self, user](const QJsonObject & jdoc) {
            QJsonArray jvals = jdoc["values"].toArray();
            QStringList titles;
            QList<double> values;
            QList<int> indexes;

            for(int i = 0; i < jvals.size(); i++) {
                const QJsonObject &jo = jvals.at(i).toObject();
                titles.append(jo["f_name"].toString());
                values.append(jo["f_value"].toDouble());
                indexes.append(i + 1);
            }

            titles.append(tr("Cancel"));
            indexes.append(0);
            DlgSimleOptions dso(titles, indexes);
            int index = dso.exec();

            if(index == 0) {
                return;
            }

            double newServiceFactor = values.at(index - 1);
            NInterface::query1("/engine/v2/waiter/order/change-service-value", user->mSessionKey, self, {
                {"id", self->mOrder.id},
                {"value", newServiceFactor}
            }, [self](const QJsonObject & jdoc1) {
                if(!self) {
                    return;
                }

                self->parseOrder(jdoc1);
            });
        });
    };

    if(mUser->check(cp_t5_waiter_change_service_factor)) {
        func(mUser);
        return;
    }

    auto* user = new C5User();
    QString pin;

    if(!DlgPassword::getPasswordString(tr("Remove printed dish"), pin)) {
        return;
    }

    user->authorize(pin, self->fHttp, [self, func, user](const QJsonObject & jdoc) {
        if(user->check(cp_t5_waiter_change_service_factor)) {
            func(user);
        } else {
            C5Message::error(tr("Permission denied"));
        }

        user->deleteLater();
    }, [user]() {
        user->deleteLater();
    });
    return;
}

void DlgOrder::on_btnStopListMode_clicked()
{
    auto func = [this]() {
        DlgStopListOption d(this, mUser);
        d.exec();
    };

    if(mUser->check(cp_t5_waiter_edit_stoplist)) {
        func();
        return;
    }

    QString password;

    if(!DlgPassword::getPassword(tr("Raise permissions"), password)) {
        return;
    }

    C5User *u = new C5User();
    u->authorize(password, fHttp, [ u, func](const QJsonObject & jdoc) {
        if(u->check(cp_t5_waiter_edit_stoplist)) {
            func();
        }

        u->deleteLater();
    }, [u]() {
        u->deleteLater();
    });
}

void DlgOrder::on_btnOrderComment_clicked()
{
    QString comment = mOrder.comment();

    if(!DlgText::getText(mUser, tr("Order comment"), comment)) {
        return;
    }

    NInterface::query1("/engine/v2/waiter/order/set-header-comment", mUser->mSessionKey, this, {
        {"id", mOrder.id},
        {"comment", comment}
    }, [this](const QJsonObject & jdoc) {
        parseOrder(jdoc);
    });
}

void DlgOrder::on_btnPlus1_clicked()
{
    setDishQty([](WaiterDish d) {return d.isPrinted() ? 1 : d.qty + 1;});
}

void DlgOrder::on_btnMinus1_clicked()
{
    int index = selectedWaiterDishIndex();

    if(index < 0) {
        return;
    }

    WaiterDish d = mOrder.dishes.at(index);

    if(d.type != GOODS_TYPE_GOODS && d.type != GOODS_TYPE_MODIFICATOR) {
        return;
    }

    if(d.state != DISH_STATE_OK) {
        return;
    }

    if(d.isHourlyPayment()) {
        C5Message::error(tr("Cannot add comment to hourly payment"));
        return;
    }

    if(d.isPrinted()) {
        C5Message::error(tr("Use removal tool"));
        return;
    } else {
        double newQty = 0;

        if(d.qty - 1 > 0.001) {
            newQty = d.qty - 1;
        } else if(C5Message::question(tr("Do you want to remove this item")) == QDialog::Accepted) {
            d.state = DISH_STATE_NONE;
            newQty = 0;
        }

        fHttp->createHttpQueryLambda("/engine/v2/waiter/order/set-dish-qty", {
            {"id", d.id},
            {"remove_emarks", !d.emarks.isEmpty()},
            {"dish", d.dishId},
            {"new_qty", newQty},
            {"new_state", d.state},
            {"data", d.data},
            {"order_id", mOrder.id},
            {"restore_stoplist", d.qty - newQty}
        },
        [this](const QJsonObject & jdoc) {
            parseOrder(jdoc);
        }, [](const QJsonObject & jerr) {});
    }
}

void DlgOrder::on_btnAnyqty_clicked()
{
    setDishQty([this](WaiterDish d) {
        double newQty = 999;

        if(!DlgQty::getQty(newQty, d.dishName, mUser)) {
            return 0.0;
        }

        return newQty;
    });
}

void DlgOrder::on_btnCloseCheckAll_clicked()
{
    disableForCheckall(false);
}
void DlgOrder::on_btnCheckAll_clicked()
{
    for(int i = 0, count = ui->vlDishes->count(); i < count; i++) {
        QLayoutItem *l = ui->vlDishes->itemAt(i);
        WaiterDishWidget *d = dynamic_cast<WaiterDishWidget*>(l->widget());

        if(d) {
            d->setChecked(true);
        }
    }
}

void DlgOrder::on_btnUncheckAll_clicked()
{
    for(int i = 0, count = ui->vlDishes->count(); i < count; i++) {
        QLayoutItem *l = ui->vlDishes->itemAt(i);
        WaiterDishWidget *d = dynamic_cast<WaiterDishWidget*>(l->widget());

        if(d) {
            d->setChecked(false);
        }
    }
}

void DlgOrder::on_btnReprintSelected_clicked()
{
    QPointer<DlgOrder> self(this);
    auto func = [self](C5User * user) {
        if(!self) {
            return;
        }

        self->disableForCheckall(false);
        QJsonArray ja;

        for(int i = 0, count = self->ui->vlDishes->count(); i < count; i++) {
            QLayoutItem *l = self->ui->vlDishes->itemAt(i);
            WaiterDishWidget *d = dynamic_cast<WaiterDishWidget*>(l->widget());

            if(d) {
                if(d->isChecked()) {
                    WaiterDish wd = d->mOrderItem;

                    if(wd.state == DISH_STATE_OK && !wd.isHourlyPayment() && wd.type == GOODS_TYPE_GOODS) {
                        ja.append(QJsonObject{{"f_id", wd.id}});
                    }
                }
            }
        }

        if(ja.isEmpty()) {
            return;
        } else if(C5Message::question(tr("Reprint selected items?")) != QDialog::Accepted) {
            return;
        }

        NInterface::query1("/engine/v2/waiter/order/print-service-check", user->mSessionKey, self,
        {{"items", ja}, {"reprint", true}, {"header_id", self->mOrder.id}},
        [self](const QJsonObject & jdoc) {
            if(!self) {
                return;
            }

            self->printService(jdoc);
            self->parseOrder(jdoc);
        });
    };
    funcWithAuth(cp_t5_waiter_reprint_goods, tr("Reprint service"), func);
}

void DlgOrder::on_btnGroupSelect_clicked()
{
    disableForCheckall(true);
}

void DlgOrder::on_btnRemoveSelected_clicked()
{
    disableForCheckall(false);
    QList<WaiterDish> dishes;
    bool needReason = false;
    QJsonArray ja;

    for(int i = 0, count = ui->vlDishes->count(); i < count; i++) {
        QLayoutItem *l = ui->vlDishes->itemAt(i);
        WaiterDishWidget *d = dynamic_cast<WaiterDishWidget*>(l->widget());

        if(d) {
            if(d->isChecked()) {
                WaiterDish wd = d->mOrderItem;

                if(wd.state == DISH_STATE_OK && !wd.isHourlyPayment() && wd.type == GOODS_TYPE_GOODS) {
                    dishes.append(d->mOrderItem);
                    ja.append(wd.toJson());

                    if(wd.isPrinted()) {
                        needReason = true;
                    }
                }
            }
        }
    }

    if(C5Message::question(tr("Do you want to remove this item")) != QDialog::Accepted) {
        return;
    }

    QString reason;
    int newState = 0;

    if(needReason) {
        if(!DlgListOfDishComments::getText(tr("Reason of remove"), "/engine/v2/waiter/menu/get-remove-reason", reason)) {
            return;
        }

        QStringList titles = {tr("Mistake"), tr("With store output"), tr("Cancel")};
        QList<int> values = {DISH_STATE_MISTAKE, DISH_STATE_VOID, 0};
        DlgSimleOptions dso(titles, values);
        newState = dso.exec();

        if(newState == 0) {
            return;
        }
    }

    for(int i = 0; i < ja.size(); i++) {
        QJsonObject jo = ja.at(i).toObject();
        QJsonObject jd = jo["f_data"].toObject();
        jd["f_remove_reason"] = reason;

        if(jd["f_printed"].toBool()) {
            jo["f_state"] = newState;
        } else {
            jo["f_state"] = 0;
        }

        jo["f_data"] = jd;
        ja[i] = jo;
    }

    QPointer<DlgOrder> self(this);
    auto removePrintedServiceFunc = [self, ja, reason](const QString & bearer) {
        NInterface::query("/engine/v2/waiter/order/remove-array-of-dishes", bearer, self, {
            {"order_id", self->mOrder.id},
            {"dishes", ja}
        },
        [self](const QJsonObject & jdoc) {
            QJsonArray ja = jdoc["removed_dishes"].toArray();

            for(int i = 0; i < ja.size(); i++) {
                self->printRemovedDish(ja.at(i).toObject());
            }

            self->parseOrder(jdoc);
        }, [](const QJsonObject & jerr) {
            return false;
        });
    };

    if(needReason) {
        if(mUser->check(cp_t5_waiter_remove_printed_goods)) {
            removePrintedServiceFunc(mUser->mSessionKey);
            return;
        } else {
            QString pin;

            if(!DlgPassword::getPasswordString(tr("Remove printed dish"), pin)) {
                return;
            }

            auto *user = new C5User();
            user->authorize(pin, self->fHttp, [self, removePrintedServiceFunc, user](const QJsonObject & jdoc) {
                if(user->check(cp_t5_waiter_remove_printed_goods)) {
                    removePrintedServiceFunc(user->mSessionKey);
                } else {
                    C5Message::error(tr("Permission denied"));
                }

                user->deleteLater();
            }, [user]() {
                user->deleteLater();
            });
            return;
        }
    } else {
        removePrintedServiceFunc(mUser->mSessionKey);
    }
}

void DlgOrder::on_btnSetPrecent_clicked()
{
    QPointer<DlgOrder> self(this);
    auto func = [self](C5User * user) {
        if(!self) {
            return;
        }

        QJsonArray ja;

        for(int i = 0, count = self->ui->vlDishes->count(); i < count; i++) {
            QLayoutItem *l = self->ui->vlDishes->itemAt(i);
            WaiterDishWidget *d = dynamic_cast<WaiterDishWidget*>(l->widget());

            if(d) {
                if(d->isChecked()) {
                    WaiterDish wd = d->mOrderItem;

                    if(!wd.isPrinted()) {
                        continue;
                    }

                    if(wd.state == DISH_STATE_OK && !wd.isHourlyPayment() && wd.type == GOODS_TYPE_GOODS) {
                        ja.append(QJsonObject{{"f_id", wd.id}});
                    }
                }
            }
        }

        if(ja.isEmpty()) {
            return;
        } else {
            if(C5Message::question(tr("Mark selected items as complimentary")) != QDialog::Accepted) {
                return;
            }
        }

        NInterface::query1("/engine/v2/waiter/order/complimentary-items", user->mSessionKey, self,
        {{"items", ja}, {"id", self->mOrder.id}},
        [self](const QJsonObject & jdoc) {
            if(!self) {
                return;
            }

            self->disableForCheckall(false);
            self->parseOrder(jdoc);
        });
    };
    funcWithAuth(cp_t5_waiter_special_payment_types, tr("Complimentary"), func);
}

void DlgOrder::on_btnPartFavorite_clicked()
{
    makeFavorites();
}
void DlgOrder::on_btnMenuHome_clicked()
{
    makeGroups(0, 0);
}
void DlgOrder::on_btnPart1_clicked()
{
    makeGroups(0, 1);
}
void DlgOrder::on_btnPart2_clicked()
{
    makeGroups(0, 2);
}
void DlgOrder::on_btnPart3_clicked()
{
    makeGroups(0, 3);
}
void DlgOrder::on_btnBackGroup_clicked()
{
    if(!mPreviouseParent.isEmpty()) {
        int group = mPreviouseParent.pop();
        makeGroups(group, 0);
    }
}

void DlgOrder::on_btnShowHideRemoved_clicked(bool checked)
{
    mShowRemoved = checked;
    ui->btnShowHideRemoved->setIcon(mShowRemoved ? QPixmap(":/eye-no.png") : QPixmap(":/eye.png"));

    for(int i = 0 ; i <  mOrder.dishes.size(); i++) {
        auto *layout = ui->vlDishes->itemAt(i);
        WaiterDishWidget *ow = qobject_cast<WaiterDishWidget*>(layout->widget());
        WaiterDish w = mOrder.dishes.at(i);
        ow->mShowRemoved = mShowRemoved;
        ow->updateDish(w);
    }
}

void DlgOrder::on_btnSetWholeAmount_clicked()
{
    double remain = mOrder.totalDue;

    while(auto *l = ui->vlPayment->takeAt(0)) {
        if(auto *b = qobject_cast<QToolButton*>(l->widget())) {
            remain -= b->property("amount").toDouble();
        }
    }

    ui->lbAmount->setProperty("amount", remain);
    ui->lbAmount->setProperty("str", QString::number(remain, 'f', 2));
    ui->lbAmount->setText(QString("%1 %2").arg(float_str(remain, 2), CURRENCY_SHORT));
}

void DlgOrder::on_btnNumClear_clicked()
{
    ui->lbAmount->setProperty("amount", 0);
    ui->lbAmount->setProperty("str", "");
    ui->lbAmount->setText(QString("0 %2").arg(CURRENCY_SHORT));
}

void DlgOrder::on_btnTransferDishes_clicked()
{
    QPointer<DlgOrder> self(this);
    auto moveTableFunc = [self](C5User * user) {
        if(!self) {
            return;
        }

        DlgSplitOrder d(self->mOrder, user);

        if(d.exec() == QDialog::Accepted) {
            self->accept();
        }
    };

    if(mUser->check(cp_t5_waiter_transfer_items)) {
        moveTableFunc(mUser);
        return;
    }

    QString pin;

    if(!DlgPassword::getPasswordString(tr("Remove printed dish"), pin)) {
        return;
    }

    auto *user = new C5User();
    user->authorize(pin, self->fHttp, [self, moveTableFunc, user](const QJsonObject & jdoc) {
        if(user->check(cp_t5_waiter_transfer_items)) {
            moveTableFunc(user);
        } else {
            C5Message::error(tr("Permission denied"));
        }

        user->deleteLater();
    }, [user]() {
        user->deleteLater();
    });
}

void DlgOrder::on_btnTransferTable_clicked()
{
    if(mOrder.isEmpty()) {
        C5Message::error(tr("Nothing to transfer"));
        return;
    }

    DlgTables d(mUser);
    int tableId = d.exec();

    if(tableId == 0) {
        return;
    }

    QPointer<DlgOrder> self(this);
    auto func = [self](C5User * user, int destinationTableId) {
        if(!self) {
            return;
        }

        NInterface::query1("/engine/v2/waiter/order/transfer-table", user->mSessionKey, self, {
            {"id", self->mOrder.id},
            {"locksrc", hostinfo},
            {"destination", destinationTableId}
        }, [self](const QJsonObject & jdoc) {
            C5Message::info(tr("Transfer successfull"));
            self->parseOrder(jdoc);
            self->on_btnExit_clicked();
        });
    };

    if(mUser->check(cp_t5_waiter_transfer_items)) {
        func(mUser, tableId);
    } else {
        QString pin;

        if(!DlgPassword::getPasswordString(tr("Confirm table transfer"), pin)) {
            return;
        }

        auto *user = new C5User();
        user->authorize(pin, self->fHttp, [self, func, user, tableId](const QJsonObject & jdoc) {
            if(user->check(cp_t5_waiter_transfer_items)) {
                func(user, tableId);
            } else {
                C5Message::error(tr("Permission denied"));
            }

            user->deleteLater();
        }, [user]() {
            user->deleteLater();
        });
    }
}

void DlgOrder::on_btnReopenTable_clicked()
{
    if(C5Message::question(tr("Do you want to reopen order?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query1("/engine/v2/waiter/order/reopen-order", mUser->mSessionKey, this,
    {{"id", mOrder.id}},
    [this](const QJsonObject & jdoc) {
        parseOrder(jdoc);
    });
}

void DlgOrder::on_btnPrintClosedFiscal_clicked()
{
    if(C5Message::question(tr("Do you want to print the fiscal receipt?")) != QDialog::Accepted) {
        return;
    }

    QPointer<DlgOrder> self(this);
    auto *loading = new NLoadingDlg(tr("Printing fiscal check"), this);
    auto *thread  = new QThread();
    auto *pt = new PrintTaxN(self->mUser->fConfig["tax_ip"].toString(),
                             self->mUser->fConfig["tax_port"].toString().toInt(),
                             self->mUser->fConfig["tax_password"].toString(),
                             self->mUser->fConfig["tax_external_pos"].toString(),
                             self->mUser->fConfig["tax_op"].toString(),
                             self->mUser->fConfig["tax_pin"].toString());
    pt->moveToThread(thread);
    auto order = self->mOrder;
    connect(thread, &QThread::started, pt, [ = ]() {
        for(auto d : std::as_const(order.dishes)) {
            if(d.state != DISH_STATE_OK) {
                continue;
            }

            pt->addGoods(d.fiscalDepartment(),
                         d.adgtCode(),
                         QString::number(d.dishId),
                         d.dishName,
                         d.price,
                         d.qty,
                         d.discountFactor() * 100);
        }

        pt->makeJsonAndPrint(order.paidCash(), order.paidCard(), order.paidPrepaid());
    });
    connect(pt, &PrintTaxN::started, loading, &QDialog::show, Qt::QueuedConnection);
    connect(pt, &PrintTaxN::finished, self, [ = ](const QString & inJson, const QString & outJson, const QString & err, int result) {
        loading->close();
        loading->deleteLater();
        QJsonObject reply{{"id", mOrder.id},
            {
                "fiscal", QJsonObject{
                    {"in", inJson},
                    {"out", outJson},
                    {"error", err},
                    {"result", result}}
            }};

        if(result == 0) {
            QJsonObject reply{{"id", mOrder.id},
                {
                    "fiscal", QJsonObject{
                        {"in", inJson},
                        {"out", outJson},
                        {"error", err},
                        {"result", result}}
                }};
            NInterface::query("/engine/v2/waiter/order/fiscal-printed", mUser->mSessionKey, self, reply,
            [self](const QJsonObject & jdoc) {
                self->parseOrder(jdoc);
                self->printPrecheck(self->mUser->shortFullName());
            },
            [](const QJsonObject & jerr) {return false;});
        } else {
            QJsonObject reply{{"id", mOrder.id},
                {"in", inJson},
                {"out", outJson},
                {"error", err},
                {"result", result}
            };
            NInterface::query("/engine/v2/waiter/order/fiscal-log", mUser->mSessionKey, self, reply,
            [](const QJsonObject & jdoc) {},
            [](const QJsonObject & jerr) {return false;});
            C5Message::error(err);
        }

        thread->quit();
    });
    connect(thread, &QThread::finished, pt, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}
