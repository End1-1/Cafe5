#include "dlgsystem.h"
#include "ui_dlgsystem.h"
#include <QInputDialog>

DlgSystem::DlgSystem(const QStringList &dbParams) :
    C5Dialog(__c5config.dbParams()),
    ui(new Ui::DlgSystem)
{
    ui->setupUi(this);
    showMaximized();
    hide();
}

DlgSystem::~DlgSystem()
{
    delete ui;
}
