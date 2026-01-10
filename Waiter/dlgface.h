#pragma once

#include "c5waiterdialog.h"
#include <QTableWidgetItem>
#include "struct_hall.h"
#include "struct_table.h"
#include "struct_goods_group.h"
#include "struct_dish.h"
#include <QTimer>
#include <QJsonObject>

namespace Ui
{
class DlgFace;
}

class C5User;

class DlgFace : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgFace(C5User *user);

    ~DlgFace();

private slots:

    void timeout();

    void hallClicked();

    void tableClicked(int id);

    void filterStaffClicked();

    void updateHall();

    void on_btnExit_clicked();

    void on_btnGuests_clicked();

    void on_btnReports_clicked();

    void on_btnOrders_clicked();

private:
    Ui::DlgFace* ui;

    QTimer fTimer;

    QVector<HallItem> mHall;

    QVector<TableItem> mTables;

    QVector<GoodsGroupItem*> mGoodsGroups;

    QVector<DishAItem*> mDishes;

    int mSelectedHall = 0;

    int mTablesOfStaff = 0;

    void initData();

    virtual void showEvent(QShowEvent *e) override;

    void filterHall();

    void colorizeHall();

};
