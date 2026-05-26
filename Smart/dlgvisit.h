#ifndef DLGVISIT_H
#define DLGVISIT_H

#include "c5dialog.h"

namespace Ui
{
class dlgvisit;
}

class dlgvisit : public C5Dialog
{
    Q_OBJECT
public:
    explicit dlgvisit(const QString &code, C5User *user);
    ~dlgvisit();
private slots:
    void on_btnClose_clicked();

    void on_btnVisit_clicked();

private:
    Ui::dlgvisit* ui;
    QString fCode;
    double fBalance;
    double fPrice;
};

#endif // DLGVISIT_H
