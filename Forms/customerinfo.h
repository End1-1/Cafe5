#ifndef CUSTOMERINFO_H
#define CUSTOMERINFO_H

#include "c5dialog.h"

namespace Ui
{
class CustomerInfo;
}

class CustomerInfo : public C5Dialog
{
    Q_OBJECT

public:
    explicit CustomerInfo(C5User *user);

    ~CustomerInfo();
    
    static bool getCustomer(int& id, QString &name, QString &phone, QString &address, C5User *user);

    virtual bool eventFilter(QObject *o, QEvent *e) override;

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_lePhone_returnPressed();

    void on_btnKbdUS_clicked(bool checked);

    void on_btnKdbAM_clicked(bool checked);

    void on_btnExtendedVersion_clicked();

private:
    Ui::CustomerInfo* ui;
    int fCustomerId;
};
#endif // CUSTOMERINFO_H
