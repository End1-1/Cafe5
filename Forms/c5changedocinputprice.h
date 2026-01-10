#ifndef C5CHANGEDOCINPUTPRICE_H
#define C5CHANGEDOCINPUTPRICE_H

#include "c5officedialog.h"

namespace Ui
{
class C5ChangeDocInputPrice;
}

class C5ChangeDocInputPrice : public C5OfficeDialog
{
    Q_OBJECT

public:
    explicit C5ChangeDocInputPrice(C5User *user, const QString &uuid);

    ~C5ChangeDocInputPrice();

    static void changePrice(C5User *user, const QString &uuid);

private slots:
    void on_btnCancel_clicked();

    void on_btnChange_clicked();

private:
    Ui::C5ChangeDocInputPrice* ui;

};

#endif // C5CHANGEDOCINPUTPRICE_H
