#ifndef DLGDRIVERROUTEDATE_H
#define DLGDRIVERROUTEDATE_H

#include "c5dialog.h"

namespace Ui {
class DlgDriverRouteDate;
}

class DlgDriverRouteDate : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgDriverRouteDate(const QStringList &dbParams, QWidget *parent = nullptr);
    ~DlgDriverRouteDate();
    static bool getDate(const QStringList &dbParams, QDate &date, int &driver, QWidget *parent);

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::DlgDriverRouteDate *ui;
};

#endif // DLGDRIVERROUTEDATE_H
