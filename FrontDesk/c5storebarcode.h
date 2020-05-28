#ifndef C5STOREBARCODE_H
#define C5STOREBARCODE_H

#include "c5widget.h"
#include <QPrintDialog>

namespace Ui {
class C5StoreBarcode;
}

class C5StoreBarcode : public C5Widget
{
    Q_OBJECT

public:
    explicit C5StoreBarcode(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5StoreBarcode();

    void addRow(const QString &name, const QString &barcode, int qty);

    virtual QToolBar *toolBar() override;

private:
    Ui::C5StoreBarcode *ui;

    bool printOneBarcode(const QString &code, const QString &price, QPrintDialog &pd);

private slots:
    void print();

    void setList();

};

#endif // C5STOREBARCODE_H
