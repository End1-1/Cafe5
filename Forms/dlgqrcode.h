#ifndef DLGQRCODE_H
#define DLGQRCODE_H

#include <QDialog>

namespace Ui {
class DlgQrCode;
}

class DlgQrCode : public QDialog
{
    Q_OBJECT

public:
    explicit DlgQrCode(QWidget *parent = nullptr);
    ~DlgQrCode();

private:
    Ui::DlgQrCode *ui;
};

#endif // DLGQRCODE_H
