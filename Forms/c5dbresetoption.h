#ifndef C5DBRESETOPTION_H
#define C5DBRESETOPTION_H

#include "c5dialog.h"

namespace Ui {
class C5DbResetOption;
}

class C5DbResetOption : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5DbResetOption(const QStringList &dbParams);

    ~C5DbResetOption();

    bool saleAndBuy();

    bool fullReset();

private slots:
    void on_btnCancel_clicked();

    void on_btnAccept_clicked();

private:
    Ui::C5DbResetOption *ui;
};

#endif // C5DBRESETOPTION_H
