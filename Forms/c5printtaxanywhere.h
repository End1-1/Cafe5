#ifndef C5PRINTTAXANYWHERE_H
#define C5PRINTTAXANYWHERE_H

#include "c5dialog.h"
#include <QJsonObject>

namespace Ui {
class C5PrintTaxAnywhere;
}

class C5PrintTaxAnywhere : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5PrintTaxAnywhere(const QStringList &dbParams, const QString &id);

    ~C5PrintTaxAnywhere();

    QString fReceiptNumber;

private slots:
    void timeout();

    void result(const QJsonObject &jo);

    void on_btnPrintTax_clicked();

private:
    Ui::C5PrintTaxAnywhere *ui;

    QString fId;

    QMovie *fMovie;

    int fTimeout;
};

#endif // C5PRINTTAXANYWHERE_H
