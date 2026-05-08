#pragma once

#include "c5waiterdialog.h"
#include "struct_hall.h"
#include "struct_table.h"
#include "struct_goods_group.h"
#include "struct_dish.h"

namespace Ui
{
class DlgPreordersList;
}

class C5User;

class DlgPreordersList : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgPreordersList(C5User *user,
                              const QVector<HallItem> *halls,
                              const QVector<TableItem> *tables,
                              const QVector<GoodsGroupItem*> *groups,
                              const QVector<DishAItem*> *dishes,
                              QWidget *parent = nullptr);
    ~DlgPreordersList() override;

    QString mOrderId;
    int mTableId = 0;
    int mHallId = 0;

protected:
    void showEvent(QShowEvent *e) override;

private slots:
    void on_btnCreate_clicked();
    void on_btnOpen_clicked();
    void on_btnExit_clicked();
    void on_tblPreorders_cellDoubleClicked(int row, int column);

private:
    Ui::DlgPreordersList *ui;
    const QVector<HallItem> *mHalls = nullptr;
    const QVector<TableItem> *mTables = nullptr;
    const QVector<GoodsGroupItem*> *mGroups = nullptr;
    const QVector<DishAItem*> *mDishes = nullptr;
    bool mLoaded = false;

    void loadData();
    void openCurrent();
};

