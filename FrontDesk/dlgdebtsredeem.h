#pragma once

#include <QDialog>

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

    int partnerId() const;
    double amount() const;
    int cashboxId() const;
    QString comment() const;
    QString redeemDateMysql() const;

private slots:
    void tryAccept();

private:
    Ui::DlgDebtsRedeem *ui;
};
