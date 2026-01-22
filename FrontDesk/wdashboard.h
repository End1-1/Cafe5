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
    explicit WDashboard(QWidget *parent = nullptr);

    ~WDashboard();

private slots:
    void onCommandButtonClicked();

private:
    Ui::WDashboard* ui;
    
    QWidget* createForm(const QString &name, QIcon icon);
};
