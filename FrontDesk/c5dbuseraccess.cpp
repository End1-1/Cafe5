#include "c5dbuseraccess.h"
#include "ui_c5dbuseraccess.h"
#include "c5permissions.h"
#include "c5checkbox.h"
#include "c5message.h"

C5DbUserAccess::C5DbUserAccess(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5DbUserAccess)
{
    ui->setupUi(this);
    fIcon = ":/access.png";
    fLabel = tr("Database access");
    refreshData();
}

C5DbUserAccess::~C5DbUserAccess()
{
    delete ui;
}

QToolBar *C5DbUserAccess::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbSave
            << ToolBarButtons::tbRefresh
            << ToolBarButtons::tbExcel
            << ToolBarButtons::tbPrint;
            createStandartToolbar(btn);
    }
    return fToolBar;
}

void C5DbUserAccess::refreshData()
{
    fUserMap.clear();
    fDbMap.clear();
    ui->tbl->setRowCount(0);
    ui->tbl->setColumnCount(3);
    ui->tbl->setColumnWidth(0, 0);
    ui->tbl->setColumnWidth(1, 300);
    ui->tbl->setColumnWidth(2, 120);
    C5Database db(fDBParams);
    db[":f_key"] = cp_t1_login_to_manager;
    db.exec("select u.f_id, concat(u.f_last, ' ', u.f_first), u.f_login \
            from s_user u \
            where (u.f_group in (select f_group from s_user_access where f_key=:f_key and f_value='1') or u.f_group=1) \
            and u.f_state=1 \
            group by 1,2");
    while (db.nextRow()) {
        int row = ui->tbl->addEmptyRow();
        fUserMap[db.getInt(0)] = row;
        ui->tbl->setInteger(row, 0, db.getInt(0));
        ui->tbl->setString(row, 1, db.getString(1));
        ui->tbl->setString(row, 2, db.getString(2));
    }
    db.exec("select f_id, f_name from s_db");
    QStringList headers;
    headers << ""
            << tr("Name")
            << tr("Login");
    while (db.nextRow()) {
        ui->tbl->setColumnCount(ui->tbl->columnCount() + 1);
        fDbMap[db.getInt(0)] = ui->tbl->columnCount() - 1;
        headers << db.getString(1);
    }
    ui->tbl->setHorizontalHeaderLabels(headers);
    for (int i = 3; i < ui->tbl->columnCount(); i++) {
        for (int j = 0; j < ui->tbl->rowCount(); j++) {
            ui->tbl->createCheckbox(j, i);
        }
    }
    db.exec("select f_user, f_db from s_db_access where f_permit=1");
    while (db.nextRow()) {
        ui->tbl->checkBox(fUserMap[db.getInt(0)], fDbMap[db.getInt(1)])->setChecked(true);
    }
}

void C5DbUserAccess::saveDataChanges()
{
    C5Database db(fDBParams);
    db.startTransaction();
    db.exec("delete from s_db_access");
    for (int c = 3; c < ui->tbl->columnCount(); c++) {
        for (int r = 0; r < ui->tbl->rowCount(); r++) {
            if (ui->tbl->checkBox(r, c)->isChecked()) {
                db[":f_user"] = ui->tbl->getInteger(r, 0);
                db[":f_db"] = fDbMap.key(c);
                db[":f_permit"] = 1;
                db.insert("s_db_access", false);
            }
        }
    }
    C5Message::info(tr("Saved"));
}

void C5DbUserAccess::on_leUsername_textChanged(const QString &arg1)
{
    for (int r = 0; r < ui->tbl->rowCount(); r++) {
        bool hidden = true;
        for (int c = 1; c < 3; c++) {
            if (ui->tbl->getString(r, c).contains(arg1, Qt::CaseInsensitive)) {
                hidden = false;
            }
        }
        ui->tbl->setRowHidden(r, hidden);
    }
}
