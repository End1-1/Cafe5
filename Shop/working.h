#ifndef WORKING_H
#define WORKING_H

#include "c5storedraftwriter.h"
#include <QWidget>

namespace Ui {
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
    QString field;
    QString name;
};

class WCustomerDisplay;

class Working : public QWidget
{
    Q_OBJECT

public:
    explicit Working(C5User *user, QWidget *parent = nullptr);

    ~Working();

    WOrder *worder();

    WOrder *newSale(int type);

    bool eventFilter(QObject *watched, QEvent *event);

    void decQty(int id, double qty);

    void setActiveWidget(WOrder *w);

    bool findDraft(const QString &draftid);

    static Working *working();

    static QMap<int, Flag> fFlags;

    static QHash<QString, int> fGoodsRows;

    static QHash<QString,QString> fMultiscancode;

    static QMap<QString, double> fUnitDefaultQty;

    QList<IUser> fCurrentUsers;

    QTabWidget *fTab;

    Flag flag(int id);

public slots:
    void getGoods(int id);

private:
    Ui::Working *ui;

    static Working *fInstance;

    C5User *fUser;

    WCustomerDisplay *fCustomerDisplay;

    void loadStaff();

    int ordersCount();

    int fTimerCounter;

    bool fHaveChanges;

    bool fUpFinished;

private slots:

    void socketConnected();

    void socketDisconnected();

    void socketDataReceived(quint16 cmd, quint32 messageId, QByteArray d);

    void timeout();

    void uploadDataFinished();

    void threadMessageError(int code, const QString &message);

    void threadMessageData(int code, const QVariant &data);

    void shortcutEscape();

    void shortcutMinus();

    void shortcutPlus();

    void shortcutAsterix();

    void shortcutF1();

    void shortcutF2();

    void shortcutF5();

    void shortcurF6();

    void shortcutF7();

    void shortcutF8();

    void shortcutF9();

    void shortcutF10();

    void shortcutF11();

    void shortcutF12();

    void shortcutDown();

    void shortcutUp();

    void haveChanges(bool v);

    void on_tab_tabCloseRequested(int index);

    void on_btnItemBack_clicked();

    void on_tab_currentChanged(int index);

    void on_btnCloseApplication_clicked();

    void on_btnServerSettings_clicked();

    void on_btnWriteOrder_clicked();

    void on_btnGoodsMovement_clicked();

    void on_btnNewRetail_clicked();

    void on_btnNewWhosale_clicked();

    void on_btnGoodsList_clicked();

    void on_btnSalesReport_clicked();

    void on_btnDbConnection_clicked();

    void on_btnHelp_clicked();

    void on_btnManualTax_clicked();

    void on_btnMinimize_clicked();

    void on_btnClientConfigQR_clicked();

    void on_btnGiftCard_clicked();

    void on_btnCostumerDisplay_clicked(bool checked);
    void on_btnPreorder_clicked();
    void on_btnOpenDraft_clicked();
};

#endif // WORKING_H
