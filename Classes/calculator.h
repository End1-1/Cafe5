#ifndef CALCULATOR_H
#define CALCULATOR_H

#include "c5dialog.h"

namespace Ui {
class Calculator;
}

class Calculator : public C5Dialog
{
    Q_OBJECT

public:
    explicit Calculator(C5User *user);

    ~Calculator();

    static bool get(double &v, C5User *user);

    bool event(QEvent *e) override;

private:
    Ui::Calculator *ui;

    int fOperation;

    int fOperationStart;

    double fNumber;

    bool fNumberStart;

    double fResult;

    void newKey(int key);

private slots:

    void buttonPressed();

    void on_btnBackspace_clicked();

    void on_btnSign_clicked();

    void on_btnClear_clicked();

    void actionKey();
};

#endif // CALCULATOR_H
