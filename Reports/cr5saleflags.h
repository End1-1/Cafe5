#ifndef CR5SALEFLAGS_H
#define CR5SALEFLAGS_H

#include "c5dialog.h"

namespace Ui {
class CR5SaleFlags;
}

class CR5SaleFlags : public C5Dialog
{
    Q_OBJECT

public:
    explicit CR5SaleFlags();

    ~CR5SaleFlags();

    QString flagsCond() const;

private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::CR5SaleFlags *ui;
};

#endif // CR5SALEFLAGS_H
