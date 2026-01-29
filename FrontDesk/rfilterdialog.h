#pragma once

#include "c5dialog.h"
#include <QJsonArray>

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

private:
    virtual void showEvent(QShowEvent *e) override;

private slots:
    void on_btnCancel_clicked();

    void on_btnApply_clicked();

private:
    Ui::RFilterDialog* ui;

    QString mSettingsPrefix;
};
