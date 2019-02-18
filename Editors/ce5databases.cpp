#include "ce5databases.h"
#include "ui_ce5databases.h"

CE5Databases::CE5Databases(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5Databases)
{
    ui->setupUi(this);
}

CE5Databases::~CE5Databases()
{
    delete ui;
}

void CE5Databases::on_btnCheckConnection_clicked()
{
    C5Database db(ui->leHost->text(), ui->leSchema->text(), ui->leUsername->text(), ui->lePassword->text());
    if (db.open()) {
        C5Message::info(tr("Connected"));
    } else {
        C5Message::error(tr("Connection error") + "<br>" + db.fLastError);
    }
}
