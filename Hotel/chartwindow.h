#ifndef CHARTWINDOW_H
#define CHARTWINDOW_H

#include "tabwidget.h"
#include <QDate>

namespace Ui {
class ChartWindow;
}

class ChartDateScene;

class ChartWindow : public TabWidget
{
    Q_OBJECT

public:
    explicit ChartWindow();
    ~ChartWindow();

private slots:
    void on_btnForward_clicked();

private:
    Ui::ChartWindow *ui;
    void createDateChart();
    ChartDateScene *fDateScene;
    void loadScene();
};

#endif // CHARTWINDOW_H
