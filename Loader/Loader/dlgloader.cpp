#include "dlgloader.h"
#include "ui_dlgloader.h"
#include <QSettings>

DlgLoader::DlgLoader(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgLoader)
{
    ui->setupUi(this);
}

DlgLoader::~DlgLoader()
{
    delete ui;
}

void DlgLoader::checkForUpdate()
{
    QSettings s(_ORGANIZATION_, _APPLICATION_);
}

