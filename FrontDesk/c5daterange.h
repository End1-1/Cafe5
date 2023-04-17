#ifndef C5DATERANGE_H
#define C5DATERANGE_H

#include "c5dialog.h"

namespace Ui {
class C5DateRange;
}

class C5DateRange : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5DateRange(const QStringList &dbParams);

    ~C5DateRange();

    static bool dateRange(QDate &d1, QDate &d2);

    static bool date(QDate &d);

private slots:
    void on_btnCancel_clicked();

    void on_btnOk_clicked();

private:
    Ui::C5DateRange *ui;
};

#endif // C5DATERANGE_H
