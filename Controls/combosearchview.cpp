#include "combosearchview.h"
#include "ui_combosearchview.h"

ComboSearchView::ComboSearchView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComboSearchView)
{
    ui->setupUi(this);
}

ComboSearchView::~ComboSearchView()
{
    delete ui;
}
