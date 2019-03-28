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
    ui->wDownload->setVisible(false);
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
    ui->txtLog->moveCursor(QTextCursor::End);
    ui->txtLog->textCursor().insertText(str);
    ui->txtLog->textCursor().insertText("\n");
    ui->txtLog->verticalScrollBar()->setValue(ui->txtLog->verticalScrollBar()->maximum());
    qApp->processEvents();
}

void MainDialog::download(bool d)
{
    ui->wDownload->setVisible(d);
    ui->progressBar->setValue(0);
}

void MainDialog::downloadProgress(int p)
{
    if (!ui->wDownload->isVisible()) {
        ui->wDownload->setVisible(true);
    }
    ui->progressBar->setValue(p);
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
    if (QMessageBox::question(this, tr("Update"), tr("Are you sure to cancel update?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        qApp->quit();
    }
}

void MainDialog::on_btnUpdate_clicked()
{
    ui->txtLog->clear();
    fUpdateThread = new UpdateThread(this);
    fUpdateThread->fServerAddress = "http://10.1.0.70";
    fUpdateThread->fUpdateVersion = 1;
    connect(fUpdateThread, SIGNAL(message(QString)), this, SLOT(message(QString)));
    connect(fUpdateThread, SIGNAL(allDone(QString)), this, SLOT(allDone(QString)));
    connect(fUpdateThread, SIGNAL(download(bool)), this, SLOT(download(bool)));
    connect(fUpdateThread, SIGNAL(progress(int)), this, SLOT(downloadProgress(int)));
    ui->btnUpdate->setEnabled(false);
    ui->wDownload->setVisible(false);
    fUpdateThread->start();
}
