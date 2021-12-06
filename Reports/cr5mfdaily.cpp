#include "cr5mfdaily.h"
#include "ui_cr5mfdaily.h"
#include "c5selector.h"
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

    loadDoc(ui->leDate->date());
}

CR5MfDaily::~CR5MfDaily()
{
    delete ui;
}

QToolBar *CR5MfDaily::toolBar()
{
    if (!fToolBar) {
        QList<ToolBarButtons> btn;
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

void CR5MfDaily::loadDoc(const QDate &date)
{
    disconnect(ui->lstWorkers, SIGNAL(currentRowChanged(int)), this, SLOT(on_lstWorkers_currentRowChanged(int)));
    QListWidgetItem *item = ui->lstWorkers->currentItem();
    int worker = 0;
    int row = ui->lstWorkers->currentRow();
    if (item) {
        worker = item->data(Qt::UserRole + 1).toInt();
    }
    ui->lstWorkers->clear();
    ui->wt->clearTables();
    C5Database db(fDBParams);
    db[":f_date"] = date;
    db.exec("select m.f_id, concat(u.f_last, ' ', u.f_first) as f_name, m.f_worker "
        "from mf_daily_workers m "
        "inner join s_user u on u.f_id=m.f_worker "
        "where f_date=:f_date");
    while (db.nextRow()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lstWorkers);
        item->setText(db.getString("f_name"));
        item->setData(Qt::UserRole, db.getInt("f_id"));
        item->setData(Qt::UserRole + 1, db.getInt("f_worker"));
        ui->lstWorkers->addItem(item);
    }
    db[":f_date"] = date;
    QString sql = "select p.f_id, pr.f_name as f_productname, ac.f_name as f_processname, "
            "p.f_qty, p.f_price "
            "from mf_daily_process p "
            "inner join mf_actions_group pr on pr.f_id=p.f_product "
            "inner join mf_actions ac on ac.f_id=p.f_process "
            "where p.f_date=:f_date ";
    if (worker > 0) {
        db[":f_worker"] = worker;
        sql += " and p.f_worker=:f_worker";
    }
    db.exec(sql);
    while (db.nextRow()) {
        int row;
        ui->wt->setData(row, 0, db.getInt("f_id"), true)
                .setData(row, 1, db.getString("f_productname"))
                .setData(row, 2, db.getString("f_processname"))
                .createLineEdit(row, 3, db.getDouble("f_qty"), this, SLOT(processQtyChanged(QString)))
                .setData(row, 4, db.getDouble("f_price"))
                .setData(row, 5, db.getDouble("f_qty") * db.getDouble("f_price"));
    }
    ui->lstWorkers->setCurrentRow(row);
    connect(ui->lstWorkers, SIGNAL(currentRowChanged(int)), this, SLOT(on_lstWorkers_currentRowChanged(int)));
}

void CR5MfDaily::addWorker()
{
    QList<QVariant> vals;
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
    loadDoc(ui->leDate->date());
}

void CR5MfDaily::addProcess()
{
    QListWidgetItem *item = ui->lstWorkers->currentItem();
    if (!item) {
        C5Message::error(tr("Worker is not selected"));
        return;
    }
    QList<QVariant> vals;
    if (!C5Selector::getValue(fDBParams, cache_mf_process, vals)) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_date"] = ui->leDate->date();
    db[":f_worker"] = item->data(Qt::UserRole + 1);
    db[":f_product"] = vals.at(3);
    db[":f_process"] = vals.at(5);
    db[":f_qty"] = 0;
    db[":f_price"] = vals.at(8);
    int rowid = db.insert("mf_daily_process");
    qDebug() << vals;
    int row = 0;
    ui->wt->setData(row, 0, rowid, true)
            .setData(row, 1, vals.at(4))
            .setData(row, 2, vals.at(6))
            .createLineEdit(row, 3, 0, this, SLOT(processQtyChanged(QString)))
            .setData(row, 4, vals.at(8))
            .setData(row, 5, 0);
}

void CR5MfDaily::removeProcess()
{
    int r = ui->wt->currentRow();
    if (r < 0) {
        return;
    }
    if (C5Message::question(QString("%1<br>%2/%3")
                            .arg(tr("Confirm to remove"))
                            .arg(ui->wt->getData(r, 1).toString())
                            .arg(ui->wt->getData(r, 2).toString())) != QDialog::Accepted) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = ui->wt->getData(r, 0);
    db.exec("delete from mf_daily_process where f_id=:f_id");
    ui->wt->removeRow(r);
}

void CR5MfDaily::saveWork()
{
    C5Database db(fDBParams);
    for (int i = 0; i < ui->wt->rowCount(); i++) {
        C5LineEdit *l = ui->wt->lineEdit(i, 3);
        db[":f_qty"] = l->getDouble();
        db.update("mf_daily_process", "f_id", ui->wt->getData(i, 0));
    }
    C5Message::info(tr("Saved"));
}

void CR5MfDaily::selectAllWorkers()
{
    ui->lstWorkers->setCurrentRow(-1);
    loadDoc(ui->leDate->date());
}

void CR5MfDaily::processQtyChanged(const QString &arg1)
{
    C5LineEdit *l = static_cast<C5LineEdit*>(sender());
    int row = l->property("row").toInt();
    ui->wt->setData(row, 5, arg1.toDouble() * ui->wt->getData(row, 4).toDouble());
    ui->wt->countTotal(3);
}

void CR5MfDaily::on_btnDatePlus_clicked()
{
    ui->leDate->setDate(ui->leDate->date().addDays(1));
    ui->lstWorkers->setCurrentRow(-1);
    loadDoc(ui->leDate->date());
}

void CR5MfDaily::on_btnDateMinus_clicked()
{
    ui->leDate->setDate(ui->leDate->date().addDays(-1));
    ui->lstWorkers->setCurrentRow(-1);
    loadDoc(ui->leDate->date());
}

void CR5MfDaily::on_lstWorkers_currentRowChanged(int currentRow)
{
    Q_UNUSED(currentRow);
    loadDoc(ui->leDate->date());
}

void CR5MfDaily::on_leDate_returnPressed()
{
    ui->lstWorkers->setCurrentRow(-1);
    loadDoc(ui->leDate->date());
}
