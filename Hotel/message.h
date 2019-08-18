#ifndef MESSAGE_H
#define MESSAGE_H

#include "dialog.h"

namespace Ui {
class Message;
}

class Message : public Dialog
{
    Q_OBJECT

public:
    enum Result {rCancel = 0, rYes, rNo, rClose};
    explicit Message();
    ~Message();
    static void showMessage(const QString &msg);
    static Result yesNoCancel(const QString &msg);
    static Result yesNo(const QString &msg);

protected:
    virtual void setWidgetContainer() override;

private slots:
    void on_btnYes_clicked();
    void on_btnClose_clicked();
    void on_btnNo_clicked();
    void on_btnCancel_clicked();

private:
    Ui::Message *ui;
    Result fResult;
};

#endif // MESSAGE_H
