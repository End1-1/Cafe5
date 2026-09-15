#ifndef WORKING_H
#define WORKING_H

#include <QJsonObject>
#include <QPixmap>
#include <QSet>
#include <QStringList>
#include <functional>
#include "c5shopdialog.h"

namespace Ui
{
class Working;
}

class QTableWidgetItem;
class QTabWidget;
class C5User;
class WOrder;

struct IUser {
    int id;
    int group;
    QString name;
    QPixmap photo;
};

struct Flag {
    int id;
    int enabled;
    QString field;
    QString name;
};

class WCustomerDisplay;
class WSession;
class QMovie;
class C5User;

class Working : public C5ShopDialog
{
    Q_OBJECT

public:
    explicit Working(C5User *user, QWidget *parent = nullptr);

    ~Working();

    WOrder* worder();

    WCustomerDisplay* customerDisplay() const { return fCustomerDisplay; }

    /** Repaint buyer display from the current sale tab (or clear if none). */
    void refreshCustomerDisplay();

    WOrder* newSale(int type);

    /** Open a tab for an already-reopened order (make-draft). */
    WOrder* openExistingSale(const QJsonObject &orderResponse);

    bool eventFilter(QObject* watched, QEvent* event);

    void setActiveWidget(WOrder* w);

    static Working* working();

    int cashSessionId() const;

    bool hasActiveSession() const;

    /** Tables from h_tables for shop hall (ordered by f_id). */
    const QList<int>& shopTableIds() const { return mShopTableIds; }

    /** First hall table not used by an open sale tab. */
    int allocateFreeTableId() const;

    static QMap<int, Flag> fFlags;

    static QHash<QString, int> fGoodsRows;

    static QHash<QString, QString> fMultiscancode;

    static QMap<QString, double> fUnitDefaultQty;

    QList<IUser> fCurrentUsers;

    QTabWidget* fTab;

    Flag flag(int id);

    void loadStaff(std::function<void()> next = nullptr);

public slots:
    void getGoods(int id, const QString &scancode, double stockQty = -1);

private slots:
    void on_btnServiceCheck_clicked();

private slots:
    void on_btnProgressWindow_clicked();

private slots:
    void on_btnAttendance_clicked();

private:
    Ui::Working* ui;

    static Working* fInstance;

    NInterface* fHttp;

    WCustomerDisplay* fCustomerDisplay = nullptr;

    int ordersCount();

    int fTimerCounter;

    bool fHaveChanges;

    bool fUpFinished;

    QMovie* mMovie;

    void openSearch();

    void checkCashboxSession();

    void showSessionWidget();

    void applyCashboxSession(const QJsonObject &session);

    void clearCashboxSession();

    void refreshCashboxSession();

    void updateSessionUi();

    void updateWsStatus();

    void setSaleControlsEnabled(bool enabled);

    void printCloseSessionReport(const QJsonObject &cashbox, bool cashCounted);

    void printDifferenceAct(const QJsonObject &cashbox);

    /** Site sales: print receipt duplicate + service check, then mark f_state=2. */
    void enqueueSiteSalePrint(const QString &orderId);
    void fetchPendingSitePrints();
    void processSitePrintQueue();
    void printSiteSale(const QString &orderId);
    void printSiteServiceCheck(const QJsonObject &jdoc);
    void finishSiteSalePrint(const QString &orderId);

    int mCashSessionId = 0;

    QJsonObject mCashboxSessionData;

    QList<int> mShopTableIds;

    QStringList mSitePrintQueue;
    QSet<QString> mSitePrintInProgress;
    bool mSitePrintBusy = false;

    void loadShopTables(std::function<void()> next);

private slots:
    void orderSaved(QWidget* w);

    void timeout();

    void onWsTextMessage(const QString &message);

    void onCtrlI();

    void onCtrlO();

    void onCtrlL();

    void checkMessageResponse(const QJsonObject &jdoc);

    void uploadDataFinished();

    void shortcutEscape();

    void shortcutMinus();

    void shortcutPlus();

    void shortcutAsterix();

    void shortcutF1();

    void shortcutF2();

    void shortcutF7();

    void shortcutF8();

    void shortcutF9();

    void shortcutF12();

    void shortcutDown();

    void shortcutUp();

    void shortcutComma();

    void qtyRemains(const QJsonObject &jdoc);

    void haveChanges(bool v);

    void on_tab_tabCloseRequested(int index);

    void on_tab_currentChanged(int index);

    void on_btnCloseApplication_clicked();

    void on_btnWriteOrder_clicked();

    void on_btnNewRetail_clicked();

    void on_btnNewWhosale_clicked();

    void on_btnSalesReport_clicked();

    void on_btnHelp_clicked();

    void on_btnMinimize_clicked();

    void on_btnGiftCard_clicked();

    void on_btnCostumerDisplay_clicked(bool checked);

    void on_btnColumns_clicked();

    void on_chRegisterCard_clicked();

    void on_btnCashout_clicked();

    void on_btnBooking_clicked();

    void on_btnCloseSession_clicked();

    void onSessionOpened(const QJsonObject &session);
};

#endif // WORKING_H
