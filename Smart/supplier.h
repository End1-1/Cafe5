#ifndef SUPPLIER_H
#define SUPPLIER_H

#include "c5dialog.h"

namespace Ui
{
class supplier;
}

class supplier : public C5Dialog
{
    Q_OBJECT

public:
    explicit supplier();
    ~supplier();
    static bool getSupplier(int& id, QString &name);

private slots:
    void on_btnCancel_clicked();
    void on_btnOK_clicked();

private:
    Ui::supplier* ui;
    int fId;
    QString fName;
};

#endif // SUPPLIER_H
