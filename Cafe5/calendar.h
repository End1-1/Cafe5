#ifndef CALENDAR_H
#define CALENDAR_H

#include "c5dialog.h"

namespace Ui {
class Calendar;
}

class Calendar : public C5Dialog
{
    Q_OBJECT

public:
    explicit Calendar();

    ~Calendar();

    static bool getDate(QDate &d);

    static bool getDate2(QDate &d1, QDate &d2);

private slots:
    void on_btnCancel_clicked();

    void on_btnOk_clicked();

private:
    Ui::Calendar *ui;
};

#endif // CALENDAR_H
