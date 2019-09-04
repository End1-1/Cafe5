#ifndef LOGIN_H
#define LOGIN_H

#include "dialog.h"

namespace Ui {
class Login;
}

class Login : public Dialog
{
    Q_OBJECT

public:
    explicit Login();
    ~Login();
    static bool login();

protected:
    virtual void setWidgetContainer() override;

private slots:
    void on_btnCancel_clicked();    
    void on_btnOk_clicked();    
    void on_btnSettings_clicked();

private:
    Ui::Login *ui;
    bool success();
};

#endif // LOGIN_H
