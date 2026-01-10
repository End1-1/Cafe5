#ifndef CHANGE_H
#define CHANGE_H

#include "c5dialog.h"

namespace Ui {
class Change;
}

class Change : public C5Dialog
{
    Q_OBJECT

public:
    explicit Change(C5User *user);
    ~Change();
    static bool getReceived(double &v, C5User *user);

private slots:
    void buttonClicked();
    void buttonClicked2();
    void on_leReceived_textChanged(const QString &arg1);
    void on_btnCancel_clicked();
    void on_btnOK_clicked();
    void on_leReceived_returnPressed();

private:
    Ui::Change *ui;
    QList<QPushButton*> fButtons;
    QList<QPushButton*> fButtons2;
};

#endif // CHANGE_H
