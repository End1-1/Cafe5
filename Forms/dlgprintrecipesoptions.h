#ifndef DLGPRINTRECIPESOPTIONS_H
#define DLGPRINTRECIPESOPTIONS_H

#include "c5dialog.h"

namespace Ui {
class DlgPrintRecipesOptions;
}

class DlgPrintRecipesOptions : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPrintRecipesOptions(QWidget *parent = nullptr);
    ~DlgPrintRecipesOptions();

private slots:
    void on_btnPrintWithPrices_clicked();

    void on_btnPrintWithoutPrices_clicked();

    void on_btnCancel_clicked();

private:
    Ui::DlgPrintRecipesOptions *ui;
};

#endif // DLGPRINTRECIPESOPTIONS_H
