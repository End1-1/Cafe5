#ifndef C5SELECTTAXPRINTDIALOG_H
#define C5SELECTTAXPRINTDIALOG_H

#include "c5dialog.h"

namespace Ui {
class C5SelectTaxPrintDialog;
}

class C5SelectTaxPrintDialog : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5SelectTaxPrintDialog(const QString &id, const QStringList &dbParams);

    ~C5SelectTaxPrintDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::C5SelectTaxPrintDialog *ui;

    QString fId;
};

#endif // C5SELECTTAXPRINTDIALOG_H
