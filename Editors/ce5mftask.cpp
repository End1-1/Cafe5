#include "ce5mftask.h"
#include "ui_ce5mftask.h"
#include "c5cache.h"
#include "c5database.h"
#include "c5message.h"
#include <QJsonDocument>

CE5MFTask::CE5MFTask() :
    CE5Editor(),
    ui(new Ui::CE5MFTask)
{
    ui->setupUi(this);
    ui->teCreate->setTime(QTime::currentTime());
    ui->leState->setSelector(ui->leStateName, cache_mf_task_state);
    ui->leWorkshop->setSelector(ui->leWorkshowName, cache_mf_workshop);
    ui->leStage->setSelector(ui->leStageName, cache_mf_action_stage);
    ui->leProduct->setSelector(ui->leProductName, cache_mf_products);
    ui->leResponsible->setSelector(ui->leResponsibleName, cache_users);
}

CE5MFTask::~CE5MFTask()
{
    delete ui;
}

QString CE5MFTask::title()
{
    return tr("Task");
}

QString CE5MFTask::table()
{
    return "mf_tasks";
}

void CE5MFTask::clear()
{
    CE5Editor::clear();
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    setOptions();
}

void CE5MFTask::setId(int id)
{
    CE5Editor::setId(id);
    C5Database db;
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("select * from mf_tasks where f_id=:f_id");
    if (db.nextRow()) {
        ui->tbl->setRowCount(0);
        QJsonDocument doc = QJsonDocument::fromJson(db.getString("f_notes").toUtf8());
        QJsonObject jo = doc.object();
        for (const QString &k : jo.keys()) {
            int r = ui->tbl->addEmptyRow();
            ui->tbl->setString(r, 0, k);
            ui->tbl->createLineEdit(r, 1)->setText(jo[k].toString());
        }
        if (ui->tbl->rowCount() == 0) {
            setOptions();
        }
    } else {
        setOptions();
    }
}

bool CE5MFTask::save(QString &err, QList<QMap<QString, QVariant> > &data)
{
    if (!CE5Editor::save(err, data)) {
        C5Message::error(err);
        return false;
    }
    QJsonObject jo;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        jo[ui->tbl->getString(i, 0)] = ui->tbl->lineEdit(i, 1)->text();
    }
    C5Database db;
    db[":f_id"] = ui->leCode->getInteger();
    db[":f_notes"] = QJsonDocument(jo).toJson(QJsonDocument::Compact);
    db.exec("update mf_tasks set f_notes=:f_notes where f_id=:f_id");
    return true;
}

void CE5MFTask::setOptions()
{
    ui->tbl->setRowCount(0);
    int i = ui->tbl->addEmptyRow();
    ui->tbl->createLineEdit(i, 1);
    i = ui->tbl->addEmptyRow();
    ui->tbl->createLineEdit(i, 1);
    i = ui->tbl->addEmptyRow();
    ui->tbl->createLineEdit(i, 1);
    i = ui->tbl->addEmptyRow();
    ui->tbl->createLineEdit(i, 1);
    QStringList keys;
    keys.append(tr("Color"));
    keys.append(tr("Length"));
    keys.append(tr("Width"));
    keys.append(tr("Size"));
    for (int i = 0; i < keys.length(); i++) {
        ui->tbl->setString(i, 0, keys.at(i));
    }
}
