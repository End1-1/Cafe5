#ifndef DLGSELECTCURRENCY_H
#define DLGSELECTCURRENCY_H

#include "c5dialog.h"

namespace Ui {
class DlgSelectCurrency;
}

class DlgSelectCurrency : public C5Dialog
{
    Q_OBJECT

public:
    explicit DlgSelectCurrency(const QStringList &dbParams, QWidget *parent = nullptr);
    ~DlgSelectCurrency();
    static bool getCurrency(const QStringList &dbParams, int &id, QString &name, QWidget *parent);

private slots:
    void on_btnReject_clicked();

    void on_btnOK_clicked();

private:
    Ui::DlgSelectCurrency *ui;
};

#endif // DLGSELECTCURRENCY_H
