#pragma once

#include <QDialog>
#include <QJsonObject>
#include <QPair>
#include <QVector>

#include "dlgstoreinputpayment.h"

class C5User;

namespace Ui
{
class DlgDebtsRedeem;
}

class DlgDebtsRedeem : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDebtsRedeem(C5User *user, QWidget *parent, int docType, int currencyId);

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
    void onPaymentChanged(int index);
    void onPaymentConfigClicked();

private:
    void initPayment();
    void rebuildPaymentCombo();
    void selectPayment(const StorePaymentPreset &preset);
    QVector<QPair<int, QString>> loadCashboxList() const;
    void clearPartnerBalance();
    void refreshPartnerBalance();
    void setPartnerBalance(double balance);

    Ui::DlgDebtsRedeem *ui;
    C5User *mUser = nullptr;
    int mDocType = 1;
    int mCurrencyId = 1;
    int mDebtId = 0;
    bool mDeleteRequested = false;
    StorePaymentOptions mPaymentOptions;
    QVector<StorePaymentPreset> mPaymentPresets;
    StorePaymentPreset mCurrentPayment;
};
