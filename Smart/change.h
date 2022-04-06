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
    explicit Change();
    ~Change();
    static bool getReceived(double &v);

private slots:
    void buttonClicked();
    void on_leReceived_textChanged(const QString &arg1);
    void on_btnCancel_clicked();
    void on_btnOK_clicked();

private:
    Ui::Change *ui;

    QList<QPushButton*> fButtons;
};

#endif // CHANGE_H
