#ifndef DLGSHOPCUSTOMER_H
#define DLGSHOPCUSTOMER_H

#include "c5shopdialog.h"
#include "struct_partner.h"

namespace Ui {
class DlgShopCustomer;
}

class DlgShopCustomer : public C5ShopDialog
{
    Q_OBJECT

public:
    explicit DlgShopCustomer(C5User *user);
    void setTin(const QString &tin);
    ~DlgShopCustomer();
    PartnerItem mPartner;
private slots:
    void on_btnSave_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgShopCustomer *ui;
};

#endif // DLGSHOPCUSTOMER_H
