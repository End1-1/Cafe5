#pragma once

#include "c5widget.h"

namespace Ui
{
class WDashboard;
}

class WDashboard : public C5Widget
{
    Q_OBJECT
public:
    explicit WDashboard(C5User *user, QWidget *parent = nullptr);

    ~WDashboard();

    virtual QToolBar *toolBar() override;

private slots:
    void onCommandButtonClicked();
    void openDashboardSettings();

private:
    Ui::WDashboard* ui;
    void applyButtonVisibility();
    QWidget* createForm(const QString &name, QIcon icon);
};
