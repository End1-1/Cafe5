#include "dlgcl.h"
#include "ui_dlgcl.h"
#include "c5config.h"
#include "c5database.h"
#include <QSettings>

DlgCL::DlgCL() :
    C5Dialog(),
    ui(new Ui::DlgCL)
{
    ui->setupUi(this);
    QStringList dbp(C5Config::dbParams());

    if(C5Config::hotelDatabase().isEmpty()) {
        QString where;
        //where = QString("where f_category=%1").arg(__c5config.getValue(param_waiter_debt_costumer_category).toInt());
        C5Database db;
        db.exec(QString("select f_id, concat_ws(', ',f_contact, f_name, f_address) as f_info, f_phone "
                        "from c_partners %1 order by f_name").arg(where));

        while(db.nextRow()) {
            int row = ui->tblGuest->addEmptyRow();
            ui->tblGuest->setString(row, 0, db.getString(0));
            ui->tblGuest->setString(row, 1, db.getString(1));
            ui->tblGuest->setString(row, 2, db.getString(2));
        }
    } else {
        C5Database db;
        db.exec(QString("select f_id, f_name from %1.f_city_ledger order by 2").arg(__c5config.hotelDatabase()));

        while(db.nextRow()) {
            int row = ui->tblGuest->addEmptyRow();
            ui->tblGuest->setString(row, 0, db.getString(0));
            ui->tblGuest->setString(row, 1, db.getString(1));
        }
    }

    connect(ui->kbd, SIGNAL(textChanged(QString)), this, SLOT(searchCL(QString)));
    connect(ui->kbd, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kbd, SIGNAL(reject()), this, SLOT(reject()));
    ui->tblGuest->configColumns({ 120, 400});
}

DlgCL::~DlgCL()
{
    delete ui;
}

bool DlgCL::getCL(QString &id, QString &name)
{
    DlgCL *d = new DlgCL();
    bool result = d->exec() == QDialog::Accepted;

    if(result) {
        id = d->ui->tblGuest->getString(d->fRow, 0);
        name = d->ui->tblGuest->getString(d->fRow, 1);
    }

    delete d;
    return result;
}

void DlgCL::kbdAccept()
{
    QModelIndexList ml = ui->tblGuest->selectionModel()->selectedRows();

    if(ml.count() == 0) {
        return;
    }

    fRow = ml.at(0).row();
    accept();
}

void DlgCL::searchCL(const QString &arg)
{
    ui->tblGuest->search(arg);
}

void DlgCL::on_tblGuest_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    kbdAccept();
}
