#include "dlgguest.h"
#include "ui_dlgguest.h"
#include "c5config.h"
#include "c5database.h"

DlgGuest::DlgGuest() :
    C5Dialog(),
    ui(new Ui::DlgGuest)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    C5Database db;
    db.exec(QString("select r.f_id, r.f_invoice, r.f_room, g.guest, r.f_startdate, r.f_enddate, r.f_man+r.f_woman+r.f_child "
                    "from %1.f_reservation r "
                    "left join %1.guests g on g.f_id=r.f_guest "
                    "where r.f_state=1 order by r.f_room ").arg(C5Config::hotelDatabase()));
    while (db.nextRow()) {
        int row = ui->tblGuest->addEmptyRow();
        ui->tblGuest->setString(row, 0, db.getString(1));
        ui->tblGuest->setString(row, 1, db.getString(0));
        ui->tblGuest->setString(row, 2, db.getString(2));
        ui->tblGuest->setString(row, 3, db.getString(3));
        ui->tblGuest->setString(row, 4, db.getDate(4).toString(FORMAT_DATE_TO_STR));
        ui->tblGuest->setString(row, 5, db.getDate(5).toString(FORMAT_DATE_TO_STR));
        ui->tblGuest->setInteger(row, 6, db.getInt(6));
    }
    connect(ui->kbd, SIGNAL(textChanged(QString)), this, SLOT(searchGuest(QString)));
    connect(ui->kbd, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kbd, SIGNAL(reject()), this, SLOT(reject()));
    ui->tblGuest->configColumns({0, 0, 140, 500, 100, 100, 100});
}

DlgGuest::~DlgGuest()
{
    delete ui;
}

bool DlgGuest::getGuest(QString &res, QString &inv, QString &room, QString &guest)
{
    DlgGuest *d = new DlgGuest();
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
