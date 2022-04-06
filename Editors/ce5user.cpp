#include "ce5user.h"
#include "ui_ce5user.h"
#include "c5cache.h"
#include "ce5usergroup.h"
#include <QFileDialog>

CE5User::CE5User(const QStringList &dbParams, QWidget *parent) :
    CE5Editor(dbParams, parent),
    ui(new Ui::CE5User)
{
    ui->setupUi(this);
    ui->leState->setSelector(dbParams, ui->leStateName, cache_users_states);
    ui->leGroup->setSelector(dbParams, ui->leGroupName, cache_users_groups);
    ui->leConfig->setSelector(dbParams, ui->leConfigName, cache_settings_names);
    ui->leTeamLead->setSelector(dbParams, ui->leTeamLeadname, cache_users);
}

CE5User::~CE5User()
{
    delete ui;
}

bool CE5User::checkData(QString &err)
{
    bool result = CE5Editor::checkData(err);
    C5Database db(fDBParams);
    db[":f_id"] = ui->leCode->text();
    db[":f_login"] = ui->leLogin->text();
    db.exec("select * from s_user where f_login=:f_login and length(f_login)>0 and f_id<>:f_id");
    if (db.nextRow()) {
        err += tr("Duplicate login name") + "<br>";
        result = false;
    }
    return result;
}

void CE5User::on_btnNewGroup_clicked()
{
    CE5UserGroup *ep = new CE5UserGroup(fDBParams);
    C5Editor *e = C5Editor::createEditor(fDBParams, ep, 0);
    QList<QMap<QString, QVariant> > data;
    if(e->getResult(data)) {
        ui->leGroup->setValue(data.at(0)["f_id"].toString());
    }
    delete e;
}

void CE5User::on_btnLoadImage_clicked()
{
    if (ui->leCode->getInteger() == 0) {
        C5Message::error(tr("Save first"));
        return;
    }
    QString fn = QFileDialog::getOpenFileName(this, tr("Image"), "", "*.jpg;*.png;*.bmp");
    if (fn.isEmpty()) {
        return;
    }
    QPixmap pm;
    if (!pm.load(fn)) {
        C5Message::error(tr("Could not load image"));
        return;
    }
    qApp->processEvents();
    QFile f(fn);
    if (f.open(QIODevice::ReadOnly)) {
        C5Database db(fDBParams);
        db[":f_id"] = ui->leCode->getInteger();
        db.exec("delete from s_user_photo where f_id=:f_id");
        db[":f_id"] = ui->leCode->getInteger();
        db[":f_data"] = f.readAll();
        db.exec("insert into s_user_photo (f_id, f_data) values (:f_id, :f_data)");
        f.close();
        ui->lbPhoto->setPixmap(pm.scaled(ui->lbPhoto->size(), Qt::KeepAspectRatio));
    }
}


void CE5User::setId(int id)
{
    CE5Editor::setId(id);
    C5Database db(fDBParams);
    db[":f_id"] = ui->leCode->getInteger();
    db.exec("select * from s_user_photo where f_id=:f_id");
    QPixmap p;
    if (db.nextRow()) {
        if (p.loadFromData(db.getValue("f_data").toByteArray())) {

        } else {
            p = QPixmap(":/staff.png");
        }
    } else {
        p = QPixmap(":/staff.png");
    }
    ui->lbPhoto->setPixmap(p.scaled(ui->lbPhoto->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void CE5User::on_btnPin_clicked(bool checked)
{

}
