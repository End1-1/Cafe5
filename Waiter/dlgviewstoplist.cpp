#include "dlgviewstoplist.h"
#include "ui_dlgviewstoplist.h"

DlgViewStopList::DlgViewStopList() :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgViewStopList)
{
    ui->setupUi(this);
}

DlgViewStopList::~DlgViewStopList()
{
    delete ui;
}
