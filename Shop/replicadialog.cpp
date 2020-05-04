#include "replicadialog.h"
#include "ui_replicadialog.h"
#include "c5database.h"
#include "c5config.h"
#include <QTimer>
#include <QThread>

ReplicaDialog::ReplicaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReplicaDialog)
{
    ui->setupUi(this);
    QTimer *t = new QTimer();
    connect(t, SIGNAL(timeout()), this, SLOT(timeout()));
    t->start(2000);
}

ReplicaDialog::~ReplicaDialog()
{
    delete ui;
}

void ReplicaDialog::progress(const QString &msg)
{
    ui->lbMsg->setText(msg);
    qApp->processEvents();
}

void ReplicaDialog::timeout()
{
    int steps = 6;
    int step = 1;
    sender()->deleteLater();
    C5Database dr(__c5config.replicaDbParams());
    progress("Connecting to replication database");
    if (!dr.open()) {
        progress("Connection to " + __c5config.replicaDbParams().at(0) + ":" + __c5config.replicaDbParams().at(1) + " failed.<br>" + dr.fLastError);
        QThread::msleep(2000);
        reject();
        return;
    }
    C5Database db(__c5config.dbParams());

    //updateTable(dr, db, step, steps, "c_units");
    //updateTable(dr, db, step, steps, "c_groups");
    //updateTable(dr, db, step, steps, "c_goods");
    db.exec("delete from s_settings_values");
    db.exec("delete from s_settings_names");
    updateTable(dr, db, step, steps, "s_settings_names");
    updateTable(dr, db, step, steps, "s_settings_values");

    db.exec("delete from a_store_temp");
    dr[":date"] = QDate::currentDate();
    dr[":store"] = __c5config.defaultStore();
    dr.exec("select g.f_id as f_code,ss.f_name as f_storage,gg.f_name as f_group,g.f_name as f_goods,g.f_scancode,sum(s.f_qty*s.f_type) as f_qty, "
            "u.f_name as f_unit,g.f_lastinputprice,sum(g.f_lastinputprice*s.f_type*s.f_qty) as f_total,g.f_saleprice,sum(s.f_qty*s.f_type)*g.f_saleprice as f_totalsale,"
            "g.f_saleprice2,sum(s.f_qty*s.f_type)*g.f_saleprice2 as f_totalsale2 "
            "from a_store_draft s "
            "inner join c_goods g on g.f_id=s.f_goods "
            "inner join c_storages ss on ss.f_id=s.f_store "
            "inner join c_groups gg on gg.f_id=g.f_group "
            "inner join c_units u on u.f_id=g.f_unit "
            "inner join a_header h on h.f_id=s.f_document  "
            "where  h.f_date<=:date  and s.f_store=:store "
            "group by g.f_id,ss.f_name,gg.f_name,g.f_name,u.f_name,g.f_lastinputprice,g.f_saleprice,g.f_saleprice2 "
            "having sum(s.f_qty*s.f_type) > 0");
    while (dr.nextRow()) {
        db[":f_store"] = __c5config.defaultStore();
        db[":f_goods"] = dr.getInt("f_code");
        db[":f_qty"] = dr.getDouble("f_qty");
        db.insert("a_store_temp", false);
        progress(QString("Step %1 of %2. %3 %4/%5").arg(step).arg(steps).arg("Store").arg(dr.pos()).arg(dr.rowCount()));
    }
    step++;

    accept();
}

void ReplicaDialog::updateTable(C5Database &dr, C5Database &db, int &step, int steps, const QString &tableName)
{
    QStringList id;
    db.exec("select * from " + tableName);
    while (db.nextRow()) {
        id.append(db.getString("f_id"));
    }
    dr.exec("select * from " + tableName);
    while (dr.nextRow()) {
        progress(QString("Step %1 of %2. %3 %4/%5").arg(step).arg(steps).arg(tableName).arg(dr.pos()).arg(dr.rowCount()));
        db.setBindValues(dr.getBindValues());
        if (id.contains(dr.getString("f_id"))) {
            if (!db.update(tableName, where_id(dr.getString("f_id")))) {
                progress(db.fLastError);
            }
        } else {
            if (!db.insert(tableName, false)) {
                progress(db.fLastError);
            }
        }
    }
    step++;
}
