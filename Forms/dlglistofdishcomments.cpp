#include "dlglistofdishcomments.h"
#include "ui_dlglistofdishcomments.h"
#include "ninterface.h"
#include <QShowEvent>
#include <QJsonArray>

DlgListOfDishComments::DlgListOfDishComments(const QString &path) :
    C5Dialog(nullptr),
    ui(new Ui::DlgListOfDishComments)
{
    ui->setupUi(this);
    connect(ui->kbd, SIGNAL(accept()), this, SLOT(kbdAccept()));
    connect(ui->kbd, SIGNAL(reject()), this, SLOT(reject()));
    fHttp->createHttpQueryLambda(path, {},
    [this](const QJsonObject & jdoc) {
        int col = 0;
        int row = 0;
        QJsonArray ja = jdoc["data"].toArray();

        for(int i = 0; i < ja.size(); i++) {
            QJsonObject j = ja.at(i).toObject();

            if(ui->tbl->rowCount() < row + 1) {
                ui->tbl->setRowCount(row + 1);
            }

            ui->tbl->setItem(row, col++, new QTableWidgetItem(j["f_name"].toString()));

            if(col == ui->tbl->columnCount()) {
                row++;
                col = 0;
            }
        }
    }, [](const QJsonObject & jerr) {
        Q_UNUSED(jerr);
    });
}

DlgListOfDishComments::~DlgListOfDishComments()
{
    delete ui;
}

bool DlgListOfDishComments::getComment(const QString &caption, QString &comment)
{
    DlgListOfDishComments *d = new DlgListOfDishComments("/engine/v2/waiter/menu/get-comments");
    d->ui->lbComment->setText(tr("Comment for") + " " + caption);
    bool result = d->exec() == QDialog::Accepted;

    if(result) {
        comment = d->fResult;
    }

    delete d;
    return result;
}

bool DlgListOfDishComments::getText(const QString &caption, const QString &valuesPath, QString &comment)
{
    auto *d = new DlgListOfDishComments(valuesPath);
    d->ui->lbComment->setText(caption);
    bool result = d->exec() == QDialog::Accepted;
    comment = d->fResult;
    d->deleteLater();
    return result;
}

void DlgListOfDishComments::showEvent(QShowEvent *e)
{
    C5Dialog::showEvent(e);

    if(e->spontaneous()) {
        return;
    }

    int dec = 5;
    int colWidths = 0;
    int hiddenColumns = 0;

    for(int i = 0; i < ui->tbl->columnCount(); i++) {
        if(ui->tbl->columnWidth(i) == 0) {
            hiddenColumns++;
        }

        colWidths += ui->tbl->columnWidth(i);
    }

    int freeSpace = ui->tbl->width() - colWidths - dec;
    int delta = 0;

    if(ui->tbl->columnCount() - hiddenColumns > 0) {
        delta = freeSpace / (ui->tbl->columnCount() - hiddenColumns);
    }

    for(int i = 0; i < ui->tbl->columnCount(); i++) {
        if(ui->tbl->columnWidth(i) == 0) {
            continue;
        }

        ui->tbl->setColumnWidth(i, ui->tbl->columnWidth(i) + delta);
    }

    int rh = ui->tbl->verticalHeader()->defaultSectionSize();
    int visibleRows = (ui->tbl->height() - dec) / rh;
    delta = (ui->tbl->height() - dec) - (visibleRows * rh);

    if(visibleRows < 1) {
        visibleRows = 1;
    }

    ui->tbl->verticalHeader()->setDefaultSectionSize(rh + (delta / visibleRows));
}

void DlgListOfDishComments::kbdAccept()
{
    if(ui->kbd->text().isEmpty()) {
        return;
    }

    fResult = ui->kbd->text();
    accept();
}

void DlgListOfDishComments::on_tbl_itemClicked(QTableWidgetItem *item)
{
    if(!item) {
        return;
    }

    if(ui->kbd->text().isEmpty() == false) {
        ui->kbd->setText(ui->kbd->text() + ", ");
    }

    ui->kbd->setText(ui->kbd->text() + item->text());
}
