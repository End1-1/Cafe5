#pragma once

#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

class C5User;

namespace Ui
{
class DlgSoldItemOrders;
}

class DlgSoldItemOrders : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSoldItemOrders(C5User *user,
                               int goodsId,
                               const QString &goodsName,
                               const QString &date1,
                               const QString &date2,
                               int groupId,
                               int fiscalReceipt,
                               QWidget *parent = nullptr);
    ~DlgSoldItemOrders() override;

private slots:
    void onTableDoubleClicked(int row, int column);

private:
    void loadData();
    void fillTable(const QJsonObject &response);
    void openOrder(const QString &orderId);

    Ui::DlgSoldItemOrders *ui = nullptr;
    C5User *mUser = nullptr;
    int mGoodsId = 0;
    QString mGoodsName;
    QString mDate1;
    QString mDate2;
    int mGroupId = 0;
    int mFiscalReceipt = 0;
};
