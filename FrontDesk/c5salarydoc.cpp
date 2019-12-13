#include "c5salarydoc.h"
#include "ui_c5salarydoc.h"
#include "c5selector.h"
#include "c5cache.h"
#include "c5lineeditwithselector.h"

C5SalaryDoc::C5SalaryDoc(const QStringList &dbParams, QWidget *parent) :
    C5Widget(dbParams, parent),
    ui(new Ui::C5SalaryDoc)
{
    ui->setupUi(this);
    fIcon = ":/employee.png";
    fLabel = tr("Salary document");
    ui->leCash->setSelector(fDBParams, ui->leCashName, cache_cash_names);
    ui->tbl->setColumnWidths(ui->tbl->columnCount(), 0, 100, 200, 0, 300, 100);
}

C5SalaryDoc::~C5SalaryDoc()
{
    delete ui;
}

QToolBar *C5SalaryDoc::toolBar()
{
    if (!fToolBar) {
        C5Widget::toolBar();
        fToolBar->addAction(QIcon(":/save.png"), tr("Save"), this, SLOT(save()));
        fToolBar->addAction(QIcon(":/delete.png"), tr("Remove"), this, SLOT(removeDoc()));
    }
    return fToolBar;
}

void C5SalaryDoc::save()
{
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        if (ui->tbl->lineEditWithSelector(i, 1)->getInteger() == 0) {
            C5Message::error(tr("Check all positions"));
            return;
        }
    }
    C5Database db(fDBParams);
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
    } else {
        db.update("a_header", where_id(fUUID));
    }
    for (int i = 0; i < ui->tbl->rowCount(); i++) {
        db[":f_user"] = ui->tbl->getInteger(i, 3);
        db[":f_position"] = ui->tbl->lineEditWithSelector(i, 1)->getInteger();
        db[":f_amount"] = ui->tbl->lineEditWithSelector(i, 5)->getDouble();
        if (ui->tbl->getString(i, 0).isEmpty()) {
            ui->tbl->setString(i, 0, C5Database::uuid());
            db[":f_id"] = ui->tbl->getString(i, 0);
            db.insert("s_salary_body", false);
        } else {
            db.update("s_salary_body", where_id(ui->tbl->getString(i, 0)));
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
        int row = ui->tbl->addEmptyRow();
        C5LineEditWithSelector *l1 = ui->tbl->createLineEditWiSelector(row, 1);
        C5LineEditWithSelector *l2 = ui->tbl->createLineEditWiSelector(row, 2);
        l2->setReadOnly(true);
        l1->setSelector(fDBParams, l2, cache_users_groups);
        l1->setValue(db.getInt(0));
        ui->tbl->setInteger(row, 3, vals.at(0).toInt());
        ui->tbl->setString(row, 4, db.getString(1));
        l1 = ui->tbl->createLineEditWiSelector(row, 5);
        l1->setValidator(new QDoubleValidator(0, 999999999, 2));
        connect(l1, SIGNAL(textChanged(QString)), this, SLOT(countAmounts(QString)));
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
