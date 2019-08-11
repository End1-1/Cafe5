#ifndef WORKING_H
#define WORKING_H

#include <QWidget>
#include <QTimer>

namespace Ui {
class Working;
}

class QTableWidgetItem;

class Working : public QWidget
{
    Q_OBJECT

public:
    explicit Working(QWidget *parent = nullptr);

    ~Working();

    bool eventFilter(QObject *watched, QEvent *event);

    QString goodsCode(int code) const;

    void markDiscount(const QString &customer);

    bool getAdministratorRights();

private:
    Ui::Working *ui;

    QTimer fTimer;

    int fTimerTimeout;

    void getGoodsList();

    void makeWGoods();

private slots:
    void timeout();

    void shortcutF1();

    void shortcutF2();

    void shortcutF3();

    void shortcutF4();

    void shortcutDown();

    void shortcutUp();

    void on_btnNewOrder_clicked();

    void on_btnConnection_clicked();

    void on_leCode_returnPressed();

    void on_btnSaveOrder_clicked();

    void on_btnSaveOrderNoTax_clicked();

    void on_btnExit_clicked();

    void on_btnShowGoodsList_clicked();

    void on_tblGoods_itemClicked(QTableWidgetItem *item);

    void on_btnRefund_clicked();

    void on_btnDuplicateReceipt_clicked();
};

#endif // WORKING_H
