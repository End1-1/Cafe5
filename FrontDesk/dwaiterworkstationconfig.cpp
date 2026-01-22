#include "dwaiterworkstationconfig.h"
#include "ui_dwaiterworkstationconfig.h"

DWaiterWorkstationConfig::DWaiterWorkstationConfig(QWidget *parent)
    : QDialog(parent), ui(new Ui::DWaiterWorkstationConfig) {
    ui->setupUi(this);
}

DWaiterWorkstationConfig::~DWaiterWorkstationConfig() { delete ui; }
