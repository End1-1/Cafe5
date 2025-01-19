#include "c5aboutdlg.h"
#include "ui_c5aboutdlg.h"
#include "fileversion.h"
#include <QDesktopServices>
#include <QUrl>

c5aboutdlg::c5aboutdlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::c5aboutdlg)
{
    ui->setupUi(this);
    ui->lbVersion->setText(QString("Version: %").arg(FileVersion::getVersionString(qApp->applicationFilePath())));
}

c5aboutdlg::~c5aboutdlg()
{
    delete ui;
}

void c5aboutdlg::on_btnClose_clicked()
{
    accept();
}

void c5aboutdlg::on_openWeb_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.picassocloud.com"));
}
