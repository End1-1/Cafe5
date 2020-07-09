#ifndef SELECTPRINTERS_H
#define SELECTPRINTERS_H

#include "c5dialog.h"

namespace Ui {
class SelectPrinters;
}

class SelectPrinters : public C5Dialog
{
    Q_OBJECT

public:
    explicit SelectPrinters();

    ~SelectPrinters();

    static bool selectPrinters(bool &p1, bool &p2);

private slots:

    void on_btnCancel_clicked();

    void on_btnOk_clicked();

private:
    Ui::SelectPrinters *ui;
};

#endif // SELECTPRINTERS_H
