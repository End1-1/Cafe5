#include "dlgcl.h"
#include "ui_dlgcl.h"

DlgCL::DlgCL(const QStringList &dbParams) :
    C5Dialog(dbParams),
    ui(new Ui::DlgCL)
{
    ui->setupUi(this);
    QStringList dbp(C5Config::dbParams());
    dbp[1] = C5Config::hotelDatabase();
    C5Database db(dbp);
    db.exec("select f_id, f_name from f_city_ledger order by 2");
    while (db.nextRow()) {
        int row = ui->tblGuest->addEmptyRow();
        ui->tblGuest->setString(row, 0, db.getString(0));
        ui->tblGuest->setString(row, 1, db.getString(1));
    }

    connect(ui->kbd, SIGNAL(textChanged(QString)), this, SLOT(searchCL(QString)));
    connect(ui->kbd, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kbd, SIGNAL(reject()), this, SLOT(reject()));
    ui->tblGuest->setColumnWidths(ui->tblGuest->columnCount(), 120, 400);
}

DlgCL::~DlgCL()
{
    delete ui;
}

bool DlgCL::getCL(QString &id, QString &name)
{
    DlgCL *d = new DlgCL(C5Config::dbParams());
    bool result = d->exec() == QDialog::Accepted;
    if (result) {
        id = d->ui->tblGuest->getString(d->fRow, 0);
        name = d->ui->tblGuest->getString(d->fRow, 1);
    }
    delete d;
    return result;
}

void DlgCL::kbdAccept()
{
    QModelIndexList ml = ui->tblGuest->selectionModel()->selectedRows();
    if (ml.count() == 0) {
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
