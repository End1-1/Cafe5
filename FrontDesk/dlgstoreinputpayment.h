#ifndef DLGSTOREINPUTPAYMENT_H
#define DLGSTOREINPUTPAYMENT_H

#include <QDialog>
#include <QString>
#include <QVector>

namespace Ui
{
class DlgStoreInputPayment;
}

struct StorePaymentPreset
{
    int cashboxId = 0;
    QString cashboxName;
    int paymentTypeId = 0;
    QString paymentTypeName;
    int currencyId = 0;
    QString currencyName;

    QString label() const;
    bool isUnpaid() const { return cashboxId <= 0 || paymentTypeId <= 0; }
    bool matches(const StorePaymentPreset &o) const;
};

class DlgStoreInputPayment : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStoreInputPayment(QWidget *parent = nullptr);

    ~DlgStoreInputPayment() override;

    void setPresets(const QVector<StorePaymentPreset> &presets, const StorePaymentPreset &selected);

    QVector<StorePaymentPreset> presets() const;

    StorePaymentPreset selectedPreset() const;

    static bool edit(QVector<StorePaymentPreset> &presets, StorePaymentPreset &selected, QWidget *parent = nullptr);

private slots:
    void tryAccept();
    void onAddPreset();
    void onDeletePreset();
    void onPresetSelectionChanged();

private:
    Ui::DlgStoreInputPayment *ui;

    QVector<StorePaymentPreset> mPresets;

    bool readForm(StorePaymentPreset *out, bool showErrors) const;

    void writeForm(const StorePaymentPreset &preset);

    void rebuildList(int selectIndex = -1);

    int findPresetIndex(const StorePaymentPreset &preset) const;
};

#endif // DLGSTOREINPUTPAYMENT_H
