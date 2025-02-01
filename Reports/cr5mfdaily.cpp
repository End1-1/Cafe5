#include "cr5mfdaily.h"
#include "ui_cr5mfdaily.h"
#include "c5selector.h"
#include "c5lineeditwithselector.h"
#include "c5cache.h"

CR5MfDaily::CR5MfDaily(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::CR5MfDaily)
{
    ui->setupUi(this);
    fIcon = ":/manufacturing.png";
    fLabel = tr("Daily journal");
    ui->wt->addColumn(tr("Rownum"), 0);
    ui->wt->addColumn(tr("Product"), 180);
    ui->wt->addColumn(tr("Process"), 300);
    ui->wt->addColumn(tr("Quantity"), 100, true);
    ui->wt->addColumn(tr("Price"), 80);
    ui->wt->addColumn(tr("Total"), 100, true);
    ui->wt->addColumn(tr("Task"), 150);
    ui->wt->addColumn(tr("Ready"), 80);
    ui->wt->addColumn(tr("Max"), 80);
    ui->wt->addColumn(tr("Process id"), 0);
    ui->wt->addColumn(tr("Last"), 0);

    loadDoc(ui->leDate->date(), 0, 0);
    refreshTasks();
}

CR5MfDaily::~CR5MfDaily()
{
    delete ui;
}

QToolBar *CR5MfDaily::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
        btn << ToolBarButtons::tbExcel;
        createStandartToolbar(btn);
        fToolBar->addAction(QIcon(":/add-user.png"), tr("Add\nworker"), this, SLOT(addWorker()));
        fToolBar->addAction(QIcon(":/delete-user.png"), tr("Remove\nuser"), this, SLOT(removeWorker()));
        fToolBar->addAction(QIcon(":/add_work.png"), tr("Add\nwork"), this, SLOT(addProcess()));
        fToolBar->addAction(QIcon(":/delete-action.png"), tr("Remove\naction"), this, SLOT(removeProcess()));
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(saveWork()));
        fToolBar->addAction(QIcon(":/filter_clear.png"), tr("Select\nall workers"), this, SLOT(selectAllWorkers()));
    }
    return fToolBar;
}

void CR5MfDaily::loadDoc(const QDate &date, int worker, int teamlead)
{
    disconnect(ui->lstWorkers, SIGNAL(currentRowChanged(int)), this, SLOT(on_lstWorkers_currentRowChanged(int)));
    ui->lstWorkers->clear();
    ui->lstTeamlead->clear();
    ui->wt->clearTables();
    C5Database db(fDBParams);

    QListWidgetItem *litem = new QListWidgetItem(ui->lstTeamlead);
    litem->setText(tr("All"));
    litem->setData(Qt::UserRole, 0);
    ui->lstTeamlead->addItem(litem);
    db[":f_date"] = date;
    QString sql = "select distinct(u.f_teamlead) as f_teamlead ,"
        "concat_ws(' ', u.f_last, u.f_first) as f_name "
        "from mf_daily_workers m "
        "inner join s_user u on u.f_id=m.f_worker "
        "where f_date=:f_date ";
    db.exec(sql);
    while (db.nextRow()) {
        if (db.getInt(0) == 0) {
            continue;
        }
        litem = new QListWidgetItem(ui->lstTeamlead);
        litem->setText(db.getString(1));
        litem->setData(Qt::UserRole, db.getInt(0));
        ui->lstTeamlead->addItem(litem);
    }
    if (teamlead > 0) {
        for (int i = 0; i < ui->lstTeamlead->count(); i++) {
            if (ui->lstTeamlead->item(i)->data(Qt::UserRole).toInt() == teamlead) {
                ui->lstTeamlead->setCurrentRow(i);
                break;
            }
        }
    }

    db[":f_date"] = date;
    sql = "select m.f_id, u.f_teamlead, concat(u.f_last, ' ', u.f_first) as f_name, m.f_worker, m.f_checked "
        "from mf_daily_workers m "
        "inner join s_user u on u.f_id=m.f_worker "
        "where f_date=:f_date ";
    if (teamlead > 0) {
        db[":f_teamlead"] = teamlead;
        sql += " and u.f_teamlead=:f_teamlead ";
    }
    sql += " order by m.f_id";
    db.exec(sql);
    QSet<int> teamleadlist;
    while (db.nextRow()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lstWorkers);
        item->setText(db.getString("f_name"));
        item->setData(Qt::UserRole, db.getInt("f_id"));
        item->setData(Qt::UserRole + 1, db.getInt("f_worker"));
        item->setData(Qt::UserRole + 2, db.getInt("f_teamlead"));
        item->setCheckState(db.getInt("f_checked") == 0 ? Qt::Unchecked : Qt::Checked);
        ui->lstWorkers->addItem(item);
        teamleadlist.insert(db.getInt("f_teamlead"));
    }
    db[":f_date"] = date;
    sql = "select p.f_id, pr.f_name as f_productname, ac.f_name as f_processname, "
            "p.f_qty, p.f_price, p.f_taskid, concat('#', t.f_id, ' ', pr.f_name) as f_taskdate, "
            "t.f_qty as f_goal, dp.f_qty as f_ready, p.f_process, p.f_laststep "
            "from mf_daily_process p "
            "inner join mf_actions_group pr on pr.f_id=p.f_product "
            "inner join mf_actions ac on ac.f_id=p.f_process "
            "left join mf_tasks t on t.f_id=p.f_taskid "
            "left join (select dp.f_process, dp.f_taskid, sum(dp.f_qty) as f_qty from mf_daily_process dp group by 1,2) as dp on dp.f_taskid=p.f_taskid and dp.f_process=p.f_process "
            "where p.f_date=:f_date ";
    if (worker > 0) {
        db[":f_worker"] = worker;
        sql += " and p.f_worker=:f_worker";
    }
    if (teamlead > 0) {
        db[":f_teamlead"] = teamlead;
        sql += " and p.f_worker in (select f_id from s_user where f_teamlead=:f_teamlead) ";
    }
    db.exec(sql);
    ui->wt->setRowCount(db.rowCount());
    QElapsedTimer e;
    e.start();
    int row = 0;
    while (db.nextRow()) {
        ui->wt->setData(row, 0, db.getInt("f_id"))
                .setData(row, 1, db.getString("f_productname"))
                .setData(row, 2, db.getString("f_processname"))
                .createLineEdit(row, 3, db.getDouble("f_qty"), this, SLOT(processQtyChanged(QString)))
                .setData(row, 4, db.getDouble("f_price"))
                .setData(row, 5, db.getDouble("f_qty") * db.getDouble("f_price"))
                .createLineEditDblClick(row, 6, this, SLOT(processTaskDbClick()))
                .setData(row, 7, db.getDouble("f_ready"))
                .setData(row, 8, db.getDouble("f_goal"))
                .setData(row, 9, db.getInt("f_process"))
                .setData(row, 10, db.getInt("f_laststep"));
        ui->wt->lineEdit(row, 3)->setValidator(new QDoubleValidator(0, 99999999, 2));
        ui->wt->setData(row, 6, db.getInt("f_taskid"));
        ui->wt->lineEdit(row, 6)->setText(db.getString("f_taskdate"));
        if (db.getInt("f_laststep") == 1) {
            ui->wt->setRowColor(row, QColor::fromRgb(0xc7ffce));
            ui->wt->setData(row, 10, 1);
        }
        row++;
    }
    ui->wt->sumColumns();
    if (worker > 0) {
        for (int i = 0; i < ui->lstWorkers->count(); i++) {
            if (ui->lstWorkers->item(i)->data(Qt::UserRole + 1).toInt() == worker) {
                ui->lstWorkers->setCurrentRow(i);
                break;
            }
        }
    }
    connect(ui->lstWorkers, SIGNAL(currentRowChanged(int)), this, SLOT(on_lstWorkers_currentRowChanged(int)));
    on_leFilterWorker_textChanged(ui->leFilterWorker->text());
}

void CR5MfDaily::refreshTasks()
{
    ui->cbTasks->clear();
    C5Database db(fDBParams);
    db[":f_state"] = 1;
    db.exec("select t.f_id, t.f_product, concat(t.f_id, ' ', p.f_name) as f_name from mf_tasks t "
            "left join mf_actions_group p on p.f_id=t.f_product   "
            "where t.f_state=:f_state "
            "");
    while (db.nextRow()) {
        ui->cbTasks->addItem(db.getString("f_name"), db.getInt("f_id"));
        ui->cbTasks->setItemData(ui->cbTasks->count() - 1, db.getInt("f_product"), Qt::UserRole + 1);
    }
}

void CR5MfDaily::processTaskDbClick()
{
    C5LineEditWithSelector *l = static_cast<C5LineEditWithSelector*>(sender());
    int r, c;
    if (ui->wt->findWidget(r, c, l) == false) {
        return;
    }
    QVector<QJsonValue> vals;
    if (!C5Selector::getValueOfColumn(fDBParams, cache_mf_active_task, vals, 3)) {
        return;
    }
    if (vals.at(1).toInt() == 0) {
        C5Message::error(tr("Could not add task"));
        return;
    }

    ui->wt->setData(r, 6, vals.at(1));
    ui->wt->lineEdit(r, 6)->setText(QString("#%1 %2").arg(vals.at(1).toString(), vals.at(2).toString()));
}

void CR5MfDaily::exportToExcel()
{
    ui->wt->exportToExcel();
}

void CR5MfDaily::addWorker()
{
    QVector<QJsonValue> vals;
    if (!C5Selector::getValue(fDBParams, cache_users, vals)) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_date"] = ui->leDate->date();
    db[":f_worker"] = vals.at(1);
    db.exec("select f_id from mf_daily_workers where f_date=:f_date and f_worker=:f_worker");
    if (db.nextRow()) {
        C5Message::error(tr("Worker already added in selected date"));
        return;
    }
    db[":f_date"] = ui->leDate->date();
    db[":f_worker"] = vals.at(1);
    int id = db.insert("mf_daily_workers");
    QListWidgetItem *item = new QListWidgetItem(ui->lstWorkers);
    item->setText(vals.at(2).toString());
    item->setData(Qt::UserRole, id);
    item->setData(Qt::UserRole + 1, vals.at(1));
    item->setData(Qt::UserRole + 2, vals.at(2));
    ui->lstWorkers->addItem(item);
    ui->lstWorkers->setCurrentItem(item);
}

void CR5MfDaily::removeWorker()
{
    QListWidgetItem *item = ui->lstWorkers->currentItem();
    if (!item) {
        return;
    }
    if (C5Message::question(tr("Are you sure to remove?")) != QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_date"] = ui->leDate->date();
    db[":f_worker"] = item->data(Qt::UserRole + 1);
    db.exec("delete from mf_daily_workers where f_date=:f_date and f_worker=:f_worker");
    db[":f_date"] = ui->leDate->date();
    db[":f_worker"] = item->data(Qt::UserRole + 1);
    db.exec("delete from mf_daily_process where f_date=:f_date and f_worker=:f_worker");
    loadDoc(ui->leDate->date(), 0, 0);
}

void CR5MfDaily::addProcess()
{
    QListWidgetItem *item = ui->lstWorkers->currentItem();
    if (!item) {
        C5Message::error(tr("Worker is not selected"));
        return;
    }


    QString query = QString("select p.f_id as `%1`, p.f_rowid + 1 as `%2`, p.f_product as `%3`, gr.f_name as `%4`, "
                            "p.f_process as `%5`, ac.f_name as `%6`, "
                            "p.f_durationsec as `%7`, p.f_price as `%8`, ac.f_state as `%9` "
                            "from mf_process p "
                            "inner join mf_actions_group gr on gr.f_id=p.f_product "
                            "inner join mf_actions ac on ac.f_id=p.f_process "
                            "where p.f_product = %10 "
                            "order by gr.f_name, p.f_rowid")
                .arg(tr("Code"), tr("Row"), tr("Product code"), tr("Product"),
                     tr("Process code"), tr("Process"), tr("Duration"), tr("Price"),
                     tr("Stage code"),
                     ui->cbTasks->itemData(ui->cbTasks->currentIndex(), Qt::UserRole + 1).toString());
    QVector<QJsonValue> vals;
    if (!C5Selector::getValue(fDBParams, query, vals)) {
        return;
    }

    C5Database db(fDBParams);
    db[":f_product"] = vals.at(3);
    db.exec("SELECT f_process FROM mf_process WHERE f_product=:f_product ORDER BY f_rowid DESC LIMIT 1 ");
    db.nextRow();
    bool last = db.getInt("f_process") == vals.at(5).toInt();

    int taskid = 0;
    double max = 0;
    QDate date;
    db[":f_id"] = ui->cbTasks->itemData(ui->cbTasks->currentIndex(), Qt::UserRole);
    db.exec("select f_id, f_qty, f_datecreate from mf_tasks where f_state=1 and f_id=:f_id");
    if (db.nextRow()) {
        taskid = db.getInt("f_id");
        max = db.getDouble("f_qty");
        date = db.getDate("f_datecreate");
    } else {
        C5Message::error(tr("No active task"));
        return;
    }

    db[":f_date"] = ui->leDate->date();
    db[":f_worker"] = item->data(Qt::UserRole + 1);
    db[":f_product"] = vals.at(3);
    db[":f_process"] = vals.at(5);
    db[":f_qty"] = 0;
    db[":f_price"] = vals.at(8);
    db[":f_taskid"] = taskid;
    db[":f_laststep"] = last ? 1 : 0;
    int rowid = db.insert("mf_daily_process");

    db[":f_product"] = vals.at(3);
    db[":f_process"] = vals.at(5);
    db[":f_taskid"] = taskid;
    db.exec("select sum(f_qty) from mf_daily_process where f_product=:f_product "
            "and f_process=:f_process and f_taskid=:f_taskid");
    db.nextRow();
    double done = db.getDouble(0);

    db[":f_stage"] = vals.at(9);
    db[":f_id"] = taskid;
    db.exec("update mf_tasks set f_stage=:f_stage where f_id=:f_id");
    qDebug() << vals;
    int row = 0;
    ui->wt->setData(row, 0, rowid, true)
            .setData(row, 1, vals.at(4))
            .setData(row, 2, vals.at(6))
            .createLineEdit(row, 3, 0, this, SLOT(processQtyChanged(QString)))
            .setData(row, 4, vals.at(8))
            .setData(row, 5, 0)
            .createLineEdit(row, 6, date.toString("dd/MM/yyyy"));
    ui->wt->lineEdit(row, 3)->setValidator(new QDoubleValidator(0, 9999999, 2));
    ui->wt->setData(row, 6, taskid);
    ui->wt->lineEdit(row, 6)->setText(date.toString("dd/MM/yyyy"));
    ui->wt->setData(row, 7, done);
    ui->wt->setData(row, 8, max);
    ui->wt->setData(row, 9, vals.at(5));
    ui->wt->setData(row, 10, 0);

    if (last) {
        ui->wt->setRowColor(row, QColor::fromRgb(0xc7ffce));
        ui->wt->setData(row, 10, 1);
    }
}

void CR5MfDaily::removeProcess()
{
    int r = ui->wt->currentRow();
    if (r < 0) {
        return;
    }
    if (C5Message::question(QString("%1<br>%2/%3")
                            .arg(tr("Confirm to remove"),
                                 ui->wt->getData(r, 1).toString(),
                                 ui->wt->getData(r, 2).toString())) != QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = ui->wt->getData(r, 0);
    db.exec("delete from mf_daily_process where f_id=:f_id");

    if (ui->wt->getData(r, 10).toInt() == 1) {
        db[":f_id"] = ui->wt->getData(r, 6);
        db[":f_taskid"] = ui->wt->getData(r, 6);
        db[":f_process"] = ui->wt->getData(r, 9);
        db.exec("update mf_tasks set f_ready=(select sum(f_qty) from mf_daily_process where f_taskid=:f_taskid and f_process=:f_process) where f_id=:f_id");
    }

    ui->wt->removeRow(r);
    ui->wt->countTotal(-1);
}

void CR5MfDaily::saveWork()
{
    C5Database db(fDBParams);
    bool qtywarning = false;
    for (int i = 0; i < ui->wt->rowCount(); i++) {
//        if (ui->wt->getData(i, 6).toInt() == 0) {
//            C5Message::error(tr("Not all task is defined"));
//            return;
//        }
        C5LineEdit *l = ui->wt->lineEdit(i, 3);
        if (l->getDouble() < 0.001) {
            qtywarning = true;
            //l->setBgColor(QColor::fromRgb(250, 170, 170));
            l->setBgColor(Qt::red);
        } else {
            l->setBgColor(Qt::white);
        }
        db[":f_qty"] = l->getDouble();
        db[":f_taskid"] = ui->wt->getData(i, 6);
        db.update("mf_daily_process", "f_id", ui->wt->getData(i, 0));

        if (ui->wt->getData(i, 10).toInt() == 1) {
            db[":f_id"] = ui->wt->getData(i, 6);
            db[":f_taskid"] = ui->wt->getData(i, 6);
            db[":f_process"] = ui->wt->getData(i, 9);
            db.exec("update mf_tasks set f_ready=(select sum(f_qty) from mf_daily_process where f_taskid=:f_taskid and f_process=:f_process) where f_id=:f_id");
        }
    }
    if (qtywarning) {
        C5Message::error(tr("Saved, but some of quantities equals to 0."));
    } else {
        C5Message::info(tr("Saved"));
    }
}

void CR5MfDaily::selectAllWorkers()
{
    ui->lstWorkers->setCurrentRow(-1);
    loadDoc(ui->leDate->date(), 0, 0);
}

void CR5MfDaily::processQtyChanged(const QString &arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row = l->property("row").toInt();
    if (str_float(arg1) + ui->wt->getData(row, 7).toDouble() > ui->wt->getData(row, 8).toDouble()) {
        C5Message::error(tr("The quantity of the processes is greater than goal"));
    }
    ui->wt->setData(row, 5, arg1.toDouble() * ui->wt->getData(row, 4).toDouble());
    ui->wt->countTotal(-1);
}

void CR5MfDaily::on_btnDatePlus_clicked()
{
    ui->leDate->setDate(ui->leDate->date().addDays(1));
    ui->lstWorkers->setCurrentRow(-1);
    loadDoc(ui->leDate->date(), 0, 0);
}

void CR5MfDaily::on_btnDateMinus_clicked()
{
    ui->leDate->setDate(ui->leDate->date().addDays(-1));
    ui->lstWorkers->setCurrentRow(-1);
    loadDoc(ui->leDate->date(), 0, 0);
}

void CR5MfDaily::on_lstWorkers_currentRowChanged(int currentRow)
{
    Q_UNUSED(currentRow);
    int worker = 0;
    if (currentRow > -1) {
        worker = ui->lstWorkers->item(currentRow)->data(Qt::UserRole + 1).toInt();
    }
    currentRow = ui->lstTeamlead->currentRow();
    int teamlead = 0;
    if (currentRow > -1) {
        teamlead = ui->lstTeamlead->item(currentRow)->data(Qt::UserRole).toInt();
    }
    loadDoc(ui->leDate->date(), worker, teamlead);
}

void CR5MfDaily::on_leDate_returnPressed()
{
    ui->lstWorkers->setCurrentRow(-1);
    loadDoc(ui->leDate->date(), 0, 0);
}

void CR5MfDaily::on_lstTeamlead_itemClicked(QListWidgetItem *item)
{
    loadDoc(ui->leDate->date(), 0, item->data(Qt::UserRole).toInt());
}

void CR5MfDaily::on_leFilterWorker_textChanged(const QString &arg1)
{
    for (int i = 0; i < ui->lstWorkers->count(); i++) {
        ui->lstWorkers->setRowHidden(i, ui->lstWorkers->item(i)->text().contains(arg1, Qt::CaseInsensitive) == false);
    }
}

void CR5MfDaily::on_lstWorkers_itemClicked(QListWidgetItem *item)
{
    int checked = item->checkState() == Qt::Checked ? 1 : 0;
    int worker = item->data(Qt::UserRole + 1).toInt();
    if (worker > 0) {
        C5Database db(fDBParams);
        db[":f_checked"] = checked;
        db[":f_worker"] = worker;
        db[":f_date"] = ui->leDate->date();
        db.exec("update mf_daily_workers set f_checked=:f_checked where f_worker=:f_worker and f_date=:f_date");
    }
}

void CR5MfDaily::on_btnRefreshTask_clicked()
{
    refreshTasks();
}
