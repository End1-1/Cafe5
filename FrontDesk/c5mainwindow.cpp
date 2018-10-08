#include "c5mainwindow.h"
#include "ui_c5mainwindow.h"
#include "c5message.h"
#include "c5connection.h"
#include <QCloseEvent>

C5MainWindow::C5MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::C5MainWindow)
{
    ui->setupUi(this);
    __c5config.fParentWidget = this;
}

C5MainWindow::~C5MainWindow()
{
    delete ui;
}

void C5MainWindow::closeEvent(QCloseEvent *event)
{
    if (C5Message::question(tr("Are you sure to close application?")) != QDialog::Accepted) {
        event->ignore();
        return;
    }
    QMainWindow::closeEvent(event);
}

void C5MainWindow::on_action_triggered()
{
    if (C5Message::question(tr("Are you sure to close application?")) == QDialog::Accepted) {
        qApp->quit();
    }
}

void C5MainWindow::on_actionConnection_triggered()
{
    C5Connection::go<C5Connection>();
}
