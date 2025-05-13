#include "maindialog.h"
#include "ui_maindialog.h"
#include "updatethread.h"
#include <QLayout>
#include <QMessageBox>
#include <QScrollBar>
#include <QTextCursor>

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    setFixedSize(this->size());
    fUpdateThread = nullptr;
}

MainDialog::~MainDialog()
{
    if (fUpdateThread) {
        fUpdateThread->quit();
        fUpdateThread->requestInterruption();
        fUpdateThread->wait();
    }
    delete ui;
}

void MainDialog::message(const QString &str)
{
    ui->lst->addItem(new QListWidgetItem(str));
    ui->lst->verticalScrollBar()->setValue(ui->lst->verticalScrollBar()->maximum());
}

void MainDialog::downloadProgress(int p)
{
    QString s = ui->lst->item(ui->lst->count() - 1)->text();
    QString text = QString("Download progress %1%").arg(p);
    QListWidgetItem *item;
    if (s.contains("download progress", Qt::CaseInsensitive)) {
        item = ui->lst->item(ui->lst->count());
    } else {
        item = new QListWidgetItem(ui->lst);
    }
    item->setText(text);
}

void MainDialog::allDone(const QString &str)
{
    if (!str.isEmpty()) {
        message(str);
    }
    ui->btnUpdate->setEnabled(true);
    if (fUpdateThread) {
        fUpdateThread->quit();
        fUpdateThread = nullptr;
    }
}

void MainDialog::on_btnCancel_clicked()
{
    if (QMessageBox::question(this, tr("Update"), tr("Are you sure to cancel update?"),
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        qApp->quit();
    }
}

void MainDialog::on_btnUpdate_clicked()
{
    fUpdateThread = new UpdateThread(this);
    fUpdateThread->fServerAddress = "https://update.picasso.am";
    fUpdateThread->fUpdateVersion = 1;
    connect(fUpdateThread, SIGNAL(message(QString)), this, SLOT(message(QString)));
    connect(fUpdateThread, SIGNAL(allDone(QString)), this, SLOT(allDone(QString)));
    connect(fUpdateThread, SIGNAL(download(bool)), this, SLOT(download(bool)));
    connect(fUpdateThread, SIGNAL(progress(int)), this, SLOT(downloadProgress(int)));
    ui->btnUpdate->setEnabled(false);
    fUpdateThread->start();
}
