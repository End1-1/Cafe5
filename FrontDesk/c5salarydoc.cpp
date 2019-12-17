#include "c5salarydoc.h"
#include "ui_c5salarydoc.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5cashdoc.h"
#include "c5mainwindow.h"
#include "c5lineeditwithselector.h"

C5SalaryDoc::C5SalaryDoc(const QStringList &dbParams, QWidget *parent) :
    C5Document(dbParams, parent),
    ui(new Ui::C5SalaryDoc)
{
    ui->setupUi(this);
    fIcon = ":/employee.png";
    fLabel = tr("Salary document");
    ui->leDocnumber->setPlaceholderText(QString("%1").arg(genNumber(DOC_TYPE_SALARY)));
    ui->leCash->setSelector(fDBParams, ui->leCashName, cache_cash_names);
    ui->leShift->setSelector(fDBParams, ui->leShiftName, cache_salary_shift);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 150, 200, 0, 300, 100);
    setPaid();
}

C5SalaryDoc::~C5SalaryDoc()
{
    delete ui;
}

bool C5SalaryDoc::openDoc(const QString &id)
{
    if (id.isEmpty()) {
        return false;
    }
    fUUID = id;
    C5Database db(fDBParams);
    db[":f_id"] = id;
    db.exec("select * from a_header where f_id=:f_id");
    if (!db.nextRow()) {
        return false;
    }
    ui->leRemarks->setText(db.getString("f_comment"));
    ui->leTotal->setDouble(db.getDouble("f_amount"));
    ui->leDocnumber->setText(db.getString("f_userid"));
    db[":f_id"] = id;
    db.exec("select * from s_salary_options where f_id=:f_id");
    if (db.nextRow()) {
        fCashUUID = db.getString("f_cashdoc");
        ui->leShift->setValue(db.getInt("f_shift"));
    } else {
        db[":f_id"] = fUUID;
        db.insert("s_salary_options", false);
    }
    if (!fCashUUID.isEmpty()) {
        C5CashDoc *d = new C5CashDoc(fDBParams);
        if (d->openDoc(fCashUUID)) {
            ui->deCashDate->setDate(d->date());
            ui->leCash->setValue(d->outputCash());
        }
    }
    db[":f_header"] = fUUID;
    db.exec("select b.f_id, b.f_position, g.f_name, b.f_user,"
            "concat(u.f_last, ' ', u.f_first) as f_employee, b.f_amount "
            "from s_salary_body b "
            "left join s_user u on u.f_id=b.f_user "
            "inner join s_user_group g on g.f_id=u.f_group "
            "where b.f_header=:f_header");
    while (db.nextRow()) {
        int row = newRow();
        ui->tbl->setString(row, 0, db.getString("f_id"));
        ui->tbl->lineEditWithSelector(row, 1)->setValue(db.getInt("f_position"));
        ui->tbl->setInteger(row, 3, db.getInt("f_user"));
        ui->tbl->setString(row, 4, db.getString("f_employee"));
        ui->tbl->lineEditWithSelector(row, 5)->setDouble(db.getDouble("f_amount"));
    }
    setPaid();
    return true;
}

QToolBar *C5SalaryDoc::toolBar()
{
    if (!fToolBar) {
        C5Widget::toolBar();
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(save()));
        fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(removeDoc()));
        fToolBar->addAction(QIcon(":/cash.png"), tr("Create cash document"), this, SLOT(createCashDocument()));
        fToolBar->addAction(QIcon(":/calculator.png"), tr("Count salary"), this, SLOT(countSalary()));
    }
    return fToolBar;
}

void C5SalaryDoc::save()
{
    QString error;
    if (ui->leShift->getInteger() == 0) {
        error += tr("Shift is not selected") + "<br>";
    }
    if (ui->tbl->rowCount() == 0) {
        error += tr("Empty document") + "<br>";
    }
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
    if (ui->leDocnumber->isEmpty()) {
        ui->leDocnumber->setPlaceholderText(genNumberText(DOC_TYPE_SALARY));
        ui->leDocnumber->setText(ui->leDocnumber->placeholderText());
        updateGenNumber(ui->leDocnumber->getInteger(), DOC_TYPE_SALARY);
    }
    C5Database db(fDBParams);
    db[":f_operator"] = __userid;
    db[":f_userid"] = ui->leDocnumber->text();
    db[":f_comment"] = ui->leRemarks->text();
    db[":f_type"] = DOC_TYPE_SALARY;
    db[":f_state"] = DOC_STATE_SAVED;
    db[":f_date"] = ui->deDate->date();
    db[":f_createdate"] = QDate::currentDate();
    db[":f_createtime"] = QTime::currentTime();
    db[":f_partner"] = 0;
    db[":f_amount"] = ui->leTotal->getDouble();
    if (fUUID.isEmpty()) {
        fUUID = C5Database::uuid();
        db[":f_id"] = fUUID;
        db.insert("a_header", false);
        db[":f_id"] = fUUID;
        db[":f_cashdoc"] = fCashUUID;
        db.insert("s_salary_options", false);
    } else {
        db.update("a_header", where_id(fUUID));
    }
    db[":f_cashdoc"] = fCashUUID;
    db[":f_shift"] = ui->leShift->getInteger();
    db.update("s_salary_options", where_id(fUUID));
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        db[":f_header"] = fUUID;
        db[":f_user"] = ui->tbl->getInteger(i, 3);
        db[":f_position"] = ui->tbl->lineEditWithSelector(i, 1)->getInteger();
        db[":f_amount"] = ui->tbl->lineEditWithSelector(i, 5)->getDouble();
        db[":f_row"] = i;
        if (ui->tbl->getString(i, 0).isEmpty()) {
            ui->tbl->setString(i, 0, C5Database::uuid());
            db[":f_id"] = ui->tbl->getString(i, 0);
            db.insert("s_salary_body", false);
        } else {
            db.update("s_salary_body", where_id(ui->tbl->getString(i, 0)));
        }
    }
    if (!fCashUUID.isEmpty()) {
        C5CashDoc *doc = new C5CashDoc(fDBParams);
        if (doc->openDoc(fCashUUID)) {
            doc->setRelation(true);
            doc->setCashOutput(ui->leCash->getInteger());
            doc->setDate(ui->deCashDate->date());
            doc->setComment(tr("Salary") + " #" + ui->leDocnumber->text() + "/" + ui->deDate->text());
            doc->updateRow(0, tr("Salary") + " #" + ui->leDocnumber->text() + "/" + ui->deDate->text(), ui->leTotal->getDouble());
            doc->save();
        }
        delete doc;
    }
    C5Message::info(tr("Saved"));
}

void C5SalaryDoc::countSalary()
{
    if (ui->leShift->getInteger() == 0) {
        C5Message::error(tr("Shift was not selected"));
        return;
    }
    QMap<int, int> posCount;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        posCount[ui->tbl->lineEditWithSelector(i, 1)->getInteger()] = posCount[ui->tbl->lineEditWithSelector(i, 1)->getInteger()] + 1;
    }
    C5Database db(fDBParams);
    db[":f_shift"] = ui->leShift->getInteger();
    db.exec("select f_start, f_end from s_salary_shift_time where f_shift=:f_shift");
    QList<QTime> timeStart, timeEnd;
    while (db.nextRow()) {
        timeStart.append(db.getTime(0));
        timeEnd.append(db.getTime(1));
    }
    double amount = 0;

        db[":f_shift"] = ui->leShift->getInteger();
        db[":f_datecash1"] = ui->deDate->date();
        db[":f_datecash2"] = ui->deDate->date();
        db.exec("select sum(f_amounttotal) from o_header "
                "where f_datecash between :f_datecash1 and :f_datecash2 "
                "and f_shift=:f_shift "
                "and f_state=2 ");
        db.nextRow();
        amount += db.getDouble(0);

    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        db[":f_shift"] = ui->leShift->getInteger();
        db[":f_position"] = ui->tbl->lineEditWithSelector(i, 1)->getInteger();
        db.exec("select f_individual, f_percent, f_round from s_salary_account where f_shift=:f_shift and f_position=:f_position");
        if (db.nextRow()) {
            if (db.getInt(0) > 0) {
                ui->tbl->lineEditWithSelector(i, 5)->setDouble(db.getInt(0));
            } else {
                double counted = (amount * db.getDouble(1)) / posCount[ui->tbl->lineEditWithSelector(i, 1)->getInteger()];
//                if (db.getInt(2) > 0) {
//                    counted = trunc(ceil(counted / (pow(10, db.getInt(2)))) * pow(10, db.getInt(2)));
//                } else if (db.getInt(2) < 0) {
//                    counted = trunc(floor(counted / (pow(10, db.getInt(2)))) * pow(10, db.getInt(2)));
//                }
                ui->tbl->lineEditWithSelector(i, 5)->setDouble(counted);
            }
        }
    }
}

void C5SalaryDoc::countAmounts(const QString &arg1)
{
    Q_UNUSED(arg1);
    double total = 0;
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        total += ui->tbl->lineEdit(i, 5)->getDouble();
    }
    ui->leTotal->setDouble(total);
}

void C5SalaryDoc::createCashDocument()
{
    if (fUUID.isEmpty()) {
        C5Message::error(tr("Save first"));
        return;
    }
    if (!fCashUUID.isEmpty()) {
        C5Message::error(tr("Already created"));
        return;
    }
    C5CashDoc *doc = new C5CashDoc(fDBParams);
    doc->setRelation(true);
    doc->setCashOutput(ui->leCash->getInteger());
    doc->setDate(ui->deCashDate->date());
    doc->setComment(tr("Salary") + " #" + ui->leDocnumber->text() + "/" + ui->deDate->text());
    doc->addRow(tr("Salary") + " #" + ui->leDocnumber->text() + "/" + ui->deDate->text(), ui->leTotal->getDouble());
    doc->save();
    fCashUUID = doc->uuid();
    delete doc;
    C5Database db(fDBParams);
    db[":f_cashdoc"] = fCashUUID;
    db.update("s_salary_options" , where_id(fUUID));
    setPaid();
}

void C5SalaryDoc::removeDoc(bool showmessage)
{
    if (showmessage) {
        if (C5Message::question(tr("Confirm to remove")) != QDialog::Accepted) {
            return;
        }
    }
    C5Database db(fDBParams);
    db.startTransaction();
    db[":_id"] = fUUID;
    db.exec("delete from a_header where f_id=:f_id");
    db[":f_id"] = fUUID;
    db.exec("delete from s_salary_options where f_id=:f_id");
    db[":f_header"] = fUUID;
    db.exec("delete from s_salary_body where f_header=:f_header");
    db.commit();
    __mainWindow->removeTab(this);
}

void C5SalaryDoc::on_btnAddEmployee_clicked()
{
    QList<QVariant> vals;
    if (!C5Selector::getValue(fDBParams, cache_users, vals)) {
        return;
    }
    C5Database db(fDBParams);
    db[":f_id"] = vals.at(0).toInt();
    db.exec("select g.f_id, concat(u.f_last, ' ', u.f_first) as f_employee "
            "from s_user u "
            "inner join s_user_group g on g.f_id=u.f_group "
            "where u.f_id=:f_id");
    if (db.nextRow()) {
        int row = newRow();
        ui->tbl->lineEditWithSelector(row, 1)->setValue(db.getInt(0));
        ui->tbl->setInteger(row, 3, vals.at(0).toInt());
        ui->tbl->setString(row, 4, db.getString(1));
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
    C5LineEditWithSelector *l1 = ui->tbl->createLineEditWiSelector(row, 1);
    C5LineEditWithSelector *l2 = ui->tbl->createLineEditWiSelector(row, 2);
    l2->setReadOnly(true);
    l1->setSelector(fDBParams, l2, cache_users_groups);
    l1 = ui->tbl->createLineEditWiSelector(row, 5);
    l1->setValidator(new QDoubleValidator(0, 999999999, 2));
    connect(l1, SIGNAL(textChanged(QString)), this, SLOT(countAmounts(QString)));
    l1->setFocus();
    return row;
}

void C5SalaryDoc::setPaid()
{
    QString text;
    QPalette p = ui->lePaid->palette();
    if (fCashUUID.isEmpty()) {
        p.setColor(QPalette::Text, Qt::red);
        text = tr("Unpaid");
    } else {
        p.setColor(QPalette::Text, QColor::fromRgb(20, 100, 50));
        text = tr("Paid");
    }
    ui->lePaid->setPalette(p);
    ui->lePaid->setText(text);
}
