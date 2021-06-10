#include "dialog.h"
#include "ui_dialog.h"
#include "c5crypt.h"
#include <QFileDialog>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}


void Dialog::on_btnFilename_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this, "License file");
    if (filename.isEmpty()) {
        return;
    }
    ui->leFilename->setText(filename);
}

void Dialog::on_btnGenerate_clicked()
{
    C5Crypt c;
    QByteArray key = ui->leHDD->text().toUtf8();
    QByteArray in = ui->leDate->text().toUtf8();
    QByteArray out;
    c.cryptData(key, in, out);
    QFile f(ui->leFilename->text());
    if (f.open(QIODevice::WriteOnly)) {
        f.write(out);
        f.close();
    }
}
