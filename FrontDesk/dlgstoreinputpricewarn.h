#ifndef DLGSTOREINPUTPRICEWARN_H
#define DLGSTOREINPUTPRICEWARN_H

#include <QDialog>

namespace Ui
{
class DlgStoreInputPriceWarn;
}

enum StoreInputPriceWarnMode {
    StoreInputPriceWarnOff = 0,
    StoreInputPriceWarnOnSave = 1,
    StoreInputPriceWarnOnFocus = 2
};

class DlgStoreInputPriceWarn : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStoreInputPriceWarn(QWidget *parent = nullptr);

    ~DlgStoreInputPriceWarn() override;

    void setMode(int mode);

    void setPercent(int percent);

    void setAllowZeroPrice(bool allow);

    int mode() const;

    int percent() const;

    bool allowZeroPrice() const;

    static bool edit(int &mode, int &percent, bool &allowZeroPrice, QWidget *parent = nullptr);

private:
    Ui::DlgStoreInputPriceWarn *ui;
};

#endif // DLGSTOREINPUTPRICEWARN_H
