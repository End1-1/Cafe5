#ifndef DLGSTOREINPUTPAYMENT_H
#define DLGSTOREINPUTPAYMENT_H

#include <QDialog>
#include <QPair>
#include <QString>
#include <QVector>

namespace Ui
{
class DlgStoreInputPayment;
}

/** One selectable payment row in store-input combo (runtime). */
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

/** Fixed payment methods enabled for store input (persisted). */
struct StorePaymentOptions
{
    bool unpaidEnabled = true;
    /** Cashbox ids allowed for cash payment. Empty = none. */
    QVector<int> enabledCashboxIds;
    bool cardEnabled = false;
    bool bankEnabled = false;
    int cardCashboxId = 0;
    QString cardCashboxName;
    int bankCashboxId = 0;
    QString bankCashboxName;
    bool bank2Enabled = false;
    int bank2CashboxId = 0;
    QString bank2CashboxName;
    bool bank3Enabled = false;
    int bank3CashboxId = 0;
    QString bank3CashboxName;
    /** True after load() saw cashbox ids key (false = first run, select all). */
    bool cashboxesConfigured = false;
    /** Settings group: each document kind keeps its own payment list. */
    QString settingsGroup = QStringLiteral("StoreInputPayment");

    void load();
    void save() const;

    bool cashEnabled() const { return !enabledCashboxIds.isEmpty(); }

    /** Build combo presets from options + cashbox list. */
    QVector<StorePaymentPreset> buildPresets(const QVector<QPair<int, QString>> &cashboxes) const;
};

class DlgStoreInputPayment : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStoreInputPayment(QWidget *parent = nullptr);

    ~DlgStoreInputPayment() override;

    void setCashboxes(const QVector<QPair<int, QString>> &cashboxes);

    void setOptions(const StorePaymentOptions &options);

    StorePaymentOptions options() const;

    /** unpaidText replaces the "Unpaid" option caption (e.g. "Not from cashbox" for salary). */
    static bool configure(StorePaymentOptions &options,
                          const QVector<QPair<int, QString>> &cashboxes,
                          QWidget *parent = nullptr,
                          const QString &unpaidText = QString(),
                          const QString &hintText = QString());

private slots:
    void tryAccept();
    void syncCashboxEnabled();

private:
    Ui::DlgStoreInputPayment *ui;

    void fillCashboxList(const QVector<QPair<int, QString>> &cashboxes,
                         const QVector<int> &checkedIds,
                         bool checkAllIfEmpty);
};

#endif // DLGSTOREINPUTPAYMENT_H
