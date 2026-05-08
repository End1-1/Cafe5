#pragma once

#include <QDialog>

class QAbstractButton;
class QListWidget;
class QListWidgetItem;

namespace Ui {
class WDashboardSettings;
}

class WDashboardSettings : public QDialog
{
    Q_OBJECT

public:
    explicit WDashboardSettings(const QList<QAbstractButton*> &buttons, const QVariantMap &visibility, QWidget *parent = nullptr);
    ~WDashboardSettings();

    QVariantMap visibilityMap() const;

private slots:
    void on_btnAll_clicked();
    void on_btnNone_clicked();

private:
    Ui::WDashboardSettings *ui;
    QListWidget *currentList() const;
};
