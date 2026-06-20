#pragma once

#include "c5dialog.h"
#include <QJsonArray>
#include <QList>

class QLineEdit;
class QToolButton;
class QLabel;
class QDoubleSpinBox;
class QVBoxLayout;
class QGridLayout;

class C5CashTransfer : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5CashTransfer(C5User *user, QWidget *parent = nullptr);
    ~C5CashTransfer() override;

    void setSourceCashbox(int cashboxId, const QString &name);
    void setCurrencyId(int currencyId);

private slots:
    void on_btnSource_clicked();
    void on_btnDest_clicked();
    void on_btnSave_clicked();
    void on_btnCancel_clicked();

private:
    struct FundRow {
        int paymentTypeId = 0;
        double available = 0.0;
        QDoubleSpinBox *amount = nullptr;
    };

    void loadSourceFunds();
    void clearFundRows();
    void rebuildFundRows(const QJsonArray &funds);

    QLineEdit *mLeSource = nullptr;
    QLineEdit *mLeDest = nullptr;
    QLineEdit *mLeComment = nullptr;
    QGridLayout *mFundsGrid = nullptr;
    QLabel *mFundsHint = nullptr;
    QList<FundRow> mRows;

    int mSourceId = 0;
    int mDestId = 0;
    int mCurrencyId = 1;
};
