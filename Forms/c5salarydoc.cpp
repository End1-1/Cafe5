#include "c5salarydoc.h"
#include "ui_c5salarydoc.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5daterange.h"
#include "c5cashdoc.h"
#include "c5storedraftwriter.h"
#include "c5mainwindow.h"
#include "c5dateedit.h"
#include "c5cashdoc.h"
#include "c5user.h"
#include "c5lineeditwithselector.h"

C5SalaryDoc::C5SalaryDoc(const QStringList &dbParams, QWidget *parent) :
    C5Document(dbParams, parent),
    ui(new Ui::C5SalaryDoc)
{
    ui->setupUi(this);
    fIcon = ":/employee.png";
    fLabel = tr("Salary document");
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 150, 200, 0, 300, 100, 80, 80, 80, 80, 100);
    fDate = QDate::currentDate();
    fDate = fDate.addDays((fDate.day() - 1) * -1);
    ui->deDate->setText(fDate.toString("MMMM-yyyy"));
    getSessions();
    openDoc();
}

C5SalaryDoc::~C5SalaryDoc()
{
    delete ui;
}

bool C5SalaryDoc::openDoc()
{
    if (ui->cbShift->currentData().toInt() == 0) {
        return false;
    }
    double total = 0;
    C5Database db(fDBParams);
    db[":f_shift"] = ui->cbShift->currentData();
    db.exec("select b.f_id, b.f_worker, b.f_position, g.f_name,"
            "concat(u.f_last, ' ', u.f_first) as f_employee, b.f_amount, "
            "b.f_indate, b.f_intime, b.f_outdate, b.f_outtime, b.f_paid "
            "from s_salary_attendance b "
            "left join s_user u on u.f_id=b.f_worker "
            "inner join s_user_group g on g.f_id=u.f_group "
            "where b.f_shift=:f_shift");
    ui->tbl->setRowCount(0);
    while (db.nextRow()) {
        int row = newRow();
        ui->tbl->setString(row, 0, db.getString("f_id"));
        ui->tbl->lineEditWithSelector(row, 1)->setValue(db.getInt("f_position"));
        ui->tbl->setInteger(row, 3, db.getInt("f_worker"));
        ui->tbl->setString(row, 4, db.getString("f_employee"));
        ui->tbl->lineEditWithSelector(row, 5)->setDouble(db.getDouble("f_amount"));
        ui->tbl->getWidget<C5DateEdit>(row, 6)->setDate(db.getDate("f_indate"));
        ui->tbl->lineEdit(row, 7)->setText(db.getTime("f_intime").toString("HH:mm:ss"));
        ui->tbl->getWidget<C5DateEdit>(row, 8)->setDate(db.getDate("f_outdate"));
        ui->tbl->lineEdit(row, 9)->setText(db.getTime("f_outtime").toString("HH:mm:ss"));
        qDebug() << db.getString("f_paid").isEmpty() << db.getDateTime("f_paid");
        ui->tbl->setString(row, 10, db.getString("f_paid").isEmpty()  ? tr("No") : tr("Yes") );
        total += db.getDouble("f_amount");
    }
    ui->leTotal->setDouble(total);
    return true;
}

QToolBar *C5SalaryDoc::toolBar()
{
    if (!fToolBar) {
        C5Widget::toolBar();
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(save()));
        fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(removeDoc()));
        fToolBar->addAction(QIcon(":/cash.png"), tr("Create cash document"), this, SLOT(createCashDocument()));
        fToolBar->addAction(QIcon(":/employee.png"), tr("Get employes list"), this, SLOT(getEmployesInOutList()));
    }
    return fToolBar;
}

void C5SalaryDoc::save()
{
    removeSalaryOfShift();
    QString error;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->lineEditWithSelector(i, 1)->getInteger() == 0) {
            error += tr("Check all positions") + "<br>";
            break;
        }
    }
    if (!error.isEmpty()) {
        C5Message::error(error);
        return;
    }
    C5Database db(fDBParams);
    db[":f_shift"] = ui->cbShift->currentData();
    db.exec("delete from s_salary_attendance where f_shift=:f_shift");
    db[":f_shift"] = ui->cbShift->currentData();
    db.exec("delete from s_salary_payment where f_shift=:f_shift");
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        db[":f_date"] = ui->tbl->getWidget<C5DateEdit>(i, 6)->date();
        db[":f_shift"] = ui->cbShift->currentData();
        db[":f_worker"] = ui->tbl->getInteger(i, 3);
        db[":f_position"] = ui->tbl->getInteger(i, 1);
        db[":f_amount"] = ui->tbl->lineEdit(i, 5)->getDouble();
        db[":f_indate"] = ui->tbl->getWidget<C5DateEdit>(i, 6)->date();
        db[":f_intime"] = QTime::fromString(ui->tbl->lineEdit(i, 7)->text(), "HH:mm:ss");
        db[":f_outdate"] = ui->tbl->getWidget<C5DateEdit>(i, 8)->date();
        db[":f_outtime"] = QTime::fromString(ui->tbl->lineEdit(i, 9)->text(), "HH:mm:ss");
        ui->tbl->setInteger(i, 0, db.insert("s_salary_attendance"));
    }
    countSalary();
    db[":f_date"] = ui->cbShift->currentData(Qt::UserRole + 2);
    db[":f_shift"] = ui->cbShift->currentData();
    db.exec("insert into s_salary_payment (f_worker, f_date, f_shift, f_amount)  "
            "select  distinct(f_worker), :f_date, :f_shift, sum(f_amount) from s_salary_attendance "
            "where f_shift=:f_shift group by 1 ");
}

void C5SalaryDoc::createCashDocument()
{
    removeSalaryOfShift();
    C5Database db(fDBParams);
    C5StoreDraftWriter dw(db);
    int docnum = genNumber(DOC_TYPE_CASH);
    updateGenNumber(docnum, DOC_TYPE_CASH);
    QString fUuid;
    dw.writeAHeader(fUuid, QString::number(docnum), DOC_STATE_SAVED, DOC_TYPE_CASH, __user->id(),
                    QDate::currentDate(),
                    QDate::currentDate(), QTime::currentTime(),
                    0, ui->leTotal->getDouble(),
                    QString("%1 %2").arg("Salary").arg(ui->cbShift->currentText()), 1, 1,
                    ui->cbShift->currentData().toInt());
    dw.writeAHeaderCash(fUuid, 0, 1, 0, "", "");
    QString idout;
    dw.writeECash(idout, fUuid, 1, -1, QString("%1 %2").arg("Salary").arg(ui->cbShift->currentText()),
                  ui->leTotal->getDouble(), idout, 0);
    db[":f_shift"] = ui->cbShift->currentData();
    db[":f_paid"] = fUuid;
    db.exec("update s_salary_attendance set f_paid=:f_paid where f_shift=:f_shift");
    openDoc();
}

void C5SalaryDoc::countSalaryResponse(const QJsonObject &jdoc)
{
    openDoc();
    fHttp->httpQueryFinished(sender());
    C5Message::info(tr("Saved"));
}

void C5SalaryDoc::getEmployesInOutList()
{
    C5Database db(fDBParams);
    //db[":f_shift"] = ui->deDate->date();
    db.exec("select u.f_group, ug.f_name as position_name, u.f_id, concat(u.f_last, ' ', u.f_first) as employee_name "
            "from s_salary_inout io "
            "left join s_user u on u.f_id=io.f_user "
            "left join s_user_group ug on ug.f_id=u.f_id "
            "where io.f_datein=:f_date and io.f_dateout is not null ");
    ui->tbl->clearContents();
    ui->tbl->setRowCount(0);
    while (db.nextRow()) {
        int r = newRow();
        ui->tbl->lineEditWithSelector(r, 1)->setValue(db.getInt("f_group"));
        ui->tbl->setInteger(r, 3, db.getInt("f_id"));
        ui->tbl->setString(r, 4, db.getString("employee_name"));
    }
}

void C5SalaryDoc::on_btnAddEmployee_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValue(fDBParams, cache_users, vals)) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = vals.at(1).toInt();
    db.exec("select g.f_id, concat(u.f_last, ' ', u.f_first) as f_employee, "
            "g.f_starttime, g.f_duration "
            "from s_user u "
            "inner join s_user_group g on g.f_id=u.f_group "
            "where u.f_id=:f_id");
    if (db.nextRow()) {
        for (int i = 0; i < ui->tbl->rowCount(); i++) {
            if (ui->tbl->getInteger(i, 3) == vals.at(1).toInt()) {
                C5Message::error(tr("This employee already in list"));
                return;
            }
        }
        int row = newRow();
        ui->tbl->lineEditWithSelector(row, 1)->setValue(db.getInt(0));
        ui->tbl->setInteger(row, 3, vals.at(1).toInt());
        ui->tbl->setString(row, 4, db.getString(1));
        ui->tbl->getWidget<C5DateEdit>(row, 6)->setDate(ui->cbShift->currentData(Qt::UserRole + 1).toDate());
        ui->tbl->lineEdit(row, 7)->setText(db.getString("f_starttime"));
        QDateTime dt = QDateTime::fromString(QString("%1 %2").arg(ui->deDate->text(),
                                             ui->tbl->lineEdit(row, 7)->text()),
                                             "dd/MM/yyyy HH:mm:ss");
        dt = dt.addSecs(60 * 60 * db.getInt("f_duration"));
        ui->tbl->getWidget<C5DateEdit>(row, 8)->setDate(dt.date());
        ui->tbl->lineEdit(row, 9)->setText(dt.time().toString("HH:mm:ss"));
    }
}

void C5SalaryDoc::on_btnRemoveEmploye_clicked()
{
    QModelIndexList ml = ui->tbl->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    ui->tbl->removeRow(ml.at(0).row());
}

int C5SalaryDoc::newRow()
{
    int row = ui->tbl->addEmptyRow();
    C5LineEditWithSelector *l1 = ui->tbl->createWidget<C5LineEditWithSelector>(row, 1);
    C5LineEditWithSelector *l2 = ui->tbl->createWidget<C5LineEditWithSelector>(row, 2);
    l2->setReadOnly(true);
    l1->setSelector(fDBParams, l2, cache_users_groups);
    l1 = ui->tbl->createWidget<C5LineEditWithSelector>(row, 5);
    l1->setValidator(new QDoubleValidator(0, 999999999, 2));
    connect(l1, SIGNAL(textChanged(QString)), this, SLOT(countAmounts(QString)));
    l1->setFocus();
    ui->tbl->createWidget<C5DateEdit>(row, 6);
    ui->tbl->createLineEdit(row, 7);
    ui->tbl->createWidget<C5DateEdit>(row, 8);
    ui->tbl->createLineEdit(row, 9);
    return row;
}

void C5SalaryDoc::getSessions()
{
    ui->cbShift->clear();
    C5Database db(fDBParams);
    QString ds =  QString("%1-%2-01")
                  .arg(fDate.year(), 4, 10, QChar('0'))
                  .arg(fDate.month(),  2, 10,  QChar('0'));
    QDate date1 = QDate::fromString(ds, FORMAT_DATE_TO_STR_MYSQL);
    QDate date2 = date1.addDays(date1.daysInMonth() - 1);
    db[":date1"] = date1;
    db[":date2"] = date2;
    db.exec("select * from s_working_sessions where cast(f_close as date) between :date1 and :date2 order by f_id desc");
    QSet<QDate> dates;
    while (db.nextRow()) {
        ui->cbShift->addItem(
            QString("%1, %2 - %3").arg(QString::number(db.getInt("f_id")),
                                       db.getDateTime("f_open").toString(FORMAT_DATETIME_TO_STR),
                                       db.getDateTime("f_close").toString(FORMAT_DATETIME_TO_STR)),
            db.getInt("f_id"));
        int i = ui->cbShift->count() - 1;
        ui->cbShift->setItemData(i, db.getDateTime("f_open"), Qt::UserRole + 1);
        ui->cbShift->setItemData(i, db.getDateTime("f_close").date(), Qt::UserRole + 2);
        dates.insert(db.getDate("f_close"));
    }
    bool getagain = false;
    for (QDate d1 = date1; d1 < date2; d1 = d1.addDays(1)) {
        if (!dates.contains(d1)) {
            db[":f_open"] = d1;
            db[":f_close"] = d1;
            db[":f_user"] = 1;
            db.insert("s_working_sessions");
            getagain = true;
        }
    }
    if (getagain) {
        getSessions();
    }
}

void C5SalaryDoc::countSalary()
{
    fHttp->createHttpQuery("/engine/cash/countsalary.php",
    QJsonObject{{"action", "count"}, {"cashsession", ui->cbShift->currentData().toInt()}},
    SLOT(countSalaryResponse(QJsonObject)));
}

void C5SalaryDoc::removeSalaryOfShift()
{
    C5Database db(fDBParams);
    db[":f_shift"] = ui->cbShift->currentData();
    db.exec("select f_paid from s_salary_attendance where f_shift=:f_shift");
    QStringList paid;
    while (db.nextRow()) {
        paid.append(db.getString("f_paid"));
    }
    for (const QString &s : paid) {
        C5CashDoc::removeDoc(db, s);
    }
}

double C5SalaryDoc::salaryOfPosition(C5Database &db, int pos, int worker)
{
    db[":f_shift"] = ui->cbShift->currentData();
    db[":f_position"] = pos;
    db.exec("select count(f_id) from s_salary_attendance where f_position=:f_position and f_shift=:f_shift");
    db.nextRow();
    int posCount = db.getInt(0);
    db[":f_worker"] = worker;
    db[":f_shift"] = ui->cbShift->currentData();
    db.exec("select * from s_salary_attendance where f_shift=:f_shift and f_worker=:f_worker");
    if (db.nextRow() == false) {
        return 0;
    }
    QDate date1 = db.getDate("f_indate"), date2 = db.getDate("f_outdate");
    QTime time1 = db.getTime("f_intime"), time2 = db.getTime("f_outtime");
    db[":f_position"] = pos;
    db.exec("select * from s_salary_rules where f_position=:f_position");
    if (db.nextRow() == false) {
        return 0;
    }
    double total = 0.0;
    double fixed = db.getDouble("f_fixed");
    double min = db.getDouble("f_min");
    double max = db.getDouble("f_max");
    double own = db.getDouble("f_owntotal") / 100;
    int dep = db.getInt("f_dep");
    double totalVal = db.getDouble("f_total");
    if (fixed < 0) {
        return -1;
    }
    if (dep > 0) {
        db[":f_printtime1"] = QDateTime::fromString(date1.toString("dd/MM/yyyy") + " " + time1.toString("HH:mm:ss"),
                              "dd/MM/yyyy HH:mm:ss");
        db[":f_printtime2"] = QDateTime::fromString(date2.toString("dd/MM/yyyy") + " " + time2.toString("HH:mm:ss"),
                              "dd/MM/yyyy HH:mm:ss");
        db[":f_state"] = 1;
        db[":f_position"] = pos;
        db.exec("select sum(b.f_total * (p2.f_salary_percent / 100)) from o_body b "
                "left join d_dish d on d.f_id=b.f_dish "
                "left join d_part2 p2 on p2.f_id=d.f_part "
                "where b.f_state=:f_state and p2.f_position=:f_position "
                "and b.f_printtime between :f_printtime1 and :f_printtime2 ");
        db.nextRow();
        dep = db.getInt(0);
    }
    if (totalVal > 0.0) {
        db[":f_state"] = 2;
        db[":f_working_session"] = ui->cbShift->currentData();
        db[":f_totalval"] = totalVal / 100;
        db.exec("select sum(oh.f_amounttotal) * :f_totalval from o_header oh "
                "where oh.f_state=:f_state and f_working_session=:f_working_session");
        db.nextRow();
        totalVal = db.getDouble(0);
    }
    if (own > 0.001) {
        db[":f_state"] = 2;
        db[":f_owntotal"] = own;
        db[":f_staff"] = worker;
        db[":f_working_session"] = ui->cbShift->currentData();
        db.exec("select sum(oh.f_amounttotal) * :f_owntotal from o_header oh "
                "where oh.f_state=:f_state "
                "and oh.f_working_session=:f_working_session "
                "and oh.f_staff=:f_staff");
        db.nextRow();
        own = db.getDouble(0);
    }
    total += dep + totalVal + own;
    total /= posCount;
    total += fixed;
    total = (total < min) && (min > 0.001) ? min : total;
    total = (total > max) && (max > 0.001) ? max : total;
    return total;
}

void C5SalaryDoc::on_toolButton_clicked()
{
    fDate = fDate.addDays(fDate.daysInMonth() * -1);
    ui->deDate->setText(fDate.toString("MMMM-yyyy"));
    getSessions();
}

void C5SalaryDoc::on_toolButton_2_clicked()
{
    fDate = fDate.addDays(1 * (fDate.daysInMonth() - 1));
    ui->deDate->setText(fDate.toString("MMMM-yyyy"));
    getSessions();
}

void C5SalaryDoc::on_deDate_returnPressed()
{
    openDoc();
}

void C5SalaryDoc::on_cbShift_currentIndexChanged(int index)
{
    openDoc();
}
