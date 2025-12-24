#ifndef C5DISCOUNTREDEEM_H
#define C5DISCOUNTREDEEM_H

#include "c5dialog.h"

namespace Ui
{
class C5DiscountRedeem;
}

class C5DiscountRedeem : public C5Dialog
{
    Q_OBJECT
public:
    explicit C5DiscountRedeem(C5User *user, const QString &id, int partnerId);
    ~C5DiscountRedeem();
    virtual void selectorCallback(int selector, const QJsonArray &data) override;
private slots:
    void on_btnCancel_clicked();

    void on_btnRedeem_clicked();

    void on_leAmountToRedeem_textChanged(const QString &arg1);

    void on_cbCard_currentIndexChanged(int index);

    void on_btnDeleteRecord_clicked();

private:
    Ui::C5DiscountRedeem* ui;

    void processPartner(const QJsonArray &data);

};

#endif // C5DISCOUNTREDEEM_H
