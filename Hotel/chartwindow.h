#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include "tabwidget.h"

namespace Ui {
class ChartWindow;
}

class ChartWindow : public TabWidget
{
    Q_OBJECT

public:
    explicit ChartWindow();
    ~ChartWindow();
    virtual const QString title() {return tr("Chart"); }
    virtual const QString icon() {return ":/images/chart.png"; }

private:
    Ui::ChartWindow *ui;
};

#endif // CHARTWINDOW_H
