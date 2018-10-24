#include "c5dishwidget.h"
#include "ui_c5dishwidget.h"
#include "c5grid.h"

C5DishWidget::C5DishWidget(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5DishWidget)
{
    ui->setupUi(this);

    C5Grid *w = new C5Grid(dbParams, this);
    ui->hlMenu->addWidget(w);
    w = new C5Grid(dbParams, this);
    ui->hlRecipe->addWidget(w);
}

C5DishWidget::~C5DishWidget()
{
    delete ui;
}
