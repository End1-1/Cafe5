#pragma once

#include "c5dialog.h"
#include <QJsonArray>
#include <QVariant>

class QToolButton;

namespace Ui
{
class RFilterDialog;
}

class RFilterDialog : public C5Dialog
{
    Q_OBJECT
public:
    explicit RFilterDialog(C5User *user);

    ~RFilterDialog();

    void buildWidget(const QString &settingsPrefix, const QJsonArray &ja,
                     const QString &pinKey = QString(), const QJsonArray &currentValues = QJsonArray());

    QJsonArray filterValues();

    // Pinned: report dates are restored from the last session; otherwise they start at today.
    static bool datesPinned(const QString &pinKey);

    static void setDatesPinned(const QString &pinKey, bool pinned);

    // Replaces every yyyy-MM-dd value with today's date.
    static void resetDatesToToday(QJsonArray &values);

    // Returns value from widget by its "name" property.
    // C5CodeNameSelector => kv->value() (int)
    // QComboBox => cb->currentData().toInt()
    // QCheckBox => 1 if checked, otherwise 0
    // C5DateEdit => ed->date() as MySQL date string
    // C5LineEdit => en->getInteger()
    QVariant valueByName(const QString &name) const;

private:
    virtual void showEvent(QShowEvent *e) override;

    void setPinButtonsChecked(bool pinned);

private slots:
    void on_btnCancel_clicked();

    void on_btnApply_clicked();

private:
    Ui::RFilterDialog* ui;

    QString mSettingsPrefix;

    QString mPinKey;

    QList<QToolButton*> mPinButtons;
};
