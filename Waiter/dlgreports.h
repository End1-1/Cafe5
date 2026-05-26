#ifndef DLGREPORTS_H
#define DLGREPORTS_H

#include "c5waiterdialog.h"
#include "struct_dish.h"
#include "struct_goods_group.h"
#include "struct_hall.h"
#include "struct_table.h"

namespace Ui
{
class DlgReports;
}

class C5User;

class DlgReports : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgReports(C5User *user,
                        const QVector<HallItem> *halls = nullptr,
                        const QVector<TableItem> *tables = nullptr,
                        const QVector<GoodsGroupItem *> *groups = nullptr,
                        const QVector<DishAItem *> *dishes = nullptr,
                        QWidget *parent = nullptr);

    ~DlgReports();

    static void openReports(C5User *user);

protected:
    virtual void showEvent(QShowEvent *e) override;

private:
    Ui::DlgReports* ui;

    int fCurrentHall;

    QDate mDate1;

    QDate mDate2;

    const QVector<HallItem> *mHalls = nullptr;
    const QVector<TableItem> *mTables = nullptr;
    const QVector<GoodsGroupItem *> *mGroups = nullptr;
    const QVector<DishAItem *> *mDishes = nullptr;

    QVector<HallItem> mHallsOwned;
    QVector<TableItem> mTablesOwned;
    QVector<GoodsGroupItem *> mGoodsGroupsOwned;
    QVector<DishAItem *> mDishesOwned;

    bool mOwnsMenuData = false;
    bool mMenuLoadStarted = false;

    static constexpr int kColTableId = 11;
    static constexpr int kColHallId = 12;

    const QVector<HallItem> &halls() const;
    const QVector<TableItem> &tables() const;

    void initMenuData();
    bool resolveHallTable(int tableId, int hallId, HallItem &h, TableItem &t) const;

    void getDailyCommon();

    void setLangIcon();

private slots:

    void handleDailyCommon(const QJsonObject &obj);

    void on_btnRefresh_clicked();

    void on_btnExit_clicked();

    void on_btnReports_clicked();

    void on_btnParams_clicked();

    void on_btnOpenReport_clicked();
};

#endif // DLGREPORTS_H
