#include "dlgface.h"
#include <QCloseEvent>
#include <QScreen>
#include <QToolButton>
#include "c5permissions.h"
#include "c5user.h"
#include "dlgdashboard.h"
#include "dlgguest.h"
#include "dlgorder.h"
#include "dlgpreorderslist.h"
#include "dlgkitcheninprogress.h"
#include "dlgtext.h"
#include "format_date.h"
#include "ninterface.h"
#include "struct_workstationitem.h"
#include "tablewidget.h"
#include "ui_dlgface.h"

#define HALL_COL_WIDTH 175
#define HALL_ROW_HEIGHT 60
#ifdef QT_DEBUG
#define TIMER_TIMEOUT_INTERVAL 5000
#else
#define TIMER_TIMEOUT_INTERVAL 30000
#endif

DlgFace::DlgFace(C5User *user) :
    C5WaiterDialog(user),
    ui(new Ui::DlgFace)

{
    ui->setupUi(this);
    ui->lbStaff->setText(user->fullName());
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
    connect(&fTimer, &QTimer::timeout, this, &DlgFace::timeout);
    fTimer.start(TIMER_TIMEOUT_INTERVAL);
    //TODO
    ui->btnGuests->setVisible(false);
    mSelectedHall = mWorkStation.defaultHallId();
    setupButtons();
}

DlgFace::~DlgFace()
{
    mTableWidgetsPool.clear();
    delete ui;
}

int DlgFace::exec()
{
    C5WaiterDialog::exec();
    return mRoute;
}

void DlgFace::timeout()
{
    ui->lbTime->setText(QTime::currentTime().toString(FORMAT_TIME_TO_SHORT_STR));
    updateHall();
}

void DlgFace::hallClicked()
{
    QToolButton *btn = static_cast<QToolButton*>(sender());
    mSelectedHall = btn->property("id").toInt();
    filterHall();
}

void DlgFace::tableClicked(int id)
{
    fTimer.stop();
    TableItem t;

    for(auto ti : mTables) {
        if(ti.id == id) {
            t = ti;
            break;
        }
    }

    HallItem h;

    for(auto hi : mHall) {
        if(hi.id == t.hall) {
            h = hi;
            break;
        }
    }

    auto *d = new DlgOrder(mUser, h,  t, &mGoodsGroups, &mDishes);
    d->exec();
    d->deleteLater();
    updateHall();
    fTimer.start(TIMER_TIMEOUT_INTERVAL);
}

void DlgFace::on_btnExit_clicked()
{
    accept();
}

void DlgFace::filterHall()
{
    while(auto *item = ui->vlHall->itemAt(0)) {
        item->widget()->deleteLater();
        ui->vlHall->removeItem(item);
    }

    for(auto const &h : mHall) {
        QToolButton *btn = new QToolButton();
        btn->setText(h.name);
        btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
        btn->setMinimumSize(QSize(140, 50));
        btn->setProperty("id", h.id);
        connect(btn, &QToolButton::clicked, this, &DlgFace::hallClicked);
        ui->vlHall->addWidget(btn);
    }

    QVector<TableItem> visible;
    visible.reserve(mTables.size());

    for(const TableItem &t : mTables) {
        if(mSelectedHall > 0 && t.hall != mSelectedHall) {
            continue;
        }

        if(mTablesOfStaff > 0 && t.staff != mTablesOfStaff) {
            continue;
        }

        visible.push_back(t);
    }

    const int sw = ui->shall->width() - 20;
    const int cc = (sw / 204) - 1;
    int delta = sw - ((cc + 1) * 204);
    delta /= cc;
    const int minWidth = 200 + delta;
    const int needed = visible.size();

    while(mTableWidgetsPool.size() < needed) {
        auto *tw = new TableWidget(this);
        connect(tw, &TableWidget::clicked, this, &DlgFace::tableClicked);
        mTableWidgetsPool.append(tw);
    }

    while(mTableWidgetsPool.size() > needed) {
        TableWidget *tw = mTableWidgetsPool.takeLast();
        ui->sglHall->removeWidget(tw);
        tw->deleteLater();
    }

    for(TableWidget *tw : mTableWidgetsPool) {
        ui->sglHall->removeWidget(tw);
    }

    int row = 0;
    int col = 0;

    for(int i = 0; i < needed; ++i) {
        TableWidget *tw = mTableWidgetsPool[i];
        tw->setMinimumWidth(minWidth);
        tw->setMaximumWidth(minWidth);
        tw->setTable(visible[i]);
        ui->sglHall->addWidget(tw, row, col++, 1, 1);

        if(col > cc) {
            row++;
            col = 0;
        }
    }

    ui->sglHall->setRowStretch(row + 1, 1);
    colorizeHall();
}

void DlgFace::colorizeHall()
{
    for(int i = 0; i < ui->vlHall->count(); i++) {
        QToolButton *btn = dynamic_cast<QToolButton*>(ui->vlHall->itemAt(i)->widget());

        if(btn) {
            if(btn->property("id").toInt() == mSelectedHall) {
                btn->setProperty("stylesheet_button_selected", true);
                btn->style()->polish(btn);
            } else {
                btn->setProperty("stylesheet_button_selected", false);
                btn->style()->polish(btn);
            }
        }
    }
}

void DlgFace::updateHall(const QJsonObject &filter)
{
    QJsonObject jparams;

    for (auto it = filter.begin(); it != filter.end(); ++it) {
        jparams.insert(it.key(), it.value());
    }

    fHttp->createHttpQueryLambda(
        "/engine/v2/waiter/hall/get",
        jparams,
        [this](const QJsonObject &jdoc) {
            mTables = parseJsonArray<TableItem>(jdoc["tables"].toArray());
            filterHall();
        },
        [](const QJsonObject &jerr) {},
        QVariant(),
        false);
}

void DlgFace::showEvent(QShowEvent *e)
{
    C5WaiterDialog::showEvent(e);

    if(e->spontaneous()) {
        return;
    }

    NInterface::query1("/engine/v2/waiter/cashbox/check-status", mUser->mSessionKey, this,
    {{"cashbox_id", mWorkStation.cashboxId()}},
    [ = ](const QJsonObject & jdoc) {
        if(jdoc["cashbox_session_id"].toInt() == 0) {
            mRoute = 2;
            accept();
        } else {
            initData();
        }
    });
}

void DlgFace::filterStaffClicked()
{
    QToolButton *btn = static_cast<QToolButton*>(sender());
    mTablesOfStaff =  btn->property("id").toInt();
    filterHall();
}

void DlgFace::on_btnGuests_clicked()
{
    QString res, inv, room, guest;
    DlgGuest::getGuest(mUser, res, inv, room, guest);
}

void DlgFace::initData()
{
    auto *httpHall  = new NInterface(this);
    httpHall->createHttpQueryLambda("/engine/v2/waiter/hall/get",
                                    {},
    [this, httpHall](const QJsonObject & jdoc) {
        mHall = parseJsonArray<HallItem>(jdoc["halls"].toArray());
        mTables = parseJsonArray<TableItem>(jdoc["tables"].toArray());
        filterHall();
        httpHall->deleteLater();
    }, [httpHall](const QJsonObject & jerr) {
        httpHall->deleteLater();
    },
    QVariant(),
    false);
    auto *httpMenu = new NInterface(this);
    httpMenu->createHttpQueryLambda("/engine/v2/waiter/menu/get", {},
    [this, httpMenu](const QJsonObject & jdoc) {
        qDeleteAll(mGoodsGroups);
        qDeleteAll(mDishes);
        mGoodsGroups.clear();
        mDishes.clear();
        QJsonArray ja = jdoc["groups"].toArray();
        QVector<GoodsGroupItem*> allGroups;
        QHash<int, GoodsGroupItem*> groupMap;
        allGroups.reserve(ja.size());

        for(int i = 0; i < ja.size(); i++) {
            auto *g = JsonParser<GoodsGroupItem>::pointerFromJson(ja.at(i).toObject());
            allGroups << g;
            groupMap[g->id] = g;
        }

        /* QHash iteration order is undefined; keep backend JSON order. */
        for(auto *g : allGroups) {
            if(g->parentId == 0) {
                mGoodsGroups << g;
            } else if(groupMap.contains(g->parentId)) {
                groupMap[g->parentId]->children << g;
            }
        }

        ja = jdoc["dishes"].toArray();
        mDishes.reserve(ja.size());

        for(int i = 0; i  < ja.size(); i++) {
            /* Keep dishes in backend JSON order (no hash/map reordering). */
            mDishes << JsonParser<DishAItem>::pointerFromJson(ja.at(i).toObject());
        }

        httpMenu->deleteLater();
    }, [httpMenu](const QJsonObject & jerr) {
        httpMenu->deleteLater();
    }, QVariant(), false);
}

void DlgFace::on_btnDashboard_clicked()
{
    mRoute = 2;
    accept();
}

void DlgFace::on_btnPreorders_clicked()
{
    /* После закрытия заказа снова показываем список; выход в зал — кнопка Lock, как на экране столов. */
    while(true) {
        DlgPreordersList d(mUser, &mHall, &mTables, &mGoodsGroups, &mDishes, this);

        if(d.exec() != QDialog::Accepted || d.mOrderId.isEmpty()) {
            break;
        }

        fTimer.stop();

        TableItem t;
        for(const auto &ti : mTables) {
            if(ti.id == d.mTableId) {
                t = ti;
                break;
            }
        }

        HallItem h;
        for(const auto &hi : mHall) {
            if(hi.id == d.mHallId) {
                h = hi;
                break;
            }
        }

        auto *order = new DlgOrder(mUser, h, t, &mGoodsGroups, &mDishes);
        order->setOrderId(d.mOrderId);
        order->exec();
        order->deleteLater();

        updateHall();
        fTimer.start(TIMER_TIMEOUT_INTERVAL);
    }
}

void DlgFace::on_btnInProgress_clicked()
{
    fTimer.stop();
    DlgKitchenInProgress d(mUser, &mHall, &mTables, &mGoodsGroups, &mDishes, this);
    d.exec();
    updateHall();
    fTimer.start(TIMER_TIMEOUT_INTERVAL);
}

void DlgFace::on_btnNormalView_clicked()
{
    updateHall();
}
