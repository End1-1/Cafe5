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

class Working : public QWidget
{
    Q_OBJECT

public:
    explicit Working(C5User *user, QWidget *parent = nullptr);

    ~Working();

    bool eventFilter(QObject *watched, QEvent *event);

    void decQty(int id, double qty);

    static QMap<int, Flag> fFlags;

    static QHash<QString, int> fGoodsRows;

    static QHash<QString,QString> fMultiscancode;

    static QMap<QString, double> fUnitDefaultQty;

    QList<IUser> fCurrentUsers;

    QTabWidget *fTab;

    Flag flag(int id);

private:
    Ui::Working *ui;

    C5User *fUser;

    WOrder *worder();

    void loadStaff();

    void newSale(int type);

    int ordersCount();

    int fTimerCounter;

    bool fHaveChanges;

    bool fUpFinished;

    void restoreSales();

private slots:
    void socketConnected();

    void socketDisconnected();

    void socketDataReceived(quint16 cmd, QByteArray d);

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

    void shortcutF3();

    void shortcutF4();

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
};

#endif // WORKING_H
