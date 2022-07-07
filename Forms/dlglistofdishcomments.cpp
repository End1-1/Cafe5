#include "dlglistofdishcomments.h"
#include "ui_dlglistofdishcomments.h"
#include "c5cafecommon.h"
#include "c5config.h"
#include "c5menu.h"

DlgListOfDishComments::DlgListOfDishComments(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgListOfDishComments)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowFullScreen);
    int col = 0;
    int row = 0;
    foreach (int id, dbdishcomment->list()) {
        if (ui->tbl->rowCount() < row + 1) {
            ui->tbl->setRowCount(row + 1);
        }
        ui->tbl->setItem(row, col++, new QTableWidgetItem(dbdishcomment->name(id)));
        if (col == ui->tbl->columnCount()) {
            row++;
            col = 0;
        }
    }
    connect(ui->kbd, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kbd, SIGNAL(reject()), this, SLOT(reject()));
}

DlgListOfDishComments::~DlgListOfDishComments()
{
    delete ui;
}

bool DlgListOfDishComments::getComment(const QString &caption, QString &comment)
{
    DlgListOfDishComments *d = new DlgListOfDishComments();
    d->ui->lbComment->setText(tr("Comment for") + " " + caption);

    int dec = 5;
    int colWidths = 0;
    int hiddenColumns = 0;
    for (int i = 0; i < d->ui->tbl->columnCount(); i++) {
        if (d->ui->tbl->columnWidth(i) == 0) {
            hiddenColumns++;
        }
        colWidths += d->ui->tbl->columnWidth(i);
    }
    int freeSpace = d->ui->tbl->width() - colWidths - dec;
    int delta = 0;
    if (d->ui->tbl->columnCount() - hiddenColumns > 0) {
        delta = freeSpace / (d->ui->tbl->columnCount() - hiddenColumns);
    }
    for (int i = 0; i < d->ui->tbl->columnCount(); i++) {
        if (d->ui->tbl->columnWidth(i) == 0) {
            continue;
        }
        d->ui->tbl->setColumnWidth(i, d->ui->tbl->columnWidth(i) + delta);
    }

    int rh = d->ui->tbl->verticalHeader()->defaultSectionSize();
    int visibleRows = (d->ui->tbl->height() - dec) / rh;
    delta = (d->ui->tbl->height() - dec) - (visibleRows * rh);
    d->ui->tbl->verticalHeader()->setDefaultSectionSize(rh + (delta / visibleRows));

    bool result = d->exec() == QDialog::Accepted;
    if (result) {
        comment = d->fResult;
    }
    delete d;
    return result;
}

void DlgListOfDishComments::kbdAccept()
{
    if (ui->kbd->text().isEmpty()) {
        return;
    }
    fResult = ui->kbd->text();
    accept();
}

void DlgListOfDishComments::on_tbl_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    if (ui->kbd->text().isEmpty() == false) {
        ui->kbd->setText(ui->kbd->text() + ", ");
    }
    ui->kbd->setText(ui->kbd->text() + item->text());
}
