#include "dlglistofdishcomments.h"
#include "ui_dlglistofdishcomments.h"
#include "c5cafecommon.h"
#include "c5config.h"

DlgListOfDishComments::DlgListOfDishComments(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgListOfDishComments)
{
    ui->setupUi(this);
    int col = 0;
    int row = 0;
    foreach (QString s, C5CafeCommon::fDishComments) {
        if (ui->tbl->rowCount() < row + 1) {
            ui->tbl->setRowCount(row + 1);
        }
        ui->tbl->setString(row, col++, s);
        if (col == ui->tbl->columnCount()) {
            row++;
            col = 0;
        }
    }
}

DlgListOfDishComments::~DlgListOfDishComments()
{
    delete ui;
}

bool DlgListOfDishComments::getComment(QString &comment)
{
    DlgListOfDishComments *d = new DlgListOfDishComments();
    d->ui->lbComment->setText(comment);
    d->showFullScreen();
    d->hide();
    d->ui->tbl->fitColumnsToWidth();
    d->ui->tbl->fitRowToHeight();
    bool result = d->exec() == QDialog::Accepted;
    if (result) {
        comment = d->fResult;
    }
    delete d;
    return result;
}

void DlgListOfDishComments::on_btnCancel_clicked()
{
    reject();
}

void DlgListOfDishComments::on_tbl_itemClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }
    fResult = item->text();
    if (fResult.isEmpty()) {
        return;
    }
    accept();
}
