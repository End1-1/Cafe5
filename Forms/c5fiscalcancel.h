#ifndef C5FISCALCANCEL_H
#define C5FISCALCANCEL_H

#include "c5dialog.h"

namespace Ui
{
class C5FiscalCancel;
}

class C5FiscalCancel : public C5Dialog
{
    Q_OBJECT
public:
    explicit C5FiscalCancel(C5User *user, const QString &id = nullptr);
    ~C5FiscalCancel();
private slots:
    void on_btnRequestCancel_clicked();

    void on_btnRefresh_clicked();

private:
    Ui::C5FiscalCancel* ui;
    QString fUuid;
    void getInfo();
};

#endif // C5FISCALCANCEL_H
