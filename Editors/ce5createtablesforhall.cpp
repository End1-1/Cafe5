#include "ce5createtablesforhall.h"
#include "ui_ce5createtablesforhall.h"
#include "c5cache.h"
#include "ce5halls.h"

CE5CreateTablesForHall::CE5CreateTablesForHall(const QStringList &dbParams, QWidget *parent) :
    C5Dialog(dbParams, parent),
    ui(new Ui::CE5CreateTablesForHall)
{
    ui->setupUi(this);
    ui->leHall->setSelector(dbParams, ui->leHallName, cache_halls);
}

CE5CreateTablesForHall::~CE5CreateTablesForHall()
{
    delete ui;
}

void CE5CreateTablesForHall::createTableForHall(const QStringList &dbParams)
{
    CE5CreateTablesForHall *d = new CE5CreateTablesForHall(dbParams, __mainWindow);
    d->exec();
    delete d;
}

void CE5CreateTablesForHall::on_btnCreate_clicked()
{
    if (ui->leHall->getInteger() == 0) {
        C5Message::error(tr("Hall is not selected"));
        return;
    }
    C5Database db(fDBParams);
    for (int i = ui->leStart->getInteger(); i < ui->leCount->getInteger() + 1; i++) {
        db[":f_hall"] = ui->leHall->getInteger();
        db[":f_name"] = ui->lePrefix->text() + QString::number(i);
        db[":f_lock"] = 0;
        db[":f_lockSrc"] = "";
        db.insert("h_tables", false);
    }
    C5Message::info(tr("Done"));
}

void CE5CreateTablesForHall::on_btnNewHall_clicked()
{
    CE5Halls *ep = new CE5Halls(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leHall->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}
