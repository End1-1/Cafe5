#ifndef C5INPUTDATE_H
#define C5INPUTDATE_H

#include "c5dialog.h"

namespace Ui {
class C5InputDate;
}

class C5InputDate : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5InputDate(const QStringList &dbParams);

    ~C5InputDate();

    static bool date(const QStringList &dbParams, QDate &d, int &shift, QString &shiftName);

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::C5InputDate *ui;
};

#endif // C5INPUTDATE_H
