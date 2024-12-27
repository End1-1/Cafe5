#ifndef CASHCOLLECTION_H
#define CASHCOLLECTION_H

#include "c5dialog.h"

namespace Ui
{
class CashCollection;
}

class CashCollection : public C5Dialog
{
    Q_OBJECT

public:
    explicit CashCollection();

    ~CashCollection();

private slots:
    void on_btnCancel_clicked();

    void on_btnSave_clicked();

    void on_leAmount_textChanged(const QString &arg1);

    void responseOfCreate(const QJsonObject &jdoc);

    void responseOfCreateIn(const QJsonObject &jdoc);

private:
    Ui::CashCollection *ui;

    double fMax;

    int fCoinCashId;

    void collectCash();
};

#endif // CASHCOLLECTION_H
