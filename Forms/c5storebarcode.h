#ifndef C5STOREBARCODE_H
#define C5STOREBARCODE_H

#include "c5widget.h"
#include <QPrintDialog>

namespace Ui
{
class C5StoreBarcode;
}

class C5StoreBarcode : public C5Widget
{
    Q_OBJECT

public:
    explicit C5StoreBarcode(QWidget *parent = nullptr);

    ~C5StoreBarcode();

    QString fCurrencyName;

    void addRow(const QString &name, const QString &barcode, int qty, int curr, const QString &sizes);

    virtual QToolBar* toolBar() override;

    static bool printOneBarcodeIllure(const QString &code, const QString &price, const QString &class1, const QString &name, QPrintDialog &pd);

    static bool printOneBarcode(const QString &code, const QString &price, const QString &class1, const QString &name, QPrintDialog &pd);

    /** Compact label 60×30 mm: name (2 lines) left, price+դր. right, barcode bottom. */
    static bool printOneBarcode60x30(const QString &code, const QString &price, const QString &name, QPrintDialog &pd);

    /** Label 57×30 mm: shop name, goods name, QR right, price + readable code bottom-left. */
    static bool printOneBarcode57x30Qr(const QString &code, const QString &price, const QString &name, QPrintDialog &pd);

    static bool printOneBarcode(const QString &code, QPrintDialog &pd);

    static bool printOneBarcode2(const QString &code, const QString &price, QString link, const QString &name, const QString &sizeList, QPrintDialog &pd);

    static constexpr const char *kTemplate60x30 = "60x30";
    static constexpr const char *kTemplate57x30Qr = "57x30_qr";

    static QString currentLabelTemplateId();
    static void setCurrentLabelTemplateId(const QString &id);
    static QString currentLabelFontFamily();
    static void setCurrentLabelFontFamily(const QString &family);
    /** Company name from sys_json_config of the current settings profile. */
    static QString configShopName();
    /** Dispatch to selected label template (default 60x30). */
    static bool printSelected(const QString &code, const QString &price, const QString &name, QPrintDialog &pd);

private:
    Ui::C5StoreBarcode* ui;

    //bool printOneBarcode(const QString &code, QPrintDialog &pd);

private slots:
    virtual void setSearchParameters();

    void print();

    void print2();

    void printDescriptions();

    void setList();

    void setQtyToOne();

};

#endif // C5STOREBARCODE_H
