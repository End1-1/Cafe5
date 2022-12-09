#ifndef SESSIONSHISTORY_H
#define SESSIONSHISTORY_H

#include "c5dialog.h"

namespace Ui {
class SessionsHistory;
}

class SessionsHistory : public C5Dialog
{
    Q_OBJECT

public:
    explicit SessionsHistory(const QDate &d1, const QDate &d2);
    ~SessionsHistory();

private slots:
    void on_btnPrintReport_clicked();

    void on_btnCancel_clicked();

    void on_btnPrintSession_clicked();

private:
    Ui::SessionsHistory *ui;
    QDate fDate1;
    QDate fDate2;
};

#endif // SESSIONSHISTORY_H
