#include "c5salarypayment.h"
#include "ui_c5salarypayment.h"
#include "c5cashdoc.h"
#include "c5message.h"

C5SalaryPayment::C5SalaryPayment(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5SalaryPayment)
{
    ui->setupUi(this);
    ui->deDate->setDate(QDate::currentDate());
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 0, 300, 100, 100);
    openDoc(ui->deDate->date());
}

C5SalaryPayment::~C5SalaryPayment()
{
    delete ui;
}

QToolBar *C5SalaryPayment::toolBar()
{
    if (!fToolBar) {
        C5Widget::toolBar();
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(save()));
    }
    return fToolBar;
}

void C5SalaryPayment::save()
{
    C5Database db(fDBParams);
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->getString(i, 1).isEmpty()) {
            if (ui->tbl->lineEdit(i, 4)->getDouble() < 0.001) {
                continue;
            }
            C5CashDoc doc(fDBParams);
            doc.fNoSavedMessage = true;
            doc.setDate(ui->deDate->date());
            doc.setComment(tr("Salary payment"));
            doc.setCashOutput(1);
            doc.addRow(ui->tbl->getString(i, 2), ui->tbl->lineEdit(i, 4)->getDouble());
            doc.save(false, true);
            ui->tbl->setString(i, 1, doc.uuid());
            db[":f_date"] = ui->deDate->date();
            db[":f_worker"] = ui->tbl->getInteger(i, 0);
            db[":f_cashdoc"] = doc.uuid();
            db[":f_amount"] = ui->tbl->lineEdit(i, 4)->getDouble();
            db.insert("s_salary_payment", false);
        } else {
            if (ui->tbl->lineEdit(i, 4)->getDouble() < 0.001) {
                C5CashDoc::removeDoc(fDBParams, ui->tbl->getString(i, 1));
                continue;
            }
            db[":f_id"] = ui->tbl->getString(i, 1);
            db[":f_amount"] = ui->tbl->lineEdit(i, 4)->getDouble();
            db.exec("update a_header set f_amount=:f_amount where f_id=:f_id");
            db[":f_header"] = ui->tbl->getString(i, 1);
            db[":f_amount"] = ui->tbl->lineEdit(i, 4)->getDouble();
            db.exec("update e_cash set f_amount=:f_amount where f_header=:f_header");
            db[":f_cashdoc"] = ui->tbl->getString(i, 1);
            db[":f_date"] = ui->deDate->date();
            db[":f_amount"] = ui->tbl->lineEdit(i, 4)->getDouble();
            db.exec("update s_salary_payment set f_amount=:f_amount where f_cashdoc=:f_cashdoc and f_date=:f_date");
        }
    }
    C5Message::info(tr("Saved"));
    openDoc(ui->deDate->date());
}

void C5SalaryPayment::on_btnLeft_clicked()
{
    ui->deDate->setDate(ui->deDate->date().addDays(-1));
    openDoc(ui->deDate->date());
}

void C5SalaryPayment::on_btnRight_clicked()
{
    ui->deDate->setDate(ui->deDate->date().addDays(1));
    openDoc(ui->deDate->date());
}

void C5SalaryPayment::on_deDate_returnPressed()
{
    openDoc(ui->deDate->date());
}

int C5SalaryPayment::addRow()
{
    int row = ui->tbl->addEmptyRow();
    ui->tbl->createLineEdit(row, 4)->setValidator(new QDoubleValidator(0, 99999999, 2));
    return row;
}

void C5SalaryPayment::openDoc(const QDate &date)
{
    ui->tbl->setRowCount(0);
    C5Database db(fDBParams);
    db.exec("select sp.f_worker, concat_ws(' ', u.f_last, u.f_first) as f_workername, "
            "sum(if(sp.f_cashdoc is null, f_amount, -1*f_amount)) as f_balance "
            "from s_salary_payment sp "
            "left join s_User u on u.f_id=sp.f_worker "
            "group by 1 "
            "having sum(if(sp.f_cashdoc is null, f_amount, -1*f_amount))<>0 ");
    while (db.nextRow()) {
        int r = addRow();
        ui->tbl->setInteger(r, 0, db.getInt("f_worker"));
        ui->tbl->setString(r, 2, db.getString("f_workername"));
        ui->tbl->setDouble(r, 3, db.getDouble("f_balance"));
    }
    db[":f_date"] = date;
    db.exec("select sp.f_worker, sp.f_cashdoc, concat_ws(' ', u.f_last, u.f_first) as f_workername, "
            "f_amount "
            "from s_salary_payment sp "
            "left join s_User u on u.f_id=sp.f_worker "
            "where sp.f_date=:f_date and sp.f_cashdoc is not null");
    while (db.nextRow()) {
        int r = -1;
        for (int i = 0; i < ui->tbl->rowCount(); i++) {
            if (ui->tbl->getInteger(i, 0) == db.getInt("f_worker")) {
                r = i;
                break;
            }
        }
        if (r < 0) {
            r = addRow();
            ui->tbl->setInteger(r, 0, db.getInt("f_worker"));
            ui->tbl->setString(r, 2, db.getString("f_workername"));
            ui->tbl->setDouble(r, 3, 0);
        }
        ui->tbl->setString(r, 1, db.getString("f_cashdoc"));
        ui->tbl->lineEdit(r, 4)->setDouble(db.getDouble("f_amount"));
    }
}
