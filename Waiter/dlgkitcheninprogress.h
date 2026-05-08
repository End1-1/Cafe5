#pragma once

#include "c5waiterdialog.h"
#include "struct_dish.h"
#include "struct_goods_group.h"
#include "struct_hall.h"
#include "struct_table.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include <QVector>

namespace Ui
{
class DlgKitchenInProgress;
}

class C5User;

class DlgKitchenInProgress : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgKitchenInProgress(C5User *user,
                                    const QVector<HallItem> *halls,
                                    const QVector<TableItem> *tables,
                                    const QVector<GoodsGroupItem *> *groups,
                                    const QVector<DishAItem *> *dishes,
                                    QWidget *parent = nullptr);
    ~DlgKitchenInProgress() override;

protected:
    void showEvent(QShowEvent *e) override;
    void hideEvent(QHideEvent *e) override;

private slots:
    void on_btnExit_clicked();

    void on_leOrderNumber_returnPressed();

    void on_leSearchAny_returnPressed();

    void on_leOrderNumber_textChanged(const QString &text);

    void on_leSearchAny_textChanged(const QString &text);

    void on_btnFilterAccepted_clicked();

    void on_btnFilterCooking_clicked();

    void on_btnFilterReady_clicked();

    void on_tblKitchenOrders_cellClicked(int row, int column);

    void on_btnSearchAnyKbd_clicked();

    void on_btnOrderNumberKbd_clicked();

    void tickClock();

private:
    struct KitchenLinePick {
        QString lineId;
        int status = 1;
        QString pickerLabel;
    };

    enum KitchenCol {
        ColHeaderId = 0,
        ColOrderPrefix,
        ColTableName,
        ColHallName,
        ColDishes,
        ColGuest,
        ColPaid,
        ColTimeOpen,
        ColStatus,
    };

    Ui::DlgKitchenInProgress *ui;

    QTimer mClockTimer;

    /** Grouped orders from GET / update-status (each element has lines[]). */
    QJsonArray mAllKitchenRows;

    /** Line ids / labels per visible table row (for status change). */
    QVector<QVector<KitchenLinePick>> m_linesPickByTableRow;

    /** 0 = all statuses; otherwise any kitchen line status 1 / 2 / 3 matches order. */
    int mStatusFilter = 0;

    const QVector<HallItem> *mHalls = nullptr;

    const QVector<TableItem> *mTables = nullptr;

    const QVector<GoodsGroupItem *> *mGroups = nullptr;

    const QVector<DishAItem *> *mDishes = nullptr;

    void reloadList();

    void ingestServerDoc(const QJsonObject &jdoc);

    void applyFrontendFilters();

    void fillTableRows(const QJsonArray &orders);
    void applyFixedColumnLayout();

    void syncFilterButtons();

    QString haystackForSearch(const QJsonObject &order) const;

    static QString normalizedOrderSuffix(const QString &s);

    QString kitchenStatusText(int status) const;

    QString paidYesNoText(bool paid) const;

    static QString formatQtyWidth5(double qty);

    static bool isOrderPaid(const QJsonObject &order);

    static QString guestMultiline(const QJsonObject &order);

    void applyKitchenLineStatusToLines(const QVector<QString> &lineIds, int status);

    void runKitchenStatusUpdateChain(const QVector<QString> &lineIds, int index, int status);

    void openStatusPickerForLineIds(const QVector<QString> &lineIds);

    void openStatusPickerForTableRow(int row);

    void openDlgOrderForKitchenRow(int row);
};
