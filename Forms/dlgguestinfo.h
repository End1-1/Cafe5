#ifndef DLGGUESTINFO_H
#define DLGGUESTINFO_H

#include <QJsonObject>
#include "c5dialog.h"

namespace Ui {
class DlgGuestInfo;
}

class DlgGuestInfo : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgGuestInfo(C5User *user);

    ~DlgGuestInfo();

    void setInfo(const QJsonObject &g);

    QJsonObject getInfo() const;

private slots:
    void on_btnCancel_clicked();

    void on_btnSave_clicked();

    void on_btnEditAddress_clicked();

    void on_btnEditPhone_clicked();

    void on_btnEditContact_clicked();

private:
    Ui::DlgGuestInfo *ui;

};

#endif // DLGGUESTINFO_H
