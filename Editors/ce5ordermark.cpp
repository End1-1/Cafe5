#include "ce5ordermark.h"
#include "ui_ce5ordermark.h"

CE5OrderMark::CE5OrderMark(QWidget *parent) :
    CE5Editor(parent),
    ui(new Ui::CE5OrderMark)
{
    ui->setupUi(this);
}

CE5OrderMark::~CE5OrderMark()
{
    delete ui;
}

QString CE5OrderMark::title()
{
    return tr("Order mark");
}

QString CE5OrderMark::table()
{
    return "b_marks";
}
