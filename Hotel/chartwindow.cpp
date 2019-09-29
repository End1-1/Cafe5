#include "chartwindow.h"
#include "ui_chartwindow.h"
#include "chartdatescene.h"

ChartWindow::ChartWindow() :
    TabWidget(),
    ui(new Ui::ChartWindow)
{
    ui->setupUi(this);
    ui->deEnd->setDate(ui->deStart->date().addDays(60));
    fDateScene = new ChartDateScene();
    ui->gvDate->setScene(fDateScene);
    loadScene();
}

ChartWindow::~ChartWindow()
{
    delete ui;
}

void ChartWindow::createDateChart()
{

}

void ChartWindow::loadScene()
{
    fDateScene->setDateRange(ui->deStart->date(), ui->deEnd->date());
}

void ChartWindow::on_btnForward_clicked()
{
    ui->deStart->setDate(ui->deStart->date().addDays(30));
    ui->deEnd->setDate(ui->deEnd->date().addDays(30));
    loadScene();
}
