#ifndef C5CHANGEDOCINPUTPRICE_H
#define C5CHANGEDOCINPUTPRICE_H

#include "c5dialog.h"

namespace Ui {
class C5ChangeDocInputPrice;
}

class C5ChangeDocInputPrice : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5ChangeDocInputPrice(const QStringList &dbParams, const QString &uuid);

    ~C5ChangeDocInputPrice();

    static void changePrice(const QStringList &dbParams, const QString &uuid);

private slots:
    void on_btnCancel_clicked();

    void on_btnChange_clicked();

private:
    Ui::C5ChangeDocInputPrice *ui;

};

#endif // C5CHANGEDOCINPUTPRICE_H
