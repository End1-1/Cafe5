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
class TableWidget;

class DlgFace : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgFace(C5User *user);

    ~DlgFace();

    virtual int exec() override;

private slots:

    void timeout();

    void hallClicked();

    void tableClicked(int id);

    void filterStaffClicked();

    void updateHall(const QJsonObject &filter = {});

    void on_btnExit_clicked();

    void on_btnGuests_clicked();

    void on_btnDashboard_clicked();

    void on_btnPreorders_clicked();

    void on_btnInProgress_clicked();

    void on_btnNormalView_clicked();

protected:
    virtual void showEvent(QShowEvent *e) override;

private:
    Ui::DlgFace* ui;

    int mRoute = 0;

    QTimer fTimer;

    QVector<HallItem> mHall;

    QVector<TableItem> mTables;

    QVector<GoodsGroupItem*> mGoodsGroups;

    QVector<DishAItem*> mDishes;

    int mSelectedHall = 0;

    int mTablesOfStaff = 0;

    QVector<TableWidget *> mTableWidgetsPool;

    void initData();

    void filterHall();

    void colorizeHall();
};
