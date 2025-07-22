#include "cr5breezeservice.h"
#include "ui_cr5breezeservice.h"

CR5BreezeService::CR5BreezeService(QWidget *parent) :
    C5ReportWidget( parent),
    ui(new Ui::CR5BreezeService)
{
    ui->setupUi(this);
}

CR5BreezeService::~CR5BreezeService()
{
    delete ui;
}
