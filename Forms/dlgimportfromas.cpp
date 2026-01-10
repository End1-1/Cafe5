#include "dlgimportfromas.h"
#include "ui_dlgimportfromas.h"
#include "c5config.h"
#include "c5message.h"
#include <QSqlDatabase>
#include <QSqlError>

DlgImportFromAS::DlgImportFromAS(C5User *user)
    : C5Dialog(user), ui(new Ui::DlgImportFromAS)
{
    ui->setupUi(this);
    ui->leAddressString->setText(__c5config.getRegValue("armsoft_import_as_string").toString());
}

DlgImportFromAS::~DlgImportFromAS() { delete ui; }

void DlgImportFromAS::on_btnEnableEdit_clicked(bool checked)
{
    ui->leAddressString->setEnabled(checked);
}

void DlgImportFromAS::on_leAddressString_textChanged(const QString &arg1)
{
    __c5config.setRegValue("armsoft_import_as_string", arg1);
}

void DlgImportFromAS::on_btnCheckConnection_clicked()
{
    QSqlDatabase db  = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(ui->leAddressString->text());
    db.setUserName("sa");
    db.setPassword("SaSa111");

    if(db.open()) {
        C5Message::info(tr("Connection success"));
    } else {
        C5Message::error(tr("Connection error") + QString("<br>") + db.lastError().databaseText());
    }
}
