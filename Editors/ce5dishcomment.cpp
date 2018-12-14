#include "ce5dishcomment.h"
#include "ui_ce5dishcomment.h"

CE5DishComment::CE5DishComment(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5DishComment)
{
    ui->setupUi(this);
}

CE5DishComment::~CE5DishComment()
{
    delete ui;
}
