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

    QString fCurrencyName;

    void addRow(const QString &name, const QString &barcode, int qty, int curr);

    virtual QToolBar *toolBar() override;

    static bool printOneBarcode(const QString &code, const QString &price, const QString &class1, const QString &name, QPrintDialog &pd);

private:
    Ui::C5StoreBarcode *ui;

    //bool printOneBarcode(const QString &code, QPrintDialog &pd);

private slots:
    void print();

    void printDescriptions();

    void setList();

    void setQtyToOne();

};

#endif // C5STOREBARCODE_H
