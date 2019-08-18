#include "login.h"
#include "ui_login.h"
#include "connectionsettings.h"

Login::Login() :
    Dialog(),
    ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

bool Login::login()
{
    Login *l = new Login();
    l->setWidgetContainer();
    if (l->success()) {
        return true;
    } else {
        return false;
    }
}

void Login::setWidgetContainer()
{
    fCtrl = ui->wc;
    fCtrl->getWatchList(fCtrl);
}

bool Login::success()
{
    exec();
    fCtrl->saveChanges();
    return true;
}

void Login::on_btnCancel_clicked()
{
    reject();
}

void Login::on_btnOk_clicked()
{
    accept();
}

void Login::on_btnSettings_clicked()
{
    ConnectionSettings::configure();
}
