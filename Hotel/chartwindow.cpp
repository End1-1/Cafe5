#include "chartwindow.h"
#include "ui_chartwindow.h"

ChartWindow::ChartWindow() :
    TabWidget(),
    ui(new Ui::ChartWindow)
{
    ui->setupUi(this);
}

ChartWindow::~ChartWindow()
{
    delete ui;
}

void ChartWindow::createDateChart()
{

}
