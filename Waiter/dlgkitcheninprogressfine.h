#pragma once

#include "c5waiterdialog.h"
#include "kitchenfineordercard.h"
#include "struct_dish.h"
#include "struct_goods_group.h"
#include "struct_hall.h"
#include "struct_table.h"
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QResizeEvent>
#include <QTimer>
#include <QVector>

namespace Ui
{
class DlgKitchenInProgressFine;
}

class C5User;
class KitchenFineOrderCard;

class DlgKitchenInProgressFine : public C5WaiterDialog
{
    Q_OBJECT

public:
    explicit DlgKitchenInProgressFine(C5User *user,
                                      const QVector<HallItem> *halls,
                                      const QVector<TableItem> *tables,
                                      const QVector<GoodsGroupItem *> *groups,
                                      const QVector<DishAItem *> *dishes,
                                      QWidget *parent = nullptr);
    ~DlgKitchenInProgressFine() override;

protected:
    void showEvent(QShowEvent *e) override;
    void hideEvent(QHideEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void on_btnExit_clicked();
    void on_btnTabLive_clicked();
    void on_btnTabHistory_clicked();
    void tickClock();

private:
    struct KitchenLinePick {
        QString lineId;
        int status = 1;
        QString pickerLabel;
    };

    struct KitchenFineCardEntry {
        QJsonObject order;
        QVector<KitchenLinePick> picks;
        KitchenFineOrderCard *widget = nullptr;
    };

    Ui::DlgKitchenInProgressFine *ui;

    QTimer mClockTimer;
    int mClockTickCounter = 0;

    QString mOriginalWindowTitle;
    bool mKitchenHistoryReadonly = false;
    QString mKitchenHistoryDate;

    QJsonArray mLiveKitchenRows;
    int mLiveKitchenRequestGen = 0;

    QJsonArray mHistoryKitchenRows;
    int mHistoryKitchenRequestGen = 0;

    QVector<KitchenFineCardEntry> mLiveCardEntries;
    QVector<KitchenFineCardEntry> mHistoryCardEntries;

    bool mDidInitialShowEventLiveLoad = false;

    const QVector<HallItem> *mHalls = nullptr;
    const QVector<TableItem> *mTables = nullptr;
    const QVector<GoodsGroupItem *> *mGroups = nullptr;
    const QVector<DishAItem *> *mDishes = nullptr;

    QWidget *mCardsRoot = nullptr;
    QHBoxLayout *mCardsColumnsLayout = nullptr;
    QVector<QWidget *> mColumnWidgets;
    QVector<QVBoxLayout *> mColumnLayouts;

    void clearAllColumns();
    void ensureColumnCount(int count);
    void trimColumnsTo(int count);
    int cardsViewportHeight() const;
    void updateCardsScrollMetrics(int columnCount);
    void rebuildCardsFromOrders(const QJsonArray &orders, bool readonly, QVector<KitchenFineCardEntry> &targetEntries);

    void reloadLiveKitchenList();
    void loadKitchenHistoryForDate(const QString &historyDateYmd);
    void setKitchenHistoryReadonly(bool readonly, const QString &historyDateYmd = QString());

    void ingestLiveKitchenDoc(const QJsonObject &jdoc);
    void ingestHistoryKitchenDoc(const QJsonObject &jdoc);

    void liveClearCards();
    void historyClearCards();
    void liveRebuildCards();
    void historyRebuildCards();
    void refreshLiveCardColors();
    void liveReflowCardsFromBuffer();
    void historyReflowCardsFromBuffer();

    QJsonArray liveOrdersForView() const;
    QJsonArray historyOrdersForView() const;

    void syncTopTabs();
    void updateLiveCountBadge(int count);

    KitchenFineCardColor resolveCardColor(const QJsonArray &lines, bool historyMode) const;
    QString cardStatusButtonText(KitchenFineCardColor color) const;
    QString kitchenStatusText(int status) const;

    QVector<KitchenLinePick> buildLinePicks(const QJsonArray &lines) const;
    KitchenFineOrderCard *createOrderCard(const QJsonObject &order,
                                          const QJsonArray &lines,
                                          const QVector<KitchenLinePick> &picks,
                                          bool readonly,
                                          int cardIndex);

    void applyKitchenLineStatusToLines(const QVector<QString> &lineIds, int status);
    void runKitchenStatusUpdateChain(const QVector<QString> &lineIds, int index, int status);
    void openStatusPickerForLineIds(const QVector<QString> &lineIds);
    void openStatusPickerForCard(int cardIndex, const QString &singleLineId = QString());
    void openDlgOrderForCard(int cardIndex);

    static int overdueMinutes();
};
