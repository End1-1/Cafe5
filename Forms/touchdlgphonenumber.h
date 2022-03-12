#ifndef TOUCHDLGPHONENUMBER_H
#define TOUCHDLGPHONENUMBER_H

#include "c5dialog.h"

namespace Ui {
class TouchDlgPhoneNumber;
}

class TouchDlgPhoneNumber : public C5Dialog
{
    Q_OBJECT

public:
    explicit TouchDlgPhoneNumber();
    ~TouchDlgPhoneNumber();
    static bool getPhoneNumber(QString &number);

private slots:
    void on_btnOk_clicked();

    void on_btn0_clicked();

    void on_btn1_clicked();

    void on_btn2_clicked();

    void on_btn3_clicked();

    void on_btn4_clicked();

    void on_btn5_clicked();

    void on_btn6_clicked();

    void on_btn7_clicked();

    void on_btn8_clicked();

    void on_btn9_clicked();

    void on_btnCancel_clicked();

    void on_btnBackspace_clicked();

private:
    Ui::TouchDlgPhoneNumber *ui;
    void buttonClicked(QPushButton *b);
};

#endif // TOUCHDLGPHONENUMBER_H
