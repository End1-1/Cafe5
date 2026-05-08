#pragma once

#include "c5dialog.h"
#include <QJsonArray>
#include <QVariant>

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

    void buildWidget(const QString &settingsPrefix, const QJsonArray &ja);

    QJsonArray filterValues();

    // Returns value from widget by its "name" property.
    // C5CodeNameSelector => kv->value() (int)
    // QComboBox => cb->currentData().toInt()
    // C5DateEdit => ed->date() as MySQL date string
    // C5LineEdit => en->getInteger()
    QVariant valueByName(const QString &name) const;

private:
    virtual void showEvent(QShowEvent *e) override;

private slots:
    void on_btnCancel_clicked();

    void on_btnApply_clicked();

private:
    Ui::RFilterDialog* ui;

    QString mSettingsPrefix;
};
