#include "c5printpreview.h"
#include "ui_c5printpreview.h"

C5PrintPreview::C5PrintPreview(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::C5PrintPreview)
{
    ui->setupUi(this);
}

C5PrintPreview::~C5PrintPreview()
{
    delete ui;
}
