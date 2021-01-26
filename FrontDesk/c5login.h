#ifndef C5LOGIN_H
#define C5LOGIN_H

#include "c5dialog.h"
namespace Ui {
class C5Login;
}

class C5Login : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5Login(const QStringList &dbParams);

    ~C5Login();

    virtual int exec() override;

private slots:
    void on_btnCancel_clicked();

    void on_btnOk_clicked();

private:
    Ui::C5Login *ui;
};

#endif // C5LOGIN_H
