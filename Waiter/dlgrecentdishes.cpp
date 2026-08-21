#include "dlgrecentdishes.h"
#include "ui_dlgrecentdishes.h"

#include "c5user.h"
#include "c5utils.h"
#include "ninterface.h"

#include <QHeaderView>
#include <QJsonObject>
#include <QSpacerItem>
#include <QToolButton>

DlgRecentDishes::DlgRecentDishes(C5User *user, int minutes, QWidget *parent)
    : C5Dialog(user, parent)
    , ui(new Ui::DlgRecentDishes)
    , mMinutes(minutes > 0 ? minutes : 40)
{
    ui->setupUi(this);
    ui->tblDishes->setColumnCount(2);
    ui->tblDishes->setHorizontalHeaderLabels({tr("Dish"), tr("Qty")});
    ui->tblDishes->horizontalHeader()->setStretchLastSection(false);
    ui->tblDishes->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tblDishes->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tblDishes->verticalHeader()->setVisible(false);
    ui->lbTitle->setText(tr("Recent dishes (%1 min)").arg(mMinutes));
    loadData();
}

DlgRecentDishes::~DlgRecentDishes()
{
    delete ui;
}

void DlgRecentDishes::open(C5User *user, int minutes, QWidget *parent)
{
    auto *dlg = new DlgRecentDishes(user, minutes, parent);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

void DlgRecentDishes::loadData()
{
    NInterface::query1("/engine/v2/waiter/order/recent-dishes",
                       mUser->mSessionKey,
                       this,
                       {{"minutes", mMinutes}},
                       [this](const QJsonObject &jo) {
                           mStations.clear();
                           const QJsonArray stations = jo.value(QStringLiteral("stations")).toArray();
                           for (const QJsonValue &v : stations) {
                               const QString s = v.toString().trimmed();
                               if (!s.isEmpty()) {
                                   mStations.append(s);
                               }
                           }
                           mRows = jo.value(QStringLiteral("rows")).toArray();
                           buildStationButtons();
                           if (!mStations.isEmpty()) {
                               showStation(mStations.first());
                           } else {
                               ui->tblDishes->setRowCount(0);
                           }
                       });
}

void DlgRecentDishes::buildStationButtons()
{
    QLayout *layout = ui->hlStations;
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }

    for (const QString &station : mStations) {
        auto *btn = new QToolButton(ui->scrollStationsContents);
        btn->setText(station);
        btn->setMinimumHeight(40);
        btn->setCheckable(true);
        btn->setAutoExclusive(true);
        btn->setProperty("station", station);
        connect(btn, &QToolButton::clicked, this, &DlgRecentDishes::stationClicked);
        layout->addWidget(btn);
    }
    layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
}

void DlgRecentDishes::stationClicked()
{
    auto *btn = qobject_cast<QToolButton *>(sender());
    if (!btn) {
        return;
    }
    showStation(btn->property("station").toString());
}

void DlgRecentDishes::showStation(const QString &station)
{
    mCurrentStation = station;
    for (int i = 0; i < ui->hlStations->count(); ++i) {
        QLayoutItem *item = ui->hlStations->itemAt(i);
        if (!item || !item->widget()) {
            continue;
        }
        if (auto *btn = qobject_cast<QToolButton *>(item->widget())) {
            btn->setChecked(btn->property("station").toString() == station);
        }
    }

    ui->tblDishes->setRowCount(0);
    int row = 0;
    for (const QJsonValue &v : mRows) {
        const QJsonObject o = v.toObject();
        if (o.value(QStringLiteral("print1")).toString() != station) {
            continue;
        }
        ui->tblDishes->insertRow(row);
        ui->tblDishes->setItem(row, 0, new QTableWidgetItem(o.value(QStringLiteral("dish_name")).toString()));
        ui->tblDishes->setItem(row, 1, new QTableWidgetItem(float_str(o.value(QStringLiteral("qty")).toDouble(), 3)));
        ++row;
    }
}

void DlgRecentDishes::on_btnClose_clicked()
{
    accept();
}
