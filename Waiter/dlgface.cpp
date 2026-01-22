#include "dlgface.h"
#include "ui_dlgface.h"
#include "c5user.h"
#include "dlgorder.h"
#include "dlgguest.h"
#include "tablewidget.h"
#include "c5permissions.h"
#include "ninterface.h"
#include "format_date.h"
#include "dlgdashboard.h"
#include "struct_workstationitem.h"
#include <QToolButton>
#include <QCloseEvent>
#include <QScreen>

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
}

DlgFace::~DlgFace()
{
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

    while(ui->sglHall->itemAt(0)) {
        ui->sglHall->itemAt(0)->widget()->deleteLater();
        ui->sglHall->removeItem(ui->sglHall->itemAt(0));
    }

    QRect f = qApp->screens().at(0)->geometry();
    int sw = ui->shall->width() - 20;
    int cc = (sw / 204) - 1;
    int delta = sw - ((cc + 1) * 204);
    delta /= cc;
    int minWidth = 200 + delta;
    qDebug() << sw;
    int col = 0;
    int row = 0;

    for(auto const &t : mTables) {
        if(mSelectedHall > 0) {
            if(t.hall != mSelectedHall) {
                continue;
            }
        }

        if(mTablesOfStaff > 0) {
            if(t.staff  != mTablesOfStaff) {
                continue;
            }
        }

        auto *tw = new TableWidget();
        tw->setMinimumWidth(minWidth);
        tw->setMaximumWidth(minWidth);
        connect(tw, &TableWidget::clicked, this, &DlgFace::tableClicked);
        tw->setTable(t);
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

void DlgFace::updateHall()
{
    fHttp->createHttpQueryLambda("/engine/v2/waiter/hall/get",
                                 {},
    [this](const QJsonObject & jdoc) {
        mTables = parseJsonArray<TableItem>(jdoc["tables"].toArray());
        int i = 0;

        while(ui->sglHall->itemAt(i)) {
            auto *tw = static_cast<TableWidget*>(ui->sglHall->itemAt(i)->widget());

            if(!tw) {
                break;
            }

            i++;
            auto it = std::find_if(mTables.begin(), mTables.end(),
            [tw](const TableItem & t) {
                return t.id == tw->mTable.id;
            });

            if(it != mTables.end()) {
                tw->updateTable(*it);
            }
        }
    }, [](const QJsonObject & jerr) {
    },
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
        QHash<int, GoodsGroupItem*> groupMap;

        for(int i = 0; i < ja.size(); i++) {
            auto *g = JsonParser<GoodsGroupItem>::pointerFromJson(ja.at(i).toObject());
            groupMap[g->id] = g;
        }

        for(auto *g : groupMap) {
            if(g->parentId == 0) {
                mGoodsGroups << g;
            } else if(groupMap.contains(g->parentId)) {
                groupMap[g->parentId]->children << g;
            }
        }

        ja = jdoc["dishes"].toArray();

        for(int i = 0; i  < ja.size(); i++) {
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
