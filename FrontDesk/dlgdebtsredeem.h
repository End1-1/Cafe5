#pragma once

#include <QDialog>
#include <QJsonObject>

namespace Ui
{
class DlgDebtsRedeem;
}

class DlgDebtsRedeem : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDebtsRedeem(QWidget *parent, int docType, int currencyId);

    ~DlgDebtsRedeem() override;

    void loadRedeem(const QJsonObject &redeem);

    int debtId() const;
    int partnerId() const;
    double amount() const;
    int cashboxId() const;
    int paymentTypeId() const;
    QString comment() const;
    QString redeemDateMysql() const;
    bool isEditMode() const;
    bool deleteRequested() const;

private slots:
    void tryAccept();
    void onDeleteClicked();

private:
    Ui::DlgDebtsRedeem *ui;
    int mDebtId = 0;
    bool mDeleteRequested = false;
};
