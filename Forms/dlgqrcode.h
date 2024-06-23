#ifndef DLGQRCODE_H
#define DLGQRCODE_H

#include "c5dialog.h"

namespace Ui
{
class DlgQrCode;
}

class DlgQrCode : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgQrCode(int dish);
    ~DlgQrCode();

private:
    Ui::DlgQrCode *ui;

private slots:
    void openResponse(const QJsonObject &jdoc);
    void appendResponse(const QJsonObject &jdoc);
    void removeResponse(const QJsonObject &jdoc);
    void on_btnEdit_clicked();
    void on_btnClose_clicked();
    void on_btnSave_clicked();
};

#endif // DLGQRCODE_H
