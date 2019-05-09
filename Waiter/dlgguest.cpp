#include "dlgguest.h"
#include "ui_dlgguest.h"

DlgGuest::DlgGuest() :
    C5Dialog(C5Config::dbParams()),
    ui(new Ui::DlgGuest)
{
    ui->setupUi(this);
    QStringList dbp(C5Config::dbParams());
    dbp[1] = C5Config::hotelDatabase();
    C5Database db(dbp);
    db.exec("select r.f_id, r.f_invoice, r.f_room, g.guest "
            "from f_reservation r "
            "left join guests g on g.f_id=r.f_guest "
            "where r.f_state=1 order by r.f_room ");
    while (db.nextRow()) {
        int row = ui->tblGuest->addEmptyRow();
        ui->tblGuest->setString(row, 0, db.getString(1));
        ui->tblGuest->setString(row, 1, db.getString(0));
        ui->tblGuest->setString(row, 2, db.getString(2));
        ui->tblGuest->setString(row, 3, db.getString(3));
    }

    connect(ui->kbd, SIGNAL(textChanged(QString)), this, SLOT(searchGuest(QString)));
    connect(ui->kbd, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kbd, SIGNAL(reject()), this, SLOT(reject()));
    ui->tblGuest->setColumnWidths(ui->tblGuest->columnCount(), 0, 0, 140, 500);
}

DlgGuest::~DlgGuest()
{
    delete ui;
}

bool DlgGuest::getGuest(QString &res, QString &inv, QString &room, QString &guest)
{
    DlgGuest *d = new DlgGuest();
    d->showFullScreen();
    d->hide();
    d->ui->kbd->adjustSize();
    qApp->processEvents();
    bool result = d->exec() == QDialog::Accepted;
    if (result) {
        inv = d->ui->tblGuest->getString(d->fRow, 0);
        res = d->ui->tblGuest->getString(d->fRow, 1);
        room = d->ui->tblGuest->getString(d->fRow, 2);
        guest = d->ui->tblGuest->getString(d->fRow, 3);
    }
    delete d;
    return result;
}

void DlgGuest::searchGuest(const QString &txt)
{
    for (int i = 0; i < ui->tblGuest->rowCount(); i++) {
        bool hidden = true;
        for (int j = 2; j < ui->tblGuest->columnCount(); j++) {
            if (ui->tblGuest->getString(i, j).contains(txt, Qt::CaseInsensitive)) {
                hidden = false;
            }
        }
        ui->tblGuest->setRowHidden(i, hidden);
    }
}

void DlgGuest::kbdAccept()
{
    QModelIndexList ml = ui->tblGuest->selectionModel()->selectedRows();
    if (ml.count() == 0) {
        return;
    }
    fRow = ml.at(0).row();
    accept();
}

void DlgGuest::on_tblGuest_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    kbdAccept();
}
