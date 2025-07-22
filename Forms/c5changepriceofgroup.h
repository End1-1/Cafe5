#ifndef C5CHANGEPRICEOFGROUP_H
#define C5CHANGEPRICEOFGROUP_H

#include "c5dialog.h"

namespace Ui {
class C5ChangePriceOfGroup;
}

class C5ChangePriceOfGroup : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5ChangePriceOfGroup();

    ~C5ChangePriceOfGroup();

    static bool groupPrice(double &price1, double &price2, double &price1disc, double &price2disc);

private slots:
    void on_btnOk_clicked();

    void on_btnCancel_clicked();

private:
    Ui::C5ChangePriceOfGroup *ui;
};

#endif // C5CHANGEPRICEOFGROUP_H
