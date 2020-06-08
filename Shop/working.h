#ifndef WORKING_H
#define WORKING_H

#include "goods.h"
#include "c5storedraftwriter.h"
#include <QWidget>

namespace Ui {
class Working;
}

class QTableWidgetItem;
class QTabWidget;

struct IUser {
    int id;
    int group;
    QString name;
    QPixmap photo;
};

class Working : public QWidget
{
    Q_OBJECT

public:
    explicit Working(QWidget *parent = nullptr);

    ~Working();

    bool eventFilter(QObject *watched, QEvent *event);

    QString goodsCode(int code) const;

    bool getAdministratorRights();

    void decQty(const IGoods &g);

    static QMap<QString, Goods> fGoods;

    static QHash<int, QString> fGoodsCodeForPrint;

    static QHash<QString, int> fGoodsRows;

    QList<IUser> fCurrentUsers;

    QTabWidget *fTab;

private:
    Ui::Working *ui;

    void makeWGoods();

    void loadStaff();

    void addGoods(QString &code);

    void newSale(int type);

    int ordersCount();

    int fTimerCounter;

    bool fHaveChanges;

    bool fUpFinished;

private slots:
    void timeout();

    void uploadDataFinished();

    void escape();

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

    void on_btnNewOrder_clicked();

    void on_btnConnection_clicked();

    void on_leCode_returnPressed();

    void on_btnSaveOrder_clicked();

    void on_btnSaveOrderNoTax_clicked();

    void on_btnExit_clicked();

    void on_btnShowGoodsList_clicked();

    void on_tblGoods_itemClicked(QTableWidgetItem *item);

    void on_btnDuplicateReceipt_clicked();

    void on_leCode_textChanged(const QString &arg1);

    void on_btnNewWhosale_clicked();

    void on_lePartner_returnPressed();

    void on_tab_tabCloseRequested(int index);

    void on_btnItemBack_clicked();

    void on_btnStoreInput_clicked();
};

#endif // WORKING_H
