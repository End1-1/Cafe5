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

    void on_btnFilterAll_clicked();

    void on_tblKitchenOrders_cellClicked(int row, int column);

    void on_btnSearchAnyKbd_clicked();

    void on_btnOrderNumberKbd_clicked();

    void tickClock();

    void on_btnSettings_clicked();

    void on_btnHistory_clicked();

private:
    struct KitchenLinePick {
        QString lineId;
        int status = 1;
        QString pickerLabel;
    };

    enum KitchenCol {
        ColHeaderId = 0,
        ColOrderPrefix,
        ColTimeOpen,
        ColTableName,
        ColHallName,
        ColDishes,
        ColGuest,
        ColPaid,
        ColStatus,
    };

    Ui::DlgKitchenInProgress *ui;

    QTimer mClockTimer;

    QString mOriginalWindowTitle;

    bool mKitchenHistoryReadonly = false;

    QString mKitchenHistoryDate;

    /** Live queue buffer — only `/in-progress/get` and update-status. */
    QJsonArray mLiveKitchenRows;
    int mLiveKitchenRequestGen = 0;

    /** History buffer — only `/in-progress/get-history-for-date`. */
    QJsonArray mHistoryKitchenRows;
    int mHistoryKitchenRequestGen = 0;

    /** Line picks for live table rows (status change). */
    QVector<QVector<KitchenLinePick>> mLiveLinePicksByTableRow;

    /** Line picks for history table rows (read-only; kept for symmetry / future). */
    QVector<QVector<KitchenLinePick>> mHistoryLinePicksByTableRow;

    /** First non-spontaneous showEvent: load live queue once. */
    bool mDidInitialShowEventLiveLoad = false;

    /** 0 = all statuses; otherwise any kitchen line status 1 / 2 / 3 matches order (live only). */
    int mStatusFilter = 0;

    const QVector<HallItem> *mHalls = nullptr;

    const QVector<TableItem> *mTables = nullptr;

    const QVector<GoodsGroupItem *> *mGroups = nullptr;

    const QVector<DishAItem *> *mDishes = nullptr;

    void reloadLiveKitchenList();

    void loadKitchenHistoryForDate(const QString &historyDateYmd);

    void setKitchenHistoryReadonly(bool readonly, const QString &historyDateYmd = QString());

    void ingestLiveKitchenDoc(const QJsonObject &jdoc);

    void ingestHistoryKitchenDoc(const QJsonObject &jdoc);

    /* --- Live queue (current kitchen): own headers, columns registry, filter, fill. --- */
    void liveSetupTableHorizontalHeaders();

    void liveApplyFixedColumnLayout();

    void liveClearTable();

    void liveFillTableRows(const QJsonArray &orders);

    QJsonArray liveFilterOrdersForView() const;

    QString liveHaystackForOrderSearch(const QJsonObject &order) const;

    void liveReflowTableFromBuffer();

    void liveApplySearchFromLineEdits();

    /* --- History (served archive): separate code path; same QTableWidget in UI only. --- */
    void historySetupTableHorizontalHeaders();

    void historyApplyFixedColumnLayout();

    void historyClearTable();

    void historyFillTableRows(const QJsonArray &orders);

    QJsonArray historyFilterOrdersForView() const;

    QString historyHaystackForOrderSearch(const QJsonObject &order) const;

    void historyReflowTableFromBuffer();

    void historyApplySearchFromLineEdits();

    void syncFilterButtons();

    static QString normalizedOrderSuffix(const QString &s);

    QString kitchenStatusText(int status) const;

    QString paymentColumnText(const QJsonObject &order) const;

    static QString formatQtyWidth5(double qty);

    static QString guestMultiline(const QJsonObject &order);

    void applyKitchenLineStatusToLines(const QVector<QString> &lineIds, int status);

    void runKitchenStatusUpdateChain(const QVector<QString> &lineIds, int index, int status);

    void openStatusPickerForLineIds(const QVector<QString> &lineIds);

    void openStatusPickerForTableRow(int row);

    void openDlgOrderForKitchenRow(int row);
};
