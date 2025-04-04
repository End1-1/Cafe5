#include "dlgdriverroutedate.h"
#include "ui_dlgdriverroutedate.h"
#include "c5cache.h"
#include "c5message.h"

DlgDriverRouteDate::DlgDriverRouteDate(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::DlgDriverRouteDate)
{
    ui->setupUi(this);
    ui->leDriver->setSelector(dbParams, ui->leDriverName, cache_users);
}

DlgDriverRouteDate::~DlgDriverRouteDate()
{
    delete ui;
}

bool DlgDriverRouteDate::getDate(const QStringList &dbParams, QDate &date, int &driver, QWidget *parent)
{
    DlgDriverRouteDate d(dbParams, parent);
    bool result = d.exec() == QDialog::Accepted;
    date = d.ui->leDate->date();
    driver = d.ui->leDriver->getInteger();
    return result;
}

void DlgDriverRouteDate::on_btnCancel_clicked()
{
    reject();
}

void DlgDriverRouteDate::on_btnOK_clicked()
{
    if (ui->leDriver->getInteger() == 0) {
        C5Message::error(tr("Select driver"));
        return;
    }
    accept();
}
