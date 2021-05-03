#include "dlgtools.h"
#include "ui_dlgtools.h"

DlgTools::DlgTools(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgTools)
{
    ui->setupUi(this);
}

DlgTools::~DlgTools()
{
    delete ui;
}
