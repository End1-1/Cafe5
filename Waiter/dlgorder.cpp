#include "dlgorder.h"
#include <QAbstractScrollArea>
#include "dlgcustdisplay.h"
#include "dict_dish_state.h"
#include "../WaiterDesigner/waitergoodsgroupstyle.h"
#include "../WaiterDesigner/waitergoodsdishstyle.h"
#include <cmath>
#include <memory>
#include <QClipboard>
#include <QCloseEvent>
#include <QFile>
#include <QInputDialog>
#include <QDialog>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPointer>
#include <QPrinterInfo>
#include <QScreen>
#include <QScrollBar>
#include <QSet>
#include <QSettings>
#include <QShortcut>
#include <QThread>
#include <QToolButton>
#include <algorithm>
#include "c5message.h"
#include "c5permissions.h"
#include "c5printing.h"
#include "c5user.h"
#include "c5utils.h"
#include "customerinfo.h"
#include "dict_currency.h"
#include "dict_goods_types.h"
#include "dict_payment_type.h"
#include "dlgcl.h"
#include "dlgdishremovereason.h"
#include "dlgguest.h"
#include "dlgguestinfo.h"
#include "dlgguests.h"
#include "dlglist.h"
#include "dlglistdishspecial.h"
#include "dlglistofdishcomments.h"
#include "dlgmovemoney.h"
#include "dlgorderdatamatrix.h"
#include "dlgpreorderdatetime.h"
#include "dlgpassword.h"
#include "dlgprecheckoptions.h"
#include "dlgrecentdishes.h"
#include "dlgqty.h"
#include "dlgreceiptlanguage.h"
#include "dlgsearchinmenu.h"
#include "dlgsimleoptions.h"
#include "dlgsplitorder.h"
#include "dlgstoplistoption.h"
#include "dlgtables.h"
#include "dlgtext.h"
#include "dlgviewstoplist.h"
#include "format_date.h"
#include "goodsgroupbutton.h"
#include "httplite.h"
#include "idram.h"
#include "ninterface.h"
#include "nloadingdlg.h"
#include "printtaxn.h"
#include "qdishbutton.h"
#include "struct_workstationitem.h"
#include "ui_dlgorder.h"
#include "waiterdishwidget.h"
#include "waiterguestwidget.h"
#include "waitermodificatorwidget.h"

#define PART2_COL_WIDTH 150
#define PART2_ROW_HEIGHT 60
#define PART3_ROW_HEIGHT 80
#define PART4_ROW_HEIGHT 80

namespace {

void fillPackageNominalDeltas(QList<WaiterDish> &dishes)
{
    for(WaiterDish &pkg : dishes) {
        pkg.packageNominalDelta = 0.0;

        if(pkg.type != GOODS_TYPE_PACKAGE || pkg.state != DISH_STATE_OK) {
            continue;
        }

        const double fixedNominal = pkg.qty * pkg.price;
        double sumChildren = 0.0;

        for(const WaiterDish &ch : dishes) {
            if(ch.state == DISH_STATE_OK && ch.parent == pkg.id) {
                sumChildren += ch.qty * ch.price;
            }
        }

        pkg.packageNominalDelta = sumChildren - fixedNominal;
    }
}

bool isGoodsLikeType(int type)
{
    return type == GOODS_TYPE_GOODS || type == GOODS_TYPE_DISH;
}

bool isCustomerDisplayDishType(int type)
{
    switch(type) {
    case GOODS_TYPE_GOODS:
    case GOODS_TYPE_DISH:
    case GOODS_TYPE_SERVICE:
    case GOODS_TYPE_PACKAGE:
    case GOODS_TYPE_UNKNOWN:
        return true;

    default:
        return false;
    }
}

double customerDisplayLineAmount(const WaiterDish &dish, int orderState)
{
    const bool isPreorder = (orderState == ORDER_STATE_PREORDER);

    if(isPreorder || dish.isPrinted()) {
        return dish.total(isPreorder);
    }

    double base = dish.qty * dish.price;
    double delta = 0;

    if(dish.countService()) {
        delta += dish.serviceFactor();
    }

    if(dish.countDiscount()) {
        delta -= qAbs(dish.discountFactor());
    }

    return base * (1.0 + delta);
}

FiscalMachine fiscalMachineForWorkstation(const WorkstationItem &ws)
{
    return getFiscalMachine(ws.fiscalMachineId());
}

/** HDM v0.7.3: map Waiter payments to paidAmount / paidAmountCard / PaymentSystem. */
struct FiscalPaymentPayload {
    double cash = 0;
    double nonCash = 0;
    double prepaid = 0;
    int paymentSystem = -1; // 1 card, 10 telcell, 13 idram; -1 omit
    bool forceInternalPos = false; // idram/telcell → useExtPOS false
};

FiscalPaymentPayload fiscalPaymentsFromOrder(const WaiterOrder &order)
{
    FiscalPaymentPayload p;
    p.cash = order.paidCash();
    const double card = order.paidCard();
    const double idram = order.paidIdram();
    const double telcell = order.paidTelcell();
    p.nonCash = card + idram + telcell;
    p.prepaid = order.paidPrepaid();

    if (p.nonCash < 0.001) {
        return p;
    }

    // Dominant non-cash; ties: Card → Idram → Telcell
    if (card >= idram && card >= telcell && card > 0.001) {
        p.paymentSystem = 1;
        p.forceInternalPos = false;
    } else if (idram >= telcell && idram > 0.001) {
        p.paymentSystem = 13;
        p.forceInternalPos = true;
    } else if (telcell > 0.001) {
        p.paymentSystem = 10;
        p.forceInternalPos = true;
    }
    return p;
}

void applyFiscalPayments(PrintTaxN *pt, const FiscalPaymentPayload &pay)
{
    if (!pt) {
        return;
    }
    pt->setPaymentSystem(pay.paymentSystem);
    if (pay.forceInternalPos) {
        pt->setUseExtPosOverride(QStringLiteral("false"));
    } else {
        pt->setUseExtPosOverride(QString());
    }
}

QStringList collectFiscalEmarks(const WaiterOrder &order)
{
    QStringList result;
    auto appendUnique = [&result](const QString &code) {
        const QString trimmed = code.trimmed();
        if (trimmed.isEmpty() || result.contains(trimmed)) {
            return;
        }
        result.append(trimmed);
    };

    const QJsonValue datamatrix = order.data.value(QStringLiteral("f_datamatrix"));
    if (datamatrix.isArray()) {
        for (const QJsonValue &v : datamatrix.toArray()) {
            appendUnique(v.toString());
        }
    }

    for (const WaiterDish &dish : order.dishes) {
        if (dish.state != DISH_STATE_OK) {
            continue;
        }
        appendUnique(dish.emarks());
    }
    return result;
}

int receiptNameWidthMm(int baseWidthMm, int sideMarginMm)
{
    if(sideMarginMm <= 0) {
        return baseWidthMm;
    }
    return qMax(8, baseWidthMm - 2 * sideMarginMm);
}

}

DlgOrder::DlgOrder(C5User *user, HallItem h, TableItem t, const QVector<GoodsGroupItem*>* groups, const QVector<DishAItem*>* dishes) :
    C5WaiterDialog(user),
    ui(new Ui::DlgOrder),
    mHall(h),
    mTable(t),
    mGroups(groups),
    mDishes(dishes)
{
    ui->setupUi(this);
    {
        const WaiterGoodsGroupStyle &groupStyle = WaiterGoodsGroupStyle::cachedStyle();
        ui->glGroups->setSpacing(groupStyle.spacing);
        ui->wgroups->setMaximumHeight(groupStyle.stripMaxHeight());
    }
    ui->glGroups->setSizeConstraint(QLayout::SetNoConstraint);
    ui->scrollAreaWidgetContents->setMinimumSize(0, 0);
    ui->scrollAreaWidgetContents->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
    ui->groupsScrollArea->setMinimumHeight(0);
    ui->groupsScrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    constexpr int toolbarRowHeight = 54;
    ui->wqtypanelup->setFixedHeight(toolbarRowHeight);
    ui->wdepts->setFixedHeight(toolbarRowHeight);
    for (QAbstractButton *b : ui->wqtypanelup->findChildren<QAbstractButton *>()) {
        b->setMinimumHeight(toolbarRowHeight);
        b->setMaximumHeight(toolbarRowHeight);
    }
    for (QAbstractButton *b : ui->wdepts->findChildren<QAbstractButton *>()) {
        b->setMinimumHeight(toolbarRowHeight);
        b->setMaximumHeight(toolbarRowHeight);
    }
    ui->horizontalLayout->setAlignment(Qt::AlignTop);
    ui->gridLayout->setRowStretch(0, 1);
    ui->gridLayout->setRowStretch(1, 0);
    ui->gridLayout->setRowStretch(2, 0);
    ui->gridLayout->setAlignment(ui->wclosedorder, Qt::AlignTop);
    ui->gridLayout->setAlignment(ui->wpayment, Qt::AlignTop);
    ui->lmenua->setStretch(ui->lmenua->indexOf(ui->wdepts), 0);
    ui->lmenua->setStretch(ui->lmenua->indexOf(ui->wgroups), 0);
    ui->lmenua->setStretch(ui->lmenua->indexOf(ui->wdishes), 1);
    ui->verticalLayout_5->setStretch(ui->verticalLayout_5->indexOf(ui->dishScrollArea), 1);
    ui->glDishes->setSizeConstraint(QLayout::SetNoConstraint);
    ui->scrollAreaWidgetContents_2->setMinimumSize(0, 0);
    ui->scrollAreaWidgetContents_2->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    ui->dishScrollArea->setMinimumHeight(0);
    ui->dishScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->dishScrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    ui->wdishes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->btnPreorderDateTime->setText(tr("Preorder datetime"));
    installEventFilter(this);
    ui->wOrderInfo->installEventFilter(this);
    ui->lbTableName->installEventFilter(this);
    ui->lbOrderNumber->installEventFilter(this);
    ui->lbStaff->installEventFilter(this);
    ui->lbTime->installEventFilter(this);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // Window-wide: numpad/keyboard +/- work even when focus is on a child (dish button, etc.)
    auto addQtyShortcut = [this](QKeySequence seq, void (DlgOrder::*slot)()) {
        auto *sc = new QShortcut(seq, this);
        sc->setContext(Qt::WindowShortcut);
        connect(sc, &QShortcut::activated, this, slot);
    };
    addQtyShortcut(QKeySequence(Qt::Key_Plus), &DlgOrder::on_btnPlus1_clicked);
    addQtyShortcut(QKeySequence(Qt::Key_Minus), &DlgOrder::on_btnMinus1_clicked);
    addQtyShortcut(QKeySequence(Qt::KeypadModifier | Qt::Key_Plus), &DlgOrder::on_btnPlus1_clicked);
    addQtyShortcut(QKeySequence(Qt::KeypadModifier | Qt::Key_Minus), &DlgOrder::on_btnMinus1_clicked);

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
#ifndef QT_DEBUG
    ui->btnCopyUUID->setVisible(false);
#endif
    createPaymentButtons();
    configBtnNum();
    configCashTenderButtons();
    configOtherButtons();
    setupButtons();
}

DlgOrder::~DlgOrder()
{
    closeCustomerDisplay();
    delete ui;
}

void DlgOrder::setOrderId(const QString &id, bool reopenIfClosed)
{
    mSkipOpenTableOnShow = true;
    mOrder.id = id;
    const char *route = reopenIfClosed
        ? "/engine/v2/waiter/order/reopen-order"
        : "/engine/v2/waiter/order/open-order";
    NInterface::query1(route, mUser->mSessionKey, this,
    {{"id", id}}, [this](const QJsonObject & jdoc) {
        parseOrder(jdoc);
    });
}

void DlgOrder::setCreateAsPreorder(bool value)
{
    mCreateAsPreorder = value;
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

void DlgOrder::updateStopList(const QJsonArray &objs)
{
    for (int i = 0; i < objs.size(); i++) {
        for (auto *d : *mDishes) {
            if (d->id == objs.at(i).toInt()) {
                d->stoplist = -1;
            }
        }
    }
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
    closeCustomerDisplay();
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
    if(e->type() == QEvent::MouseButtonRelease) {
        if(o == ui->wOrderInfo
                || o == ui->lbTableName
                || o == ui->lbOrderNumber
                || o == ui->lbStaff
                || o == ui->lbTime) {
            on_btnExit_clicked();
            return true;
        }
    }

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

        // Numpad / keyboard +/- → same as btnPlus1 / btnMinus1
        // Match by key and by text: some layouts/numpads deliver text without Key_Minus.
        const QString t = k->text();
        if(k->key() == Qt::Key_Plus || t == QLatin1String("+")) {
            on_btnPlus1_clicked();
            return true;
        }
        if(k->key() == Qt::Key_Minus || t == QLatin1String("-") || t == QString(QChar(0x2212))) {
            on_btnMinus1_clicked();
            return true;
        }

        if(!t.isEmpty()) {
            mStringBuffer += t;
            return true;
        }
    }

    return C5WaiterDialog::eventFilter(o, e);
}

void DlgOrder::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);

    for (auto child : ui->wappcontainer->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly)) {
        qDebug() << child->objectName() << "minHint:" << child->minimumSizeHint();
    }

    if(mTable.id > 0) {
        fMenuID = mHall.defaultMenu();
        makeGroups(0, 0);

        if(!mSkipOpenTableOnShow) {
            fHttp->createHttpQueryLambda("/engine/v2/waiter/order/open-table", {
                {"table", mTable.id},
                {"locksrc", hostinfo},
                {"create_as_preorder", mCreateAsPreorder ? 1 : 0}
            },
            [this](const QJsonObject  & jdoc) {
                parseOrder(jdoc);
                for (int i = 0, count = ui->vlDishes->count(); i < count; i++) {
                    QLayoutItem *l = ui->vlDishes->itemAt(i);
                    WaiterDishWidget *d = dynamic_cast<WaiterDishWidget *>(l->widget());

                    if (d) {
                        WaiterDish wd = d->mOrderItem;

                        if (wd.state == DISH_STATE_OK && wd.isHourlyPayment()) {
                            QPointer<DlgOrder> self(this);
                            auto updateAmounts = [self](const QString &bearer) {
                                NInterface::query(
                                    "/engine/v2/waiter/order/update-amounts",
                                    bearer,
                                    self,
                                    {
                                        {"id", self->mOrder.id},
                                    },
                                    [self](const QJsonObject &jdoc) { self->parseOrder(jdoc); },
                                    [](const QJsonObject &jerr) { return false; });
                            };
                            updateAmounts(self->mUser->mSessionKey);
                            break;
                        }
                    }
                }
            }, [this](const QJsonObject & jerr) {
                reject();
            });
        }
    }

    ui->vlDishes->addStretch();
    tryOpenCustomerDisplayIfEnabled();
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

    for(int i = 0; i < dcolCount; i++) {
        ui->glGroups->setColumnStretch(i, 1);
    }

    makeDishes(parent, false);
}

void DlgOrder::makeDishes(int group, int favorite)
{
    int scrollBarWidth = ui->dishScrollArea->verticalScrollBar()->isVisible() ? ui->dishScrollArea->verticalScrollBar()->width() : 0;
    if (ui->dishScrollArea->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOn) {
        scrollBarWidth = ui->dishScrollArea->verticalScrollBar()->width();
    }
    int     availableWidth = ui->dishScrollArea->viewport()->width();

    const WaiterGoodsDishStyle &dishStyle = WaiterGoodsDishStyle::cachedStyle();
    ui->glDishes->setSpacing(dishStyle.spacing);

    while (ui->glDishes->itemAt(0)) {
        ui->glDishes->itemAt(0)->widget()->deleteLater();
        ui->glDishes->removeItem(ui->glDishes->itemAt(0));
    }

    QRect scr = qApp->screens().at(mScreen < 0 ? 0 : mScreen)->geometry();
    int dcolCount = scr.width() > 1024 ? 4 : 3;
    int dcol = 0;
    int drow = 0;

    for (auto *g : *mDishes) {
        if (g->menuId != fMenuID) {
            continue;
        }

        if (group && g->group != group) {
            continue;
        }

        if (favorite && !g->favorite) {
            continue;
        }

        int w = (availableWidth - (ui->glDishes->spacing() * (dcolCount + 1))) / dcolCount;
        auto *btn = new QDishButton(g, w);
        connect(btn, &QDishButton::clicked, this, [this, btn, g]() { addDishToOrder(g, btn); });
        ui->glDishes->addWidget(btn, drow, dcol++, 1, 1);

        if (dcol == dcolCount) {
            dcol = 0;
            drow++;
        }

        ui->glDishes->setRowStretch(drow + 1, 1);
    }
}

void DlgOrder::confirmStringBuffer()
{
    if (fStoplistMode) {
        mStringBuffer.clear();
        return;
    }
    if(mStringBuffer.isEmpty()) {
        return;
    }
#ifdef QT_DEBUG
    //mStringBuffer = "0104850001011187211mIpTe<Q:m_dj93b2zi";
    //mStringBuffer = "0104850001011187211MAmKY;Y'KhvJ93htBY";
    mStringBuffer = "0104850001011187211Xwde\"LOm!w!O93YrOz";
    // mStringBuffer = "0104850001011187211amYts/>Vjuhl93wbM6";
    // mStringBuffer = "0104850001011187211MAmKY;Y'KhvJ93htBY";
#endif

    mStringBuffer = mStringBuffer.trimmed();
    QString hya("էթփձջւևրչճԷԹՓՁՋՒևՐՉՃ");
    QString num("12345678901234567890");
    QString newcode;

    for (int i = 0; i < mStringBuffer.length(); i++) {
        if (hya.contains(mStringBuffer.at(i))) {
            newcode += num.at(hya.indexOf(mStringBuffer.at(i)));
        } else {
            newcode += mStringBuffer.at(i);
        }
    }

    mStringBuffer.clear();
    if (newcode.isEmpty()) {
        return;
    }

    QString emarks;
    QString barcode;
    if (newcode.length() >= 29) {
        if (newcode.mid(0, 6) == "000000") {
            barcode = newcode.mid(6, 8);
        } else if (newcode.mid(0, 8) == "01000000") {
            barcode = newcode.mid(8, 8);
        } else if (newcode.mid(0, 3) == "010") {
            barcode = newcode.mid(3, 13);
        } else {
            barcode = newcode.mid(1, 8);
            if (barcode.isEmpty()) {
                barcode = newcode.mid(1, 13);
            }
        }

        emarks = newcode;

        if (barcode.isEmpty()) {
            C5Message::error(tr("Invalid emarks"));
            return;
        }
    } else if (newcode.length() == 13 || newcode.length() == 8) {
        barcode = newcode;
    }

    if (barcode.isEmpty()) {
        return;
    }

    for (auto *g : *mDishes) {
        if (g->barcodes.contains(barcode)) {
            QJsonObject od = g->data;
            g->data["f_emarks"] = emarks;
            addDishToOrder(g, nullptr);
            g->data = od;
        }
    }
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
    int bs = 20;
    QFont font(qApp->font());
    font.setPointSize(bs);
    const QString printerTarget = mWorkStation.precheckPrinter();
    if (printerTarget.isEmpty()) {
        C5Message::error(tr("Precheck printer is not configured"));
        return;
    }
    const bool viaPrintServer = WorkstationItem::isPrintServerTarget(printerTarget);
    const QString progressTitle = tr("Printer: %1").arg(printerTarget);
    if (NLoadingDlg *dlg = NInterface::currentLoadingDialog()) {
        dlg->setTitle(progressTitle);
        qApp->processEvents();
    }
    QElapsedTimer shownFor;
    shownFor.start();

    auto holdProgressVisible = [&]() {
        if (NLoadingDlg *dlg = NInterface::currentLoadingDialog()) {
            dlg->setTitle(progressTitle);
        }
        while (shownFor.elapsed() < 2000) {
            qApp->processEvents(QEventLoop::AllEvents, 50);
            QThread::msleep(30);
        }
    };

    C5Printing p;
    std::unique_ptr<QPrinter> printer;
    if (!viaPrintServer) {
        const QPrinterInfo pi = QPrinterInfo::printerInfo(printerTarget);
        if (pi.isNull()) {
            holdProgressVisible();
            C5Message::error(tr("Printer not found") + ": " + printerTarget);
            return;
        }
        printer = std::make_unique<QPrinter>(pi);
        printer->setPageSize(QPageSize::Custom);
        printer->setFullPage(false);
        QRectF pr = printer->pageRect(QPrinter::DevicePixel);
        constexpr qreal SAFE_RIGHT_MM = 4.0;
        qreal safePx = SAFE_RIGHT_MM * printer->logicalDpiX() / 25.4;
        p.setSceneParams(pr.width() - safePx, pr.height(), printer->logicalDpiX());
    } else {
        p.setSceneParams(650, 2800, 96);
    }
    p.setFont(font);
    p.setFontSize(bs);
    const int sideMarginMm = qMax(0, mWorkStation.printPaperWidthMm());
    p.setRightMarginMm(sideMarginMm);
    const int nameWidthMm = receiptNameWidthMm(35, sideMarginMm);
    QString logoFile = qApp->applicationDirPath() + "/logo_receipt.png";

    if(QFile::exists(logoFile)) {
        p.image(logoFile, Qt::AlignHCenter);
        p.br();
    }
    const QString &receipt_phone = mWorkStation.data.value("receipt_phone").toString();
    if (!receipt_phone.isEmpty()) {
        p.ctext(receipt_phone);
        p.br();
    }

    p.setFontSize(bs + 4);
    switch(mOrder.state) {
    case ORDER_STATE_OPEN:
    case ORDER_STATE_CLOSE:
        p.ltext(tr("Order No"), sideMarginMm);
        break;

    case ORDER_STATE_PREORDER:
        p.ltext(tr("Preorder No"), sideMarginMm);
        break;

    default:
        p.ltext(QString::number(mOrder.state) + ": " + tr("Error in state"), sideMarginMm);
        break;
    }

    p.rtext(mOrder.receiptNumber);
    p.br();
    p.setFontSize(bs);
    QJsonObject jtax = mOrder.fiscal();

    if (!jtax.isEmpty()) {
        // jtax = QJsonDocument::fromJson(jtax.value("out").toString().toUtf8()).object();
        p.ltext(jtax["taxpayer"].toString(), sideMarginMm);
        p.br();
        p.ltext(jtax["address"].toString(), sideMarginMm);
        p.br();
        p.ltext(tr("TIN"), sideMarginMm);
        p.rtext(jtax["tin"].toString());
        p.br();
        p.ltext(tr("Device number"), sideMarginMm);
        p.rtext(jtax["crn"].toString());
        p.br();
        p.ltext(tr("Serial"), sideMarginMm);
        p.rtext(jtax["sn"].toString());
        p.br();
        p.ltext(tr("Fiscal"), sideMarginMm);
        p.rtext(jtax["fiscal"].toString());
        p.br();
        p.ltext(tr("Receipt number"), sideMarginMm);
        p.rtext(QString::number(jtax["rseq"].toInt()));
        p.br();
        p.ltext(tr("Date"), sideMarginMm);
        p.rtext(QDateTime::fromMSecsSinceEpoch(jtax["time"].toDouble()).toString(FORMAT_DATETIME_TO_STR));
        p.br();
        p.ltext(tr("(F)"), sideMarginMm);
        p.br();
    }

    if (mOrder.data.contains("f_guest")) {
        const QJsonObject guest = mOrder.dataValue("f_guest").toObject();
        p.ltext(tr("Client"), sideMarginMm);
        p.br();
        if(!mWorkStation.data.value(QStringLiteral("do_not_print_customer_on_receipt")).toBool()) {
            p.ltext(guest.value(QStringLiteral("f_guest_name")).toString(), sideMarginMm);
            p.br();
        }
        p.ltext(guest.value(QStringLiteral("f_guest_phone")).toString(), sideMarginMm);
        p.br();
        p.ltext(guest.value(QStringLiteral("f_guest_address")).toString(), sideMarginMm);
        p.br();
    }

    p.br(1);
    if (!mWorkStation.data.value("receipt_no_table").toBool()) {
        p.ltext(tr("Table"), sideMarginMm);
        p.rtext(QString("%1/%2").arg(mOrder.hallName, mOrder.tableName));
        p.br();
    }
    p.ltext(tr("Staff"), sideMarginMm);
    p.rtext(currentStaff);
    p.br();
    p.br(2);
    p.line(2);
    p.br(2);
    //p.setFontSize(bs - 4);
    p.ltext(tr("Name"), sideMarginMm, nameWidthMm);
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
            p.ltext(QString("%1: %2").arg(tr("Class"), dish.adgtCode()), sideMarginMm);
            p.br();
        }

        QString name = dish.translated();

        if (!dish.countService()) {
            noservice = true;
            if (!mWorkStation.data.value("receipt_no_service_hint").toBool()) {
                name += "* ";
            }
        }

        if (!dish.countDiscount()) {
            nodiscount = true;
            if (!mWorkStation.data.value("receipt_no_discount_hint").toBool()) {
                name += "** ";
            }
        }

        if(dish.complimentary()) {
            complimentary = true;
            name += "*** ";
        }

        p.ltext(name, sideMarginMm, nameWidthMm);
        p.ltext(float_str(dish.qty, 2), 33, 8);
        p.ltext(float_str(dish.price, 2), 41, 12);
        p.rtext(float_str(dish.total(mOrder.state == ORDER_STATE_PREORDER), 2));
        p.br();
        p.br(2);
        p.line();
        p.br(1);
    }

    p.setFontSize(bs  - 2);

    if (noservice) {
        if (!mWorkStation.data.value("receipt_no_service_hint").toBool()) {
            p.ltext(QString("* - %1").arg(tr("No service")).toLower(), sideMarginMm);
            p.br();
        }
    }

    if (nodiscount) {
        if (!mWorkStation.data.value("receipt_no_discount_hint").toBool()) {
            p.ltext(QString("** - %1").arg(tr("No discount")).toLower(), sideMarginMm);
            p.br();
        }
    }

    if(complimentary) {
        p.ltext(QString("*** - %1").arg(tr("Complimentary")).toLower(), sideMarginMm);
        p.br();
    }

    p.setFontSize(bs + 2);
    p.ltext(tr("Subtotal"), sideMarginMm);
    p.rtext(float_str(mOrder.subTotal(), 2));
    p.br();

    if (mOrder.serviceFactor() > 0) {
        const QString serviceComment = mOrder.data.value(QStringLiteral("f_service_comment")).toString().trimmed();
        p.ltext(serviceComment.isEmpty() ? tr("Service") : serviceComment, sideMarginMm);
        p.rtext("+" + float_str(mOrder.serviceFactor() * 100, 2) + "%");
        p.br();
    }

    if (mOrder.discountFactor() > 0) {
        const QString discountComment = mOrder.data.value(QStringLiteral("f_discount_comment")).toString().trimmed();
        p.ltext(discountComment.isEmpty() ? tr("Discount") : discountComment, sideMarginMm);
        p.rtext("-" + float_str(mOrder.discountFactor() * 100, 2) + "%");
        p.br();
    }

    if(mOrder.prepaidAmount() > 0) {
        p.ltext(tr("Prepaid amount"), sideMarginMm);
        p.setFontSize(bs);
        p.rtext(float_str(mOrder.prepaidAmount() * -1, 2));
        p.br();
    }

    p.ltext(tr("Total due"), sideMarginMm);
    p.rtext(float_str(mOrder.totalDue, 2));
    p.br();
    p.br();
    auto printPaymentFunc = [this, &p, sideMarginMm](int id) {
        if(mOrder.payment(payment_fields[id]) > 0) {
            p.ltext(QCoreApplication::translate("PaymentType", payment_names[id]), sideMarginMm);
            p.rtext(float_str(mOrder.payment(payment_fields[id]), 2));
            p.br();
        }
    };

    for(auto pt : payment_types) {
        printPaymentFunc(pt);
    }

    if(mOrder.amountPaid() - mOrder.totalDue > 0) {
        p.br();
        p.ltext(tr("Amount paid"), sideMarginMm);
        p.rtext(float_str(mOrder.amountPaid(), 2));
        p.br();
        p.ltext(tr("Change"), sideMarginMm);
        p.rtext(float_str(mOrder.amountPaid() - mOrder.totalDue, 2));
        p.br();
    }

    bool printSignature = false;
    p.setFontSize(bs - 2);
    QString receiptPolicy = mWorkStation.data.value("receipt_policy").toString();
    if (!receiptPolicy.isEmpty()) {
        printSignature = true;
        p.br();
        p.br();
        p.ltext(receiptPolicy, sideMarginMm);
        p.br();
    }

    if (printSignature) {
        p.br();
        p.br();
        p.br();
        p.br();
        p.line(2);
        p.br();
        p.ctext(tr("Signature"));
        p.br();
    }

    p.br();
    p.setFontSize(bs - 2);
    p.ltext(tr("Thank you for visit!"), sideMarginMm);
    p.br();

    if(mOrder.state == ORDER_STATE_OPEN || mOrder.state == ORDER_STATE_CLOSE) {
        p.ltext(QString("%1: %2").arg(tr("Sample")).arg(mOrder.printCount()), sideMarginMm);
    }

    p.br();
    p.ltext(tr("Printed"), sideMarginMm);
    p.rtext(QDateTime::currentDateTime().toString(FORMAT_DATETIME_TO_STR));
    p.br();

    if (viaPrintServer) {
        HttpLite *http = new HttpLite(this);
        QJsonObject json;
        json["print_data"] = p.jsonData();
        http->post(printerTarget, json);
        holdProgressVisible();
    } else if (!p.print(*printer)) {
        holdProgressVisible();
        C5Message::error(tr("Print failed") + ": " + printerTarget);
    } else {
        holdProgressVisible();
    }
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
        const int bs = 22;
        font.setPointSize(bs);
        C5Printing p;
        QPrinterInfo pi = QPrinterInfo::printerInfo(printerName);
        QPrinter printer(pi);
        QRectF pr = printer.pageRect(QPrinter::DevicePixel);

        // Вместо printer.logicalDpiX() используем жестко 96
        qreal fixedDpi = 96.0;

        // Передаем fixedDpi в параметры сцены
        p.setSceneParams(pr.width(), pr.height(), fixedDpi);
        p.setFont(font);
        p.setFontBold(true);
        p.setFontSize(bs);
        const int sideMarginMm = qMax(0, mWorkStation.printPaperWidthMm());
        p.setRightMarginMm(sideMarginMm);
        const int nameWidthMm = receiptNameWidthMm(65, sideMarginMm);
        if(jdoc["reprint"].toBool()) {
            p.ctext(tr("REPRINT"));
            p.br();
            p.br();
        }

        if(mOrder.state == ORDER_STATE_PREORDER) {
            p.ctext("PREORDER");
            p.br();
            p.br();
        }

        p.ctext(tr("New order").toUpper());
        p.br();
        p.br();
        p.setFontBold(false);
        p.ltext(tr("Table"), sideMarginMm);
        p.rtext(jh["f_table_name"].toString());
        p.br();
        p.ltext(tr("Order no"), sideMarginMm);
        p.rtext(jh["f_prefix"].toString());
        p.br();
        p.ltext(tr("Date"), sideMarginMm);
        p.rtext(QDate::currentDate().toString(FORMAT_DATE_TO_STR));
        p.br();
        p.ltext(tr("Time"), sideMarginMm);
        p.rtext(QTime::currentTime().toString(FORMAT_TIME_TO_STR));
        p.br();
        p.ltext(tr("Staff"), sideMarginMm);
        p.rtext(mUser->shortFullName());
        p.br();
        p.line();
        p.br(2);
        QSet<QString> storages;

        for(auto const &jdv : jo["dishes"].toArray()) {
            p.setFontSize(bs + 2);
            p.setFontBold(false);
            QJsonObject jd = jdv.toObject();
            const bool fromPackage = jd.value(QStringLiteral("f_is_package_component")).toBool();
            const QString dishName = fromPackage
                                         ? QStringLiteral(">>> %1").arg(jd[QStringLiteral("f_dish_name")].toString())
                                         : jd[QStringLiteral("f_dish_name")].toString();
            p.ltext(dishName, sideMarginMm, nameWidthMm);
            p.setFontBold(true);
            const QString qtyLine = jd.value(QStringLiteral("f_qty_line")).toString();
            const QString qtyOut = qtyLine.isEmpty()
                                       ? float_str(jd[QStringLiteral("f_qty")].toDouble(), 2)
                                       : qtyLine;
            p.rtext(qtyOut);

            if(jd["f_comment"].toString().length() > 0) {
                p.br();
                p.setFontSize(bs - 4);
                p.setFontBold(true);
                p.ltext(jd["f_comment"].toString(), sideMarginMm, 650);
                p.br();
                p.setFontSize(bs + 2);
            }

            p.br();
            p.line();
            p.br(1);
        }

        p.line();
        p.br(1);
        p.setFontSize(bs - 6);
        p.ltext(QString("%1 %2").arg(tr("Printer: "), printerName), sideMarginMm);
        p.setFontBold(true);
        p.rtext(jo["side"].toString());
        p.br();

        if (!mWorkStation.printServer().isEmpty()) {
            NInterface::query1(mWorkStation.printServer(),
                               mUser->mSessionKey,
                               this,
                               {{"printer_name", printerName}, {"print_data", p.jsonData()}},
                               [this](const QJsonObject &jdoc) {

                               });
        } else {
            if (!p.print(printer)) {
                C5Message::error(tr("Print error"));
            }
        }
    }
}

bool DlgOrder::isBistroMode() const
{
    return mWorkStation.data.value(QStringLiteral("bistro")).toBool();
}

double DlgOrder::orderDisplayTotalDue() const
{
    if (isBistroMode()) {
        return mOrder.calculatedTotalDue(true);
    }
    return mOrder.totalDue;
}

void DlgOrder::applyBistroPrecheckAfterService()
{
    if(mOrder.id.isEmpty() || mOrder.isPrecheckPrinted()) {
        return;
    }

    const QString route = mOrder.state == ORDER_STATE_PREORDER
                              ? QStringLiteral("/engine/v2/waiter/order/print-precheck-of-preorder")
                              : QStringLiteral("/engine/v2/waiter/order/print-precheck");

    NInterface::query1(route, mUser->mSessionKey, this, {{"id", mOrder.id}}, [this](const QJsonObject &jdoc) { parseOrder(jdoc); });
}

void DlgOrder::syncOrderDishWidgetsVisibility()
{
    const int dishCount = mOrder.dishes.size();

    for(int i = 0; i < dishCount; ++i) {
        if(i >= ui->vlDishes->count()) {
            break;
        }

        QLayoutItem *li = ui->vlDishes->itemAt(i);
        QWidget *widget = li ? li->widget() : nullptr;

        if(!widget) {
            continue;
        }

        const WaiterDish &d = mOrder.dishes.at(i);

        if(d.state == DISH_STATE_NONE) {
            widget->setVisible(false);
        } else if(d.state == DISH_STATE_OK || d.state == DISH_STATE_SET) {
            widget->setVisible(true);
        } else {
            widget->setVisible(mShowRemoved);
        }
    }
}

void DlgOrder::removeStaleOrderDishRows()
{
    /* Last layout item is the stretch from showEvent. */
    while(ui->vlDishes->count() > mOrder.dishes.size() + 1) {
        const int idx = mOrder.dishes.size();
        QLayoutItem *taken = ui->vlDishes->takeAt(idx);

        if(!taken) {
            break;
        }

        if(QWidget *w = taken->widget()) {
            w->deleteLater();
        }

        delete taken;
    }
}

void DlgOrder::resetBistroOrderUi()
{
    on_btnNumClear_clicked();
    mSelectedPackage.clear();
    mShowRemoved = false;
    ui->btnShowHideRemoved->setChecked(false);
    ui->btnShowHideRemoved->setIcon(QPixmap(QStringLiteral(":/eye.png")));
    ui->lbOrderComment->clear();
    ui->lbOrderComment->setVisible(false);
    ui->lbAmountPaid->setText(QStringLiteral("0 %1").arg(CURRENCY_SHORT));
    ui->lbChange->setText(QStringLiteral("0 %1").arg(CURRENCY_SHORT));

    while(QLayoutItem *l = ui->vlPayment->takeAt(0)) {
        if(QWidget *w = l->widget()) {
            w->deleteLater();
        }

        delete l;
    }
}

void DlgOrder::startNewOrderOnTableAfterBistroClose()
{
    /* Same as initial table open: empty shell until first add-dish (CreateOrder runs there with cashbox_id). */
    fHttp->createHttpQueryLambda(QStringLiteral("/engine/v2/waiter/order/open-table"),
                                 {{QStringLiteral("table"), mTable.id}, {QStringLiteral("locksrc"), hostinfo}},
                                 [this](const QJsonObject &jdoc) {
                                     parseOrder(jdoc);

                                     if(isBistroMode()) {
                                         resetBistroOrderUi();
                                     }
                                 },
                                 [](const QJsonObject &) {});
}

void DlgOrder::printRemovedDish(const QJsonObject &jdoc)
{
    auto printRemoved = [this, jdoc](const QString & printerName) {
        if(printerName.isEmpty()) {
            return;
        }

        int bs = 22;
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
        p.br();
        p.line();
        p.setFontSize(bs + 2);
        p.ltext(QString("%1").arg(jdoc["f_removed_dish_name"].toString()), 0);
        p.setFontBold(true);
        p.rtext(QString("%1").arg(float_str(jdoc["f_removed_qty"].toDouble(), 2)));
        p.setFontBold(false);

        if(jdoc["f_removed_comment"].toString().length() > 0) {
            p.br();
            p.setFontSize(bs - 2);
            p.setFontBold(true);
            p.ltext(jdoc["f_removed_comment"].toString(), 0);
            p.br();
            p.setFontSize(bs);
            p.setFontBold(false);
        }

        p.br();
        p.line();
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
    const bool lockByPrecheck = mOrder.isPrecheckPrinted() && mOrder.state != ORDER_STATE_PREORDER;
    if(lockByPrecheck) {
        C5Message::error(tr("Order is not editable"));
        return;
    }

    int index = selectedWaiterDishIndex();

    if(index < 0) {
        return;
    }

    WaiterDish d = mOrder.dishes.at(index);

    if (d.isHourlyPayment()) {
        C5Message::error(tr("Hourly payment"));
        return;
    }

    /* Package (type 5):
     * - printed: duplicate full package copies on server;
     * - not printed: regular quantity update for the package row. */
    if(d.type == GOODS_TYPE_PACKAGE) {
        if(d.state != DISH_STATE_OK) {
            return;
        }

        double qty = getQty(d);

        if(qty <= 0.000001) {
            return;
        }

        if(!d.isPrinted()) {
            fHttp->createHttpQueryLambda("/engine/v2/waiter/order/set-dish-qty", {
                {"id", d.id},
                {"remove_emarks", false},
                {"dish", d.dishId},
                {"dish_name", d.dishName},
                {"new_qty", qty},
                {"new_state", d.state},
                {"data", d.data},
                {"order_id", mOrder.id},
                {"restore_stoplist", -1}
            },
            [this](const QJsonObject & jdoc) {
                parseOrder(jdoc);
            }, [](const QJsonObject & jerr) {});
            return;
        }

        const int copies = qBound(1, static_cast<int>(std::floor(qty + 1e-9)), 50);
        fHttp->createHttpQueryLambda(
            "/engine/v2/waiter/order/duplicate-printed-package",
            {
                {"header_id", mOrder.id},
                {"package_id", d.id},
                {"copies", copies},
                {"cashbox_id", mWorkStation.cashboxId()},
                {"create_process", mWorkStation.customerNotification()},
            },
            [this](const QJsonObject &jdoc) {
                parseOrder(jdoc);
            },
            [](const QJsonObject &) {});
        return;
    }

    if(!isGoodsLikeType(d.type) && d.type != GOODS_TYPE_MODIFICATOR) {
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

    if(qty <= 0.000001) {
        return;
    }

    if(d.isPrinted()) {
        int targetId = d.dishId;
        auto it = std::find_if(mDishes->begin(), mDishes->end(), [targetId](DishAItem *item) { return item && item->id == targetId; });
        QJsonObject newData;
        if (it != mDishes->end()) {
            DishAItem *foundItem = *it;
            newData = foundItem->data;
        }
        fHttp->createHttpQueryLambda(
            "/engine/v2/waiter/order/add-dish",
            {{"dish", d.dishId},
             {"dish_name", d.dishName},
             {"parent", d.parent},
             {"table", mTable.id},
             {"qty", qty},
             {"type", d.type},
             {"row", ((d.row / 100) + 1) * 100},
             {"price", d.price},
             {"store", d.store},
             {"f_data", newData},
             {"print1", d.printer1()},
             {"print2", d.printer2()},
             {"cashbox_id", mWorkStation.cashboxId()},
             {"shift_rows", true},
             {"count_service", d.countService()},
             {"count_discount", d.countDiscount()},
             {"empty_order", mOrder.dishes.empty()},
             {"service_factor", mHall.serviceFactor()},
             {"create_process", mWorkStation.customerNotification()}},
            [this, d](const QJsonObject &jdoc) {
                if (jdoc.contains("stoplist")) {
                    for (auto *jg : *mDishes) {
                        if (jg->id == d.dishId) {
                            jg->stoplist = jdoc["stoplist"].toDouble();
                        }
                    }
                }

                parseOrder(jdoc);
            },
            [](const QJsonObject &jerr) {});
        return;
    }

    if (!d.emarks().isEmpty()) {
        C5Message::error(tr("The quantity of dishes with remarks cannot be changed"));
        return;
    }

    fHttp->createHttpQueryLambda("/engine/v2/waiter/order/set-dish-qty", {
        {"id", d.id},
        {"remove_emarks", false},
        {"dish", d.dishId},
        {"dish_name", d.dishName},
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

    if (!mSelectedPackage.isEmpty()) {
        if (!isGoodsLikeType(g->type)) {
            C5Message::error(tr("Only goods can be appended in package"));
            return;
        }
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
        fHttp->createHttpQueryLambda(
            "/engine/v2/waiter/order/add-dish",
            {{"dish", g->id},
             {"dish_name", g->name},
             {"order_id", mOrder.id},
             {"is_preorder", mCreateAsPreorder ? 1 : 0},
             {"parent", mSelectedPackage},
             {"table", mTable.id},
             {"qty", 1},
             {"type", g->type},
             {"cashbox_id", mWorkStation.cashboxId()},
             {"row", mOrder.dishes.count() * 100},
             {"price", g->price},
             {"count_service", g->countService()},
             {"count_discount", g->countDiscount()},
             {"f_data", g->data},
             {"store", g->store},
             {"print1", g->print1},
             {"print2", g->print2},
             {"empty_order", mOrder.dishes.empty()},
             {"service_factor", mHall.serviceFactor()},
             {"create_process", mWorkStation.customerNotification()}},
            [this, g, btn](const QJsonObject &jdoc) {
                if (jdoc.contains("stoplist")) {
                    for (auto *jg : *mDishes) {
                        if (jg->id == g->id) {
                            jg->stoplist = jdoc["stoplist"].toDouble();
                        }
                    }
                } else {
                    g->stoplist = -1;
                }

                if (btn) {
                    btn->update();
                }

                parseOrder(jdoc);
                scrollOrderToBottom();
            },
            [](const QJsonObject &jerr) {});
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
    if (!(fTimerCounter % 60)) {
        QPointer<DlgOrder> self(this);
        auto updateAmounts = [self](const QString &bearer) {
            if (self->mOrder.id.isEmpty()) {
                return;
            }
            bool doNotCheck = true;
            for (auto a : self->mOrder.dishes) {
                if (a.isHourlyPayment()) {
                    doNotCheck = false;
                    break;
                }
            }
            if (doNotCheck) {
                return;
            }
            NInterface::query(
                "/engine/v2/waiter/order/update-amounts",
                bearer,
                self,
                {
                    {"id", self->mOrder.id},
                },
                [self](const QJsonObject &jdoc) { self->parseOrder(jdoc); },
                [](const QJsonObject &jerr) { return false; });
        };

        updateAmounts(self->mUser->mSessionKey);
    }
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

    const bool lockByPrecheck = mOrder.isPrecheckPrinted() && mOrder.state != ORDER_STATE_PREORDER;
    if(lockByPrecheck) {
        C5Message::error(tr("Order is not editable"));
        return;
    }

    WaiterDish d = mOrder.dishes.at(index);

    if(d.state != DISH_STATE_OK) {
        return;
    }

    if(d.type == GOODS_TYPE_PACKAGE) {
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
            auto removePrintedPackageFunc = [self, d, result, reason](const QString & bearer) {
                NInterface::query("/engine/v2/waiter/order/remove-package-with-children", bearer, self, {
                    {"id", d.id},
                    {"order_id", self->mOrder.id},
                    {"dish", d.dishId},
                    {"dish_name", d.dishName},
                    {"new_state", result},
                    {"data", d.data},
                    {"new_qty", d.qty},
                    {"restore_stoplist", d.qty},
                    {"remove_emarks", !d.emarks().isEmpty()},
                    {"remove_reason", reason}
                },
                [self](const QJsonObject & jdoc) {
                    const QJsonArray ja = jdoc["removed_dishes"].toArray();

                    for(int i = 0; i < ja.size(); ++i) {
                        self->printRemovedDish(ja.at(i).toObject());
                    }

                    self->parseOrder(jdoc);
                }, [](const QJsonObject & jerr) {
                    return false;
                });
            };

            if(mUser->check(cp_t5_waiter_remove_printed_goods)) {
                removePrintedPackageFunc(mUser->mSessionKey);
                return;
            }

            QString pin;

            if(!DlgPassword::getPasswordString(tr("Remove printed dish"), pin)) {
                return;
            }

            auto *user = new C5User();
            user->authorize(pin, self->fHttp, [self, removePrintedPackageFunc, user](const QJsonObject & jdoc) {
                if(user->check(cp_t5_waiter_remove_printed_goods)) {
                    removePrintedPackageFunc(user->mSessionKey);
                } else {
                    C5Message::error(tr("Permission denied"));
                }

                user->deleteLater();
            }, [user]() {
                user->deleteLater();
            });
            return;
        }

        d.state = 0;
        fHttp->createHttpQueryLambda(
            "/engine/v2/waiter/order/remove-package-with-children",
            {{"id", d.id},
             {"order_id", mOrder.id},
             {"dish", d.dishId},
             {"dish_name", d.dishName},
             {"new_qty", 0},
             {"new_state", d.state},
             {"data", d.data},
             {"remove_emarks", !d.emarks().isEmpty()},
             {"restore_stoplist", d.qty}},
            [this](const QJsonObject &jdoc) { parseOrder(jdoc); },
            [](const QJsonObject &jerr) {});
        return;
    }

    if(!isGoodsLikeType(d.type) && d.type != GOODS_TYPE_MODIFICATOR) {
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
                {"dish_name", d.dishName},
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
        fHttp->createHttpQueryLambda(
            "/engine/v2/waiter/order/set-dish-qty",
            {{"id", d.id},
             {"remove_emarks", !d.emarks().isEmpty()},
             {"dish", d.dishId},
             {"dish_name", d.dishName},
             {"new_qty", 0},
             {"new_state", d.state},
             {"data", d.data},
             {"order_id", mOrder.id},
             {"restore_stoplist", d.qty}},
            [this](const QJsonObject &jdoc) { parseOrder(jdoc); },
            [](const QJsonObject &jerr) {});
    }
}

void DlgOrder::on_btnComment_clicked()
{
    int index = selectedWaiterDishIndex();
    bool lastItem = index == mOrder.dishes.size() - 1;

    if(index < 0) {
        return;
    }

    WaiterDish d = mOrder.dishes.at(index);

    if(!isGoodsLikeType(d.type)) {
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
            [self, lastItem](const QJsonObject & jdoc) {
                self->parseOrder(jdoc);

                if(lastItem) {
                    self->scrollOrderToBottom();
                }
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
    if(mOrder.id.isEmpty()) {
        return;
    }

    const QString route = mOrder.state == ORDER_STATE_PREORDER
                              ? "/engine/v2/waiter/order/print-service-of-preorder"
                              : "/engine/v2/waiter/order/print-service-check";
    fHttp->createHttpQueryLambda(route, {{"header_id", mOrder.id}}, [this](const QJsonObject & jdoc) {
        parseOrder(jdoc);
        if(isBistroMode() && !jdoc.value(QStringLiteral("reprint")).toBool()) {
            applyBistroPrecheckAfterService();
        }
        printService(jdoc);
    }, [](const QJsonObject & jerr) {});
}

void DlgOrder::on_btnSit_clicked()
{
    DlgGuests dg(mUser);
    dg.setGuests(mOrder.data.value("f_guests_count").toInt());
    if (dg.exec() == QDialog::Accepted) {
        NInterface::query1("/engine/v2/waiter/order/set-data-value",
                           mUser->mSessionKey,
                           this,
                           {{"id", mOrder.id}, {"key", "f_guests_count"}, {"value", dg.guests()}},
                           [this](const QJsonObject &jdoc) { parseOrder(jdoc); });
    }
}

void DlgOrder::on_btnPrepaid_clicked()
{
    if(mOrder.id.isEmpty()) {
        return;
    }

    DlgMoveMoney d(mUser);
    d.setMode(3);
    if(d.exec() != QDialog::Accepted) {
        return;
    }

    NInterface::query1("/engine/v2/waiter/order/set-data-value",
                       mUser->mSessionKey,
                       this,
                       {{"id", mOrder.id}, {"key", "f_deposit_prepaid"}, {"value", d.amount()}},
                       [this](const QJsonObject &jdoc) { parseOrder(jdoc); });
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
    if (mOrder.id.isEmpty()) {
        C5Message::error(tr("Order was not opened"));
        return;
    }

    QPointer<DlgOrder> self(this);
    auto func = [self](C5User *user) {
        if (!self || self->mOrder.id.isEmpty()) {
            return;
        }

        NInterface::query1("/engine/v2/waiter/order/get-discount-values", user->mSessionKey, self, {}, [self, user](const QJsonObject &jdoc) {
            if (!self || self->mOrder.id.isEmpty()) {
                return;
            }

            QJsonArray jvals = jdoc["values"].toArray();
            QStringList titles;
            QStringList comments;
            QList<double> values;
            QList<int> indexes;

            for (int i = 0; i < jvals.size(); i++) {
                const QJsonObject &jo = jvals.at(i).toObject();
                const double discountFactor = qAbs(jo["f_value"].toDouble());
                const QString comment = jo["f_name"].toString().trimmed() + " " + float_str(discountFactor * 100, 2) + "%";
                titles.append(comment);
                comments.append(comment);
                values.append(discountFactor);
                indexes.append(i + 1);
            }

            DlgSimleOptions dso(titles, indexes);
            int index = dso.exec();

            if (index == 0) {
                return;
            }

            const double newDiscountFactor = values.at(index - 1);
            NInterface::query1("/engine/v2/waiter/order/change-discount-value",
                               user->mSessionKey,
                               self,
                               {{"id", self->mOrder.id}, {"value", newDiscountFactor}, {"comment", comments.at(index - 1)}},
                               [self](const QJsonObject &jdoc1) {
                                   if (!self) {
                                       return;
                                   }

                                   self->parseOrder(jdoc1);
                               });
        });
    };

    if (mUser->check(cp_t5_waiter_change_service_factor)) {
        func(mUser);
        return;
    }

    QString pin;

    if (!DlgPassword::getPasswordString(tr("Remove printed dish"), pin)) {
        return;
    }
    auto *user = new C5User();
    user->authorize(
        pin,
        self->fHttp,
        [self, func, user](const QJsonObject &jdoc) {
            if (user->check(cp_t5_waiter_change_service_factor)) {
                func(user);
            } else {
                C5Message::error(tr("Permission denied"));
            }

            user->deleteLater();
        },
        [user]() { user->deleteLater(); });
    return;
}

void DlgOrder::on_btnTotal_clicked()
{
    if(mOrder.state != ORDER_STATE_PREORDER && !mOrder.isReadyForPrecheck()) {
        C5Message::error(tr("Print service check before payment or precheck"));
        return;
    }

    QPointer<DlgOrder> self(this);
    auto repeatPrecheckFunc = [self](const QString & bearer, const QString & currentUserName) {
        if(!self) {
            return;
        }

        const QString route = self->mOrder.state == ORDER_STATE_PREORDER
                                  ? "/engine/v2/waiter/order/print-precheck-of-preorder"
                                  : "/engine/v2/waiter/order/print-precheck";
        NInterface::prepareLoadingTitle(
            QObject::tr("Printer: %1").arg(mWorkStation.precheckPrinter()));
        self->fHttp->query(route,
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

                    if(self->isBistroMode()) {
                        self->resetBistroOrderUi();
                        self->syncOrderDishWidgetsVisibility();
                    }
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
    const QJsonObject &jo = mWorkStation.data.value("setup_buttons").toObject();

    for(auto pt : std::as_const(payment_types)) {
        auto *btn = new QToolButton(this);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setMinimumHeight(50);
        btn->setText(QCoreApplication::translate("PaymentType", payment_names[pt]));
        btn->setEnabled(payment_special[pt] ? mUser->check(cp_t5_waiter_special_payment_types) : true);
        const bool disabled = jo.value(QString("payment_button_%1").arg(pt)).toBool(false);
        btn->setEnabled(btn->isEnabled() && !disabled);
        connect(btn, &QToolButton::clicked, this, [self, pt]() {
            if (self->ui->lbAmount->property("amount").toDouble() < 0.01) {
                return;
            }

            if(self->mOrder.state != ORDER_STATE_PREORDER && !self->mOrder.isReadyForPrecheck()) {
                C5Message::error(tr("Print service check before payment or precheck"));
                return;
            }

            NInterface::query(
                "/engine/v2/waiter/order/set-amount",
                self->mUser->mSessionKey,
                self,
                {{"id", self->mOrder.id},
                 {"payment_field", payment_fields[pt]},
                 {"amount", self->ui->lbAmount->property("amount").toDouble()}},
                [self](const QJsonObject &jdoc) {
                    if (!self) {
                        return;
                    }

                    self->on_btnNumClear_clicked();
                    self->parseOrder(jdoc);
                },
                [](const QJsonObject &jerr) { return false; });
        });
        ui->glPaymentType->addWidget(btn, row, col);
        col++;

        if (col == 2) {
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

void DlgOrder::configCashTenderButtons()
{
    const QVector<QToolButton *> buttons = {
        ui->btnCash1,
        ui->btnCash2,
        ui->btnCash3,
        ui->btnCash4,
    };
    for (QToolButton *b : buttons) {
        connect(b, &QToolButton::clicked, this, [this, b]() {
            const double value = b->property("amount").toDouble();
            if (value < 0.01) {
                return;
            }
            ui->lbAmount->setProperty("amount", value);
            ui->lbAmount->setProperty("str", QString::number(value, 'f', 0));
            ui->lbAmount->setText(QString("%1 %2").arg(float_str(value, 2), CURRENCY_SHORT));
        });
    }
    updateCashTenderButtons();
}

QList<int> DlgOrder::cashTenderSuggestions(double due) const
{
    static const int denoms[] = {1000, 2000, 5000, 10000, 20000};
    const int dueInt = qMax(0, static_cast<int>(std::ceil(due - 1e-9)));
    if (dueInt <= 0) {
        return {};
    }

    QSet<int> candidates;
    for (int b : denoms) {
        const int n = ((dueInt + b - 1) / b) * b;
        if (n >= dueInt) {
            candidates.insert(n);
        }
    }
    const int base = ((dueInt + 999) / 1000) * 1000;
    candidates.insert(base + 1000);

    QList<int> sorted = candidates.values();
    std::sort(sorted.begin(), sorted.end());

    QList<int> result;
    for (int v : sorted) {
        if (v >= dueInt) {
            result.append(v);
            if (result.size() == 4) {
                return result;
            }
        }
    }

    int last = result.isEmpty() ? base : result.last();
    while (result.size() < 4) {
        last += 10000;
        if (!result.contains(last)) {
            result.append(last);
        }
    }
    return result;
}

double DlgOrder::paymentRemainDue() const
{
    double remain = orderDisplayTotalDue();
    for (auto pt : payment_types) {
        remain -= mOrder.payment(payment_fields[pt]);
    }
    return remain;
}

void DlgOrder::updateCashTenderButtons()
{
    const QList<int> suggestions = cashTenderSuggestions(paymentRemainDue());
    const QVector<QToolButton *> buttons = {
        ui->btnCash1,
        ui->btnCash2,
        ui->btnCash3,
        ui->btnCash4,
    };
    for (int i = 0; i < buttons.size(); ++i) {
        QToolButton *b = buttons.at(i);
        if (i < suggestions.size()) {
            const int amount = suggestions.at(i);
            b->setProperty("amount", amount);
            b->setText(QString::number(amount));
            b->setEnabled(true);
        } else {
            b->setProperty("amount", 0);
            b->setText(QString());
            b->setEnabled(false);
        }
    }
}

void DlgOrder::configOtherButtons()
{
    ui->btnTotal->setEnabled(mUser->check(cp_t5_waiter_print_precheck));
    ui->btnCloseOrder->setEnabled(mUser->check(cp_t5_waiter_close_order));
}

int DlgOrder::selectedWaiterDishIndex()
{
    QString focusedId;

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
            if(w->isFocused()) {
                focusedId = w->mOrderItem.id;
                break;
            }
        }
    }

    if(focusedId.isEmpty()) {
        return -1;
    }

    for(int j = 0; j < mOrder.dishes.size(); ++j) {
        if(mOrder.dishes.at(j).id == focusedId) {
            return j;
        }
    }

    return -1;
}

void DlgOrder::syncQtyPanelButtonsEnabled(bool enabled)
{
    if(!enabled) {
        return;
    }

    /* setupButtons() can leave qty children disabled after wqty was off during precheck. */
    for(QAbstractButton *b : ui->wqtypanelup->findChildren<QAbstractButton*>()) {
        b->setEnabled(true);
        setupButton(b);
    }
}
WaiterOrderItemWidget* DlgOrder::createOrderItemWidget(WaiterDish d)
{
    switch(d.type) {
    case GOODS_TYPE_GOODS:
    case GOODS_TYPE_DISH:
        return new WaiterDishWidget(d, mShowRemoved);

    default:
        return new WaiterDishWidget(d, mShowRemoved);
    }
}

void DlgOrder::scrollOrderToBottom()
{
    QTimer::singleShot(0, this, [this]() {
        auto sb = ui->orderScrollArea->verticalScrollBar();
        sb->setValue(sb->maximum());
    });
}

void DlgOrder::parseOrder(const QJsonObject & jdoc)
{
    ui->orderScrollArea->setUpdatesEnabled(false);
    QString selectedId;
    mOrder = JsonParser<WaiterOrder>::fromJson(jdoc["order"].toObject());
    fillPackageNominalDeltas(mOrder.dishes);
    const bool lockByPrecheck = mOrder.isPrecheckPrinted() && mOrder.state != ORDER_STATE_PREORDER;
    // Empty/new table can come with state=0 and no id; keep menu editable for first dish append.
    const bool editableState = mOrder.id.isEmpty()
                               || mOrder.state == ORDER_STATE_OPEN
                               || mOrder.state == ORDER_STATE_PREORDER;
    ui->lbTableName->setText(mOrder.tableName);
    ui->lbOrderNumber->setText(mOrder.receiptNumber);
    ui->lbOrderComment->setVisible(!mOrder.comment().isEmpty());
    ui->lbOrderComment->setText(mOrder.comment());
    ui->wgroups->setVisible(!lockByPrecheck);
    bool dishesVisible = ui->wdishes->isVisible();
    bool newDishesVisible = !lockByPrecheck;

    if(dishesVisible != newDishesVisible) {
        ui->wdishes->setVisible(newDishesVisible);
    }

    //dishes
    for(int i = 0 ; i <  mOrder.dishes.size(); i++) {
        WaiterOrderItemWidget *ow = nullptr;
        WaiterDish w = mOrder.dishes.at(i);

        //2 - becouse we created streach in the showEvent
        if(i > ui->vlDishes->count() - 2) {
            ow = createOrderItemWidget(w);
            ui->vlDishes->insertWidget(i, ow);
        } else {
            const int layCount = ui->vlDishes->count();

            if(i < 0 || i >= layCount) {
                ow = createOrderItemWidget(w);
                ui->vlDishes->insertWidget(i, ow);
            } else {
                QLayoutItem *li = ui->vlDishes->itemAt(i);
                QWidget *lw = li ? li->widget() : nullptr;

                if(li && lw) {
                    if(auto *wf = qobject_cast<WaiterOrderItemWidget *>(lw)) {
                        if(wf->isFocused()) {
                            selectedId = wf->mOrderItem.id;
                        }
                    }

                    //todo visnet esli klikat bistro na dobavlenie pateta
                    switch(w.type) {
                    case GOODS_TYPE_GOODS:
                    case GOODS_TYPE_DISH:
                    case GOODS_TYPE_SERVICE:
                    case GOODS_TYPE_UNKNOWN:
                        if(auto *dwGoods = qobject_cast<WaiterDishWidget *>(lw)) {
                            if(dwGoods->mOrderItem.id != w.id) {
                                QLayoutItem *taken = ui->vlDishes->takeAt(i);

                                if(taken) {
                                    if(QWidget *oldw = taken->widget()) {
                                        oldw->deleteLater();
                                    }

                                    delete taken;
                                }

                                ow = createOrderItemWidget(w);
                                ui->vlDishes->insertWidget(i, ow);
                            } else {
                                dwGoods->mShowRemoved = mShowRemoved;
                                ow = dwGoods;
                            }
                        }

                        break;

                    case GOODS_TYPE_MODIFICATOR:
                        ow = qobject_cast<WaiterModificatorWidget *>(lw);
                        break;

                    case GOODS_TYPE_GUEST:
                        ow = qobject_cast<WaiterGuestWidget *>(lw);
                        break;

                    case GOODS_TYPE_PACKAGE:
                        if(auto *dwPkg = qobject_cast<WaiterDishWidget *>(lw)) {
                            if(dwPkg->mOrderItem.id != w.id) {
                                QLayoutItem *taken = ui->vlDishes->takeAt(i);

                                if(taken) {
                                    if(QWidget *oldw = taken->widget()) {
                                        oldw->deleteLater();
                                    }

                                    delete taken;
                                }

                                ow = createOrderItemWidget(w);
                                ui->vlDishes->insertWidget(i, ow);
                            } else {
                                dwPkg->mShowRemoved = mShowRemoved;
                                ow = dwPkg;
                            }
                        }

                        break;

                    default:
                        break;
                    }

                    if(!ow) {
                        /* must takeAt — deleting itemAt()'s pointer corrupts the layout */
                        QLayoutItem *taken = ui->vlDishes->takeAt(i);

                        if(taken) {
                            if(QWidget *oldw = taken->widget()) {
                                oldw->deleteLater();
                            }

                            delete taken;
                        }

                        ow = createOrderItemWidget(w);
                        ui->vlDishes->insertWidget(i, ow);
                    }
                } else {
                    ow = createOrderItemWidget(w);
                    ui->vlDishes->insertWidget(i, ow);
                }
            }
        }

        if(auto *dw = qobject_cast<WaiterDishWidget *>(ow)) {
            dw->setDisplayContext(isBistroMode(), mOrder.state == ORDER_STATE_PREORDER,
                                  mOrder.serviceFactor(), mOrder.discountFactor());
        }

        ow->updateDish(w);
        ow->setFocused(ow->mOrderItem.id == selectedId);
        connect(ow, &WaiterDishWidget::focused,
                this, &DlgOrder::handleOrderDishClick,
                Qt::UniqueConnection);
        connect(this, &DlgOrder::orderDishClicked,
                ow, &WaiterDishWidget::checkFocus,
                Qt::UniqueConnection);
        if (w.isHourlyPayment()) {
            if (auto *dishWidget = qobject_cast<WaiterDishWidget *>(ow)) {
                if (w.isPlaying()) {
                    connect(dishWidget, &WaiterDishWidget::stopPlay, this, [=]() {
                        NInterface::query1("/engine/v2/waiter/order/stop-play",
                                           mUser->mSessionKey,
                                           this,
                                           {{"id", dishWidget->mOrderItem.id}},
                                           [=](const QJsonObject &jdoc) { parseOrder(jdoc); });
                    });
                }
                connect(dishWidget, &WaiterDishWidget::setEndDate, this, [=](QDateTime dt) {
                    NInterface::query1("/engine/v2/waiter/order/set-end-datetime",
                                       mUser->mSessionKey,
                                       this,
                                       {{"id", dishWidget->mOrderItem.id}, {"datetime", dt.toString(FORMAT_DATETIME_TO_STR_MYSQL)}},
                                       [=](const QJsonObject &jdoc) { parseOrder(jdoc); });
                });
            }
        }

        if(w.type == GOODS_TYPE_PACKAGE) {
            if(auto *dw = qobject_cast<WaiterDishWidget *>(ow)) {
                connect(dw, &WaiterDishWidget::packageParentToggled, this, &DlgOrder::onPackageFillParentToggled, Qt::UniqueConnection);
            }
        }
    }

    removeStaleOrderDishRows();
    syncOrderDishWidgetsVisibility();
    syncPackageParentButtons();

    /* PAYMENT BUTTON */
    if(mOrder.state == ORDER_STATE_OPEN || mOrder.id.isEmpty()) {
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
        const bool autoFiscal = mWorkStation.data.value(QStringLiteral("f_auto_fiscal")).toBool();
        bool forceAutoFiscal = false;

        for(auto pt : payment_types) {
            double rowAmount = mOrder.payment(payment_fields[pt]);

            /* Депозит в f_deposit_prepaid до строки f_amount_prepaid — показываем кнопку «Предоплата» после пречека. */
            if(pt == PAYMENT_TYPE_PREPAID && rowAmount <= 0.001) {
                rowAmount = mOrder.prepaidAmount();
            }

            if(rowAmount <= 0.001) {
                continue;
            }

            if(autoFiscal
                    && (pt == PAYMENT_TYPE_CASH
                        || pt == PAYMENT_TYPE_CARD
                        || pt == PAYMENT_TYPE_IDRAM
                        || pt == PAYMENT_TYPE_TELCELL)) {
                forceAutoFiscal = true;
            } else if(!autoFiscal && pt == PAYMENT_TYPE_CARD) {
                ui->btnPrintFiscal->setChecked(true);
            }

            auto *b = new QToolButton(self);
            b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            b->setMinimumHeight(50);
            b->setText(QString("%1 %2 %3").arg(QCoreApplication::translate("PaymentType", payment_names[pt]), float_str(rowAmount, 2), CURRENCY_SHORT));
            b->setProperty("method", pt);
            b->setProperty("amount", rowAmount);
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

        if(forceAutoFiscal) {
            ui->btnPrintFiscal->setChecked(true);
            ui->btnPrintFiscal->setEnabled(false);
        } else {
            ui->btnPrintFiscal->setEnabled(true);
        }

        updateCashTenderButtons();
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
    {
        const bool kitchenReady = mOrder.state == ORDER_STATE_PREORDER || mOrder.isReadyForPrecheck();
        ui->wpayment->setVisible(lockByPrecheck && editableState && kitchenReady);
    }
    ui->wmenua->setVisible(!lockByPrecheck && editableState);
    ui->wappmenu->setEnabled(!lockByPrecheck);
    ui->wqty->setEnabled(!lockByPrecheck && editableState);
    ui->btnService->setEnabled(!lockByPrecheck && editableState);
    ui->btnDiscount->setEnabled(!lockByPrecheck && editableState);
    {
        const QString serviceComment = mOrder.data.value(QStringLiteral("f_service_comment")).toString().trimmed();
        const QString serviceTitle = serviceComment.isEmpty() ? tr("Service") : serviceComment;
        ui->btnService->setText(QString("%1\n%2%").arg(serviceTitle, float_str(mOrder.serviceFactor() * 100, 2)));
    }
    {
        const QString discountComment = mOrder.data.value(QStringLiteral("f_discount_comment")).toString().trimmed();
        const QString discountTitle = discountComment.isEmpty() ? tr("Discount") : discountComment;
        ui->btnDiscount->setText(QString("%1\n%2%").arg(discountTitle, float_str(mOrder.discountFactor() * 100, 2)));
    }
    ui->btnTotal->setText(QString("%1\n%2 %3").arg(tr("Precheck"), float_str(orderDisplayTotalDue(), 2), "դր․"));
    ui->btnSit->setText(QString::number(mOrder.data.value("f_guests_count").toInt()));
    const double prepaidAmount = mOrder.prepaidAmount();
    ui->btnPrepaid->setText(prepaidAmount > 0.001
                                ? QString("%1 %2").arg(float_str(prepaidAmount, 2), CURRENCY_SHORT)
                                : tr("Prepaid"));
    const double deliveryAmount = mOrder.data.value("f_delivery_amount").toDouble();
    ui->btnDeliveryAmount->setText(deliveryAmount > 0.001
                                       ? QString("%1 %2").arg(float_str(deliveryAmount, 2), CURRENCY_SHORT)
                                       : tr("Delivery\namount"));
    {
        const bool preorder = (mOrder.state == ORDER_STATE_PREORDER);
        ui->btnPreorderDateTime->setVisible(preorder);

        if(preorder) {
            const QString raw = mOrder.data.value(QStringLiteral("f_preorder_datetime")).toString().trimmed();
            const QDateTime pdt = QDateTime::fromString(raw, FORMAT_DATETIME_TO_STR_MYSQL);

            ui->btnPreorderDateTime->setText(pdt.isValid() ? pdt.toString(QStringLiteral("dd.MM.yyyy\nHH:mm")) : tr("Preorder datetime"));
        }
    }
    updateScrollButtonPositions();

    if(jdoc.contains("focused_dish")) {
        emit orderDishClicked(jdoc["focused_dish"].toString());
    }

    ui->lbSubtotal->setText(QString("%1 %2").arg(float_str(mOrder.subTotal(), 2), CURRENCY_SHORT));
    ui->lbServiceFeeName->setText(QString("%1 %2%").arg(tr("Service fee"), float_str(mOrder.serviceFactor() * 100, 2)));
    ui->lbServiceFee->setText(QString("%1 %2").arg(float_str(mOrder.serviceAmount(), 2), CURRENCY_SHORT));
    ui->lbDiscountFeeName->setText(QString("%1 %2%").arg(tr("Discount fee"), float_str(mOrder.discountFactor() * 100, 2)));
    ui->lbDiscount->setText(QString("%1 %2").arg(float_str(mOrder.discountAmount(), 2), CURRENCY_SHORT));
    ui->lbTotalDue->setText(QString("%1 %2").arg(float_str(orderDisplayTotalDue(), 2), CURRENCY_SHORT));
    QDateTime startQuery = QDateTime::fromString(jdoc["query_start"].toString(), "yyyy-MM-dd HH:mm:ss.zzz");
    QJsonObject jtax = mOrder.fiscal();

    if(mOrder.state == ORDER_STATE_CLOSE) {
        ui->btnPrintClosedFiscal->setEnabled(jtax.isEmpty());
    }

    ui->btnPrintService->setEnabled(!mOrder.id.isEmpty());
    ui->btnPreorderDateTime->setEnabled(!mOrder.id.isEmpty() && mOrder.state == ORDER_STATE_PREORDER);
    ui->btnActivatePreorder->setVisible(mOrder.state == ORDER_STATE_PREORDER);
    ui->btnActivatePreorder->setEnabled(!mOrder.id.isEmpty() && mOrder.state == ORDER_STATE_PREORDER);
    ui->btnTransferDishes->setEnabled(!mOrder.id.isEmpty());
    ui->btnTransferTable->setEnabled(!mOrder.id.isEmpty());
    const QJsonObject &jg = mOrder.data.value("f_guest").toObject();
    QString guestName = jg.value("f_guest_name").toString();
    ui->btnGuest->setText(guestName.isEmpty() ? tr("Guest info") : guestName);
    updateDatamatrixButton();
    qDebug() << "Parse order" << startQuery.msecsTo(QDateTime::currentDateTime());
    ui->orderScrollArea->setUpdatesEnabled(true);
    qApp->processEvents();
    updateCustomerDisplay();
    setupButtons();
    syncQtyPanelButtonsEnabled(!lockByPrecheck && editableState);
}

void DlgOrder::handleOrderDishClick(const QString & id)
{
    emit(orderDishClicked(id));
    // Keep keyboard focus on the dialog so numpad +/- / barcode buffer work.
    setFocus(Qt::OtherFocusReason);
}

void DlgOrder::onPackageFillParentToggled(const QString &waiterLineId, bool checked)
{
    if(checked) {
        mSelectedPackage = waiterLineId;
    } else if(mSelectedPackage == waiterLineId) {
        mSelectedPackage.clear();
    }

    syncPackageParentButtons();
}

void DlgOrder::syncPackageParentButtons()
{
    if(!mSelectedPackage.isEmpty()) {
        bool stillValid = false;

        for(int i = 0, c = ui->vlDishes->count(); i < c; ++i) {
            QLayoutItem *li = ui->vlDishes->itemAt(i);

            if(!li || !li->widget()) {
                continue;
            }

            auto *dw = qobject_cast<WaiterDishWidget *>(li->widget());

            if(dw && dw->mOrderItem.type == GOODS_TYPE_PACKAGE
               && dw->mOrderItem.state == DISH_STATE_OK
               && dw->mOrderItem.id == mSelectedPackage) {
                stillValid = true;
                break;
            }
        }

        if(!stillValid) {
            mSelectedPackage.clear();
        }
    }

    for(int i = 0, c = ui->vlDishes->count(); i < c; ++i) {
        QLayoutItem *li = ui->vlDishes->itemAt(i);

        if(!li || !li->widget()) {
            continue;
        }

        auto *dw = qobject_cast<WaiterDishWidget *>(li->widget());

        if(!dw || dw->mOrderItem.type != GOODS_TYPE_PACKAGE) {
            continue;
        }

        const bool on = !mSelectedPackage.isEmpty() && dw->mOrderItem.id == mSelectedPackage;
        dw->setPackageParentButtonChecked(on);
    }
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


void DlgOrder::on_btnCloseOrder_clicked()
{
    if(mOrder.state != ORDER_STATE_PREORDER && !mOrder.isReadyForPrecheck()) {
        C5Message::error(tr("Print service check before payment or precheck"));
        return;
    }

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

        NInterface::prepareLoadingTitle(
            QObject::tr("Printer: %1").arg(mWorkStation.precheckPrinter()));
        NInterface::query(
            "/engine/v2/waiter/order/close-order",
            self->mUser->mSessionKey,
            self,
            {{"id", self->mOrder.id},
             {"fiscal", fiscalInfo},
             {"cashbox_id", mWorkStation.cashboxId()},
             {"cost_depend_on_service_and_discount", mWorkStation.data.value(QStringLiteral("cost_depend_on_service_and_discount")).toBool()},
             {"cash_session_id", self->mOrder.cashSessionId}},
            [self](const QJsonObject &jdoc) {
                self->parseOrder(jdoc);
                self->printPrecheck(self->mUser->shortFullName());

                if(self->isBistroMode()) {
                    self->startNewOrderOnTableAfterBistroClose();
                    return;
                }

                self->accept();
            },
            [](const QJsonObject &jerr) { return false; });
    };

    if(ui->btnPrintFiscal->isChecked()) {
        const FiscalMachine fm = fiscalMachineForWorkstation(mWorkStation);

        if(fm.id == 0) {
            C5Message::error(tr("Fiscal machine is not configured for this workstation."));
            return;
        }

        auto *loading = new NLoadingDlg(tr("Printing fiscal check"), this);
        QPointer<NLoadingDlg> loadingPtr(loading);
        auto *thread = new QThread();
        auto *pt = new PrintTaxN(fm.ip, fm.port, fm.machinePassword, fm.externalPosString(), fm.opPin, fm.opPassword);
        pt->moveToThread(thread);
        const WaiterOrder order = self->mOrder;
        const double serviceFactor = order.serviceFactor();
        const QString sessionKey = mUser->mSessionKey;
        auto fiscalHandled = std::make_shared<bool>(false);

        connect(pt, &PrintTaxN::finished, pt, &QObject::deleteLater);
        connect(pt, &PrintTaxN::finished, thread, &QThread::quit);
        connect(thread, &QThread::started, pt, [pt, order, serviceFactor]() {
            for(const WaiterDish &dish : order.dishes) {
                if(dish.state != DISH_STATE_OK) {
                    continue;
                }

                WaiterDish d = dish;
                d.data["f_service_factor"] = serviceFactor;
                pt->addGoods(d.fiscalDepartment(),
                             d.adgtCode(),
                             QString::number(d.dishId),
                             d.dishName,
                             d.total(false) / d.qty,
                             d.qty,
                             d.discountFactor() * 100);
            }

            const FiscalPaymentPayload pay = fiscalPaymentsFromOrder(order);
            applyFiscalPayments(pt, pay);
            pt->fEmarks = collectFiscalEmarks(order);
            pt->makeJsonAndPrint(pay.cash, pay.nonCash, pay.prepaid);
        });
        connect(pt, &PrintTaxN::started, loading, &QDialog::show, Qt::QueuedConnection);
        connect(pt, &PrintTaxN::finished, self, [self, loadingPtr, closeOrderFunc, sessionKey, fiscalHandled](
                    const QString &inJson, const QString &outJson, const QString &err, int result) {
            if(!self || *fiscalHandled) {
                return;
            }

            *fiscalHandled = true;

            if(loadingPtr) {
                loadingPtr->close();
                loadingPtr->deleteLater();
            }

            QJsonObject reply{{"in", QJsonDocument::fromJson(inJson.toUtf8()).object()},
                              {"out", QJsonDocument::fromJson(outJson.toUtf8()).object()},
                              {"error", err},
                              {"result", result},
                              {"f_fiscal_machine_id", mWorkStation.fiscalMachineId()}};

            if(result == 0) {
                closeOrderFunc(reply);
            } else {
                reply.insert("id", self->mOrder.id);
                NInterface::query("/engine/v2/waiter/order/fiscal-log", sessionKey, self, reply,
                                  [](const QJsonObject &) {},
                                  [](const QJsonObject &) { return false; });
                C5Message::error(err);
            }
        }, Qt::QueuedConnection);
        connect(thread, &QThread::finished, thread, &QObject::deleteLater);
        thread->start();
    } else {
        closeOrderFunc({});
    }
}

void DlgOrder::on_btnService_clicked()
{
    if(mOrder.id.isEmpty()) {
        C5Message::error(tr("Order was not opened"));
        return;
    }

    QPointer<DlgOrder> self(this);
    auto func = [self](C5User * user) {
        if(!self || self->mOrder.id.isEmpty()) {
            return;
        }

        NInterface::query1("/engine/v2/waiter/order/get-service-values", user->mSessionKey, self, {},
        [self, user](const QJsonObject & jdoc) {
            if(!self || self->mOrder.id.isEmpty()) {
                return;
            }

            QJsonArray jvals = jdoc["values"].toArray();
            QStringList titles;
            QStringList comments;
            QList<double> values;
            QList<int> indexes;

            for(int i = 0; i < jvals.size(); i++) {
                const QJsonObject &jo = jvals.at(i).toObject();
                const QString comment = jo["f_name"].toString().trimmed() + " " + float_str(jo["f_value"].toDouble() * 100, 2) + "%";
                titles.append(comment);
                comments.append(comment);
                values.append(jo["f_value"].toDouble());
                indexes.append(i + 1);
            }

            DlgSimleOptions dso(titles, indexes);
            int index = dso.exec();

            if(index == 0) {
                return;
            }

            double newServiceFactor = values.at(index - 1);
            NInterface::query1("/engine/v2/waiter/order/change-service-value",
                               user->mSessionKey,
                               self,
                               {{"id", self->mOrder.id}, {"value", newServiceFactor}, {"comment", comments.at(index - 1)}},
                               [self](const QJsonObject &jdoc1) {
                                   if (!self) {
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

    QString pin;

    if(!DlgPassword::getPasswordString(tr("Remove printed dish"), pin)) {
        return;
    }
    auto *user = new C5User();
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
    setDishQty([](WaiterDish d) {
        return d.isPrinted() ? 1 : d.qty + 1;
    });
}

void DlgOrder::on_btnMinus1_clicked()
{
    int index = selectedWaiterDishIndex();

    if(index < 0) {
        return;
    }

    WaiterDish d = mOrder.dishes.at(index);

    if(!isGoodsLikeType(d.type) && d.type != GOODS_TYPE_MODIFICATOR) {
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

        fHttp->createHttpQueryLambda(
            "/engine/v2/waiter/order/set-dish-qty",
            {{"id", d.id},
             {"remove_emarks", !d.emarks().isEmpty()},
             {"dish", d.dishId},
             {"dish_name", d.dishName},
             {"new_qty", newQty},
             {"new_state", d.state},
             {"data", d.data},
             {"order_id", mOrder.id},
             {"restore_stoplist", d.qty - newQty}},
            [this](const QJsonObject &jdoc) { parseOrder(jdoc); },
            [](const QJsonObject &jerr) {});
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

                    if(wd.state == DISH_STATE_OK && !wd.isHourlyPayment() && isGoodsLikeType(wd.type)) {
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

                if(wd.state == DISH_STATE_OK && !wd.isHourlyPayment() && isGoodsLikeType(wd.type)) {
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

                    if(wd.state == DISH_STATE_OK && !wd.isHourlyPayment() && isGoodsLikeType(wd.type)) {
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
        QLayoutItem *li = ui->vlDishes->itemAt(i);
        QWidget *lw = li ? li->widget() : nullptr;
        WaiterDishWidget *ow = lw ? qobject_cast<WaiterDishWidget *>(lw) : nullptr;
        WaiterDish w = mOrder.dishes.at(i);

        if(!ow) {
            continue;
        }

        ow->mShowRemoved = mShowRemoved;
        ow->setDisplayContext(isBistroMode(), mOrder.state == ORDER_STATE_PREORDER,
                              mOrder.serviceFactor(), mOrder.discountFactor());
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
    updateCashTenderButtons();
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
    QString tableName = d.mTableName;

    if(tableId == 0) {
        return;
    }

    QPointer<DlgOrder> self(this);
    auto func = [self](C5User * user, int destinationTableId, const QString & destinationTableName) {
        if(!self) {
            return;
        }

        NInterface::query1("/engine/v2/waiter/order/transfer-table", user->mSessionKey, self, {
            {"id", self->mOrder.id},
            {"source_table_name", self->mOrder.tableName},
            {"destination_table_name", destinationTableName},
            {"locksrc", hostinfo},
            {"destination", destinationTableId}
        }, [self](const QJsonObject & jdoc) {
            C5Message::info(tr("Transfer successfull"));
            self->parseOrder(jdoc);
            self->on_btnExit_clicked();
        });
    };

    if(mUser->check(cp_t5_waiter_transfer_items)) {
        func(mUser, tableId, tableName);
    } else {
        QString pin;

        if(!DlgPassword::getPasswordString(tr("Confirm table transfer"), pin)) {
            return;
        }

        auto *user = new C5User();
        user->authorize(pin, self->fHttp, [self, func, user, tableId, tableName](const QJsonObject & jdoc) {
            if(user->check(cp_t5_waiter_transfer_items)) {
                func(user, tableId, tableName);
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

    const FiscalMachine fm = fiscalMachineForWorkstation(mWorkStation);

    if(fm.id == 0) {
        C5Message::error(tr("Fiscal machine is not configured for this workstation."));
        return;
    }

    QPointer<DlgOrder> self(this);
    auto *loading = new NLoadingDlg(tr("Printing fiscal check"), this);
    QPointer<NLoadingDlg> loadingPtr(loading);
    auto *thread = new QThread();
    auto *pt = new PrintTaxN(fm.ip, fm.port, fm.machinePassword, fm.externalPosString(), fm.opPin, fm.opPassword);
    pt->moveToThread(thread);
    const WaiterOrder order = self->mOrder;
    const QString sessionKey = mUser->mSessionKey;
    const QString orderId = order.id;

    connect(pt, &PrintTaxN::finished, pt, &QObject::deleteLater);
    connect(pt, &PrintTaxN::finished, thread, &QThread::quit);
    connect(thread, &QThread::started, pt, [pt, order]() {
        for(const WaiterDish &src : order.dishes) {
            if(src.state != DISH_STATE_OK) {
                continue;
            }

            WaiterDish dish = src;
            pt->addGoods(dish.fiscalDepartment(),
                         dish.adgtCode(),
                         QString::number(dish.dishId),
                         dish.dishName,
                         dish.price,
                         dish.qty,
                         dish.discountFactor() * 100);
        }

        const FiscalPaymentPayload pay = fiscalPaymentsFromOrder(order);
        applyFiscalPayments(pt, pay);
        pt->fEmarks = collectFiscalEmarks(order);
        pt->makeJsonAndPrint(pay.cash, pay.nonCash, pay.prepaid);
    });
    connect(pt, &PrintTaxN::started, loading, &QDialog::show, Qt::QueuedConnection);
    connect(pt, &PrintTaxN::finished, self, [self, loadingPtr, sessionKey, orderId](
                const QString &inJson, const QString &outJson, const QString &err, int result) {
        if(!self) {
            return;
        }

        if(loadingPtr) {
            loadingPtr->close();
            loadingPtr->deleteLater();
        }

        if(result == 0) {
            QJsonObject reply{{"id", orderId},
                              {"fiscal", QJsonObject{{"in", inJson},
                                                     {"out", outJson},
                                                     {"error", err},
                                                     {"result", result},
                                                     {"f_fiscal_machine_id", mWorkStation.fiscalMachineId()}}}};
            NInterface::prepareLoadingTitle(
                QObject::tr("Printer: %1").arg(mWorkStation.precheckPrinter()));
            NInterface::query("/engine/v2/waiter/order/fiscal-printed", sessionKey, self, reply,
                              [self](const QJsonObject &jdoc) {
                                  if(!self) {
                                      return;
                                  }

                                  self->parseOrder(jdoc);
                                  self->printPrecheck(self->mUser->shortFullName());
                              },
                              [](const QJsonObject &) { return false; });
        } else {
            QJsonObject reply{{"id", orderId},
                              {"in", inJson},
                              {"out", outJson},
                              {"error", err},
                              {"result", result},
                              {"f_fiscal_machine_id", mWorkStation.fiscalMachineId()}};
            NInterface::query("/engine/v2/waiter/order/fiscal-log", sessionKey, self, reply,
                              [](const QJsonObject &) {},
                              [](const QJsonObject &) { return false; });
            C5Message::error(err);
        }
    }, Qt::QueuedConnection);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void DlgOrder::on_btnCopyUUID_clicked()
{
    auto *cl = qApp->clipboard();
    cl->setText(mOrder.id);
}

void DlgOrder::on_btnGuest_clicked()
{
    if (mOrder.id.isEmpty()) {
        C5Message::error(tr("Empty order"));
        return;
    }
    DlgGuestInfo dg(mUser);
    dg.setInfo(mOrder.data.value("f_guest").toObject());
    if (dg.exec() == QDialog::Accepted) {
        mOrder.data["f_guest"] = dg.getInfo();
        NInterface::query1("/engine/v2/waiter/order/save-data",
                           mUser->mSessionKey,
                           this,
                           {{"id", mOrder.id}, {"data", mOrder.data}, {"create_guest", true}},
                           [this](const QJsonObject &jdoc) { parseOrder(jdoc); });
    }
}

void DlgOrder::on_btnPreorderDateTime_clicked()
{
    if(mOrder.id.isEmpty()) {
        return;
    }

    DlgPreorderDateTime dlg(mUser, this);
    const QString raw = mOrder.data.value(QStringLiteral("f_preorder_datetime")).toString().trimmed();
    QDateTime initial = QDateTime::fromString(raw, FORMAT_DATETIME_TO_STR_MYSQL);

    if(!initial.isValid()) {
        initial = QDateTime::currentDateTime();
    }

    dlg.setInitial(initial);

    if(dlg.exec() != QDialog::Accepted) {
        return;
    }

    if(dlg.clearRequested()) {
        QJsonObject params;
        params.insert(QStringLiteral("id"), mOrder.id);
        params.insert(QStringLiteral("key"), QStringLiteral("f_preorder_datetime"));
        params.insert(QStringLiteral("value"), QJsonValue::Null);
        NInterface::query1(QStringLiteral("/engine/v2/waiter/order/set-data-value"),
                           mUser->mSessionKey,
                           this,
                           params,
                           [this](const QJsonObject &jdoc) {
                               parseOrder(jdoc);
                           });
        return;
    }

    const QString stored = dlg.selectedDateTime().toString(FORMAT_DATETIME_TO_STR_MYSQL);
    NInterface::query1(QStringLiteral("/engine/v2/waiter/order/set-data-value"),
                       mUser->mSessionKey,
                       this,
                       {{"id", mOrder.id}, {"key", QStringLiteral("f_preorder_datetime")}, {"value", stored}},
                       [this](const QJsonObject &jdoc) {
                           parseOrder(jdoc);
                       });
}

void DlgOrder::on_btnActivatePreorder_pressed()
{
    if(mOrder.id.isEmpty() || mOrder.state != ORDER_STATE_PREORDER) {
        return;
    }

    if(C5Message::question(tr("Turn this preorder into an open order on the table?")) != QDialog::Accepted) {
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/order/activate-preorder"), mUser->mSessionKey, this,
    {{"id", mOrder.id}},
    [this](const QJsonObject &jdoc) {
        parseOrder(jdoc);
    });
}

static QSettings customerDisplaySettings()
{
    return QSettings(_ORGANIZATION_, _APPLICATION_ + QString("\\") + _MODULE_);
}

void DlgOrder::openCustomerDisplay()
{
    if(mCustomerDisplay) {
        return;
    }

    if(qApp->screens().size() < 2) {
        ui->btnShowCustomerDisplay->setChecked(false);
        C5Message::error(tr("A second display is not connected. Connect a customer display and try again.",
                            "customer_display_no_second_screen"));
        return;
    }

    customerDisplaySettings().setValue(QStringLiteral("customerdisplay"), true);

    mCustomerDisplay = new DlgCustDisplay();
    mCustomerDisplay->setAttribute(Qt::WA_DeleteOnClose);
    connect(mCustomerDisplay, &QObject::destroyed, this, [this]() {
        mCustomerDisplay = nullptr;
        ui->btnShowCustomerDisplay->setChecked(false);
    });

    updateCustomerDisplay();
    mCustomerDisplay->showOnSecondScreen();
    ui->btnShowCustomerDisplay->setChecked(true);
}

void DlgOrder::tryOpenCustomerDisplayIfEnabled()
{
    if(mCustomerDisplay) {
        return;
    }

    if(!customerDisplaySettings().value(QStringLiteral("customerdisplay")).toBool()) {
        return;
    }

    if(qApp->screens().size() < 2) {
        return;
    }

    openCustomerDisplay();
}

void DlgOrder::closeCustomerDisplay(bool userTurnedOff)
{
    if(userTurnedOff) {
        customerDisplaySettings().setValue(QStringLiteral("customerdisplay"), false);
    }

    if(!mCustomerDisplay) {
        ui->btnShowCustomerDisplay->setChecked(false);
        return;
    }

    mCustomerDisplay->close();
    mCustomerDisplay->deleteLater();
    mCustomerDisplay = nullptr;
    ui->btnShowCustomerDisplay->setChecked(false);
}

void DlgOrder::updateCustomerDisplay()
{
    if(!mCustomerDisplay) {
        return;
    }

    const bool bistro = isBistroMode();
    const bool isPreorder = (mOrder.state == ORDER_STATE_PREORDER);
    const double orderServiceFactor = mOrder.serviceFactor();
    const double orderDiscountFactor = qAbs(mOrder.discountFactor());
    const WaiterOrderCalculatedAmounts bistroAmounts = bistro ? mOrder.calculatedAmounts(true)
                                                              : WaiterOrderCalculatedAmounts{};

    QList<CustDisplayLine> lines;

    for(const WaiterDish &dish : mOrder.dishes) {
        if(dish.state != DISH_STATE_OK || !isCustomerDisplayDishType(dish.type)) {
            continue;
        }

        const double lineAmount = bistro
                                      ? dish.lineAmount(isPreorder, true, orderServiceFactor, orderDiscountFactor)
                                      : customerDisplayLineAmount(dish, mOrder.state);

        CustDisplayLine line;
        line.name = dish.dishName.trimmed().isEmpty() ? dish.translated() : dish.dishName;
        line.qtyText = float_str(dish.qty, 2);
        line.priceText = float_str(dish.price, 2);
        line.amountText = float_str(lineAmount, 2);
        lines.append(line);
    }

    CustDisplayTotals totals;
    const double subtotal = bistro ? bistroAmounts.subtotal : mOrder.subTotal();
    totals.subtotalText = QString("%1 %2").arg(float_str(subtotal, 2), CURRENCY_SHORT);

    const double serviceAmount = bistro ? bistroAmounts.serviceAmount : mOrder.serviceAmount();
    if(mOrder.serviceFactor() > 0.0001 || serviceAmount > 0.0001) {
        const QString serviceComment = mOrder.data.value(QStringLiteral("f_service_comment")).toString().trimmed();
        totals.serviceCaption = serviceComment.isEmpty() ? tr("Service") : serviceComment;
        totals.serviceText = QStringLiteral("+%1%  %2 %3")
                                 .arg(float_str(mOrder.serviceFactor() * 100, 2),
                                      float_str(serviceAmount, 2),
                                      CURRENCY_SHORT);
        totals.showService = true;
    }

    const double discountAmount = bistro ? bistroAmounts.discountAmount : mOrder.discountAmount();
    if(mOrder.discountFactor() > 0.0001 || discountAmount > 0.0001) {
        const QString discountComment = mOrder.data.value(QStringLiteral("f_discount_comment")).toString().trimmed();
        totals.discountCaption = discountComment.isEmpty() ? tr("Discount") : discountComment;
        totals.discountText = QStringLiteral("-%1%  %2 %3")
                                  .arg(float_str(mOrder.discountFactor() * 100, 2),
                                       float_str(discountAmount, 2),
                                       CURRENCY_SHORT);
        totals.showDiscount = true;
    }

    const double totalDue = bistro ? bistroAmounts.totalDue : mOrder.totalDue;
    totals.totalDueText = QString("%1 %2").arg(float_str(totalDue, 2), CURRENCY_SHORT);
    mCustomerDisplay->setContent(lines, totals);
}

void DlgOrder::on_btnShowCustomerDisplay_clicked()
{
    if(mCustomerDisplay) {
        closeCustomerDisplay(true);
        return;
    }

    openCustomerDisplay();
}

void DlgOrder::on_btnDeliveryAmount_clicked()
{
    if(mOrder.id.isEmpty()) {
        C5Message::error(tr("Order was not opened"));
        return;
    }

    DlgMoveMoney d(mUser);
    d.setMode(0);
    d.setAmount(mOrder.data.value("f_delivery_amount").toDouble());
    if(d.exec() != QDialog::Accepted) {
        return;
    }

    NInterface::query1("/engine/v2/waiter/order/set-data-value",
                       mUser->mSessionKey,
                       this,
                       {{"id", mOrder.id}, {"key", "f_delivery_amount"}, {"value", d.amount()}},
                       [this](const QJsonObject &jdoc) { parseOrder(jdoc); });
}

void DlgOrder::updateDatamatrixButton()
{
    bool hasCodes = false;
    const QJsonValue datamatrix = mOrder.data.value(QStringLiteral("f_datamatrix"));
    if (datamatrix.isArray()) {
        for (const QJsonValue &v : datamatrix.toArray()) {
            if (!v.toString().trimmed().isEmpty()) {
                hasCodes = true;
                break;
            }
        }
    }

    ui->btnDatamatrix->setStyleSheet(hasCodes
        ? QStringLiteral(
            "QToolButton {"
            " background-color: #a8e6a1;"
            " border: 2px solid #3d8b37;"
            " border-radius: 4px;"
            "}"
            "QToolButton:pressed {"
            " background-color: #7ed987;"
            "}")
        : QString());
}

void DlgOrder::on_btnDatamatrix_clicked()
{
    if (mOrder.id.isEmpty()) {
        C5Message::error(tr("Order was not opened"));
        return;
    }

    QStringList codes;
    const QJsonValue datamatrix = mOrder.data.value(QStringLiteral("f_datamatrix"));
    if (datamatrix.isArray()) {
        for (const QJsonValue &v : datamatrix.toArray()) {
            const QString code = v.toString().trimmed();
            if (!code.isEmpty()) {
                codes.append(code);
            }
        }
    }

    if (!DlgOrderDatamatrix::editCodes(mUser, codes, this)) {
        return;
    }

    QJsonArray arr;
    for (const QString &code : codes) {
        arr.append(code);
    }

    NInterface::query1("/engine/v2/waiter/order/set-data-value",
                       mUser->mSessionKey,
                       this,
                       {{"id", mOrder.id}, {"key", "f_datamatrix"}, {"value", arr}},
                       [this](const QJsonObject &jdoc) { parseOrder(jdoc); });
}

void DlgOrder::on_btnLast40Min_clicked()
{
    const int minutes = mWorkStation.data.value(QStringLiteral("recent_dishes_minutes")).toInt(40);
    DlgRecentDishes::open(mUser, minutes, this);
}
