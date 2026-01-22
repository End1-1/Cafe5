#include "rwaiterstationconfig.h"
#include "ui_rwaiterstationconfig.h"

RWaiterStationConfig::RWaiterStationConfig(QWidget *parent)
    : RAbstractSpecialWidget(parent),
      ui(new Ui::RWaiterStationConfig)
{
    ui->setupUi(this);
}

RWaiterStationConfig::~RWaiterStationConfig() { delete ui; }
