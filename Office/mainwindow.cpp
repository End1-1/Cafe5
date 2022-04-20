#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "socketconnection.h"
#include "rawmessage.h"
#include "dlgserversettings.h"
#include "messagelist.h"
#include "tablewidget.h"
#include "config.h"
#include <QPushButton>
#include <QByteArray>
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fConnectionStatusLabel = new QPushButton("", ui->statusbar);
    fConnectionStatusLabel->setMaximumSize(QSize(24, 24));
    fConnectionStatusLabel->setIconSize(QSize(20, 20));
    connect(fConnectionStatusLabel, &QPushButton::clicked, this, &MainWindow::connectionButtonClicked);
    fConnectionStatusLabel->setIcon(QIcon(":/wifi_off.png"));
    fLoginButton = new QPushButton("", ui->statusbar);
    fLoginButton->setMaximumSize(QSize(24, 24));
    fLoginButton->setIconSize(QSize(20, 20));
    fLoginButton->setIcon(QIcon(":/login2.png"));
    connect(fLoginButton, &QPushButton::clicked, this, &MainWindow::loginButtonClicked);
    ui->statusbar->addPermanentWidget(fLoginButton);
    ui->statusbar->addPermanentWidget(fConnectionStatusLabel);
    connect(SocketConnection::instance(), &SocketConnection::connected, this, &MainWindow::socketConnected);
    connect(SocketConnection::instance(), &SocketConnection::connectionLost, this, &MainWindow::socketDisconnected);
    connect(SocketConnection::instance(), &SocketConnection::externalDataReady, this, &MainWindow::socketDataReceived);
    connect(this, &MainWindow::dataReady, SocketConnection::instance(), &SocketConnection::sendData);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addTabWidget(QWidget *w, const QIcon &icon, const QString &title)
{
    ui->tw->addTab(w, icon, title);
    ui->tw->setCurrentIndex(ui->tw->count() - 1);
}

void MainWindow::socketConnected()
{
    fConnectionStatusLabel->setIcon(QIcon(":/wifib.png"));
}

void MainWindow::socketDisconnected()
{
    fConnectionStatusLabel->setIcon(QIcon(":/wifi_off.png"));
    fLoginButton->setEnabled(false);
}

void MainWindow::connectionButtonClicked()
{
    QString ip, username, password;
    int port;
    ServerConnection::getParams(ip, port, username, password);
    if (password.isEmpty() == false) {
        bool ok = false;
        QString passwordInput = QInputDialog::getText(this, tr("Password"), "", QLineEdit::Password, "", &ok);
        if (ok == false) {
            return;
        }
        if (password != passwordInput) {
            QMessageBox::critical(this, tr("Error"), tr("Invalid password"));
            return;
        }
    }
    DlgServerSettings d;
    d.exec();
}

void MainWindow::loginButtonClicked()
{

}

void MainWindow::socketDataReceived(quint16 cmd, QByteArray d)
{
    switch (cmd) {
    case MessageList::silent_auth:
        quint32 userid;
        RawMessage r(nullptr);
        r.readUInt(userid, d);
        if (userid > 0) {
            fConnectionStatusLabel->setIcon(QIcon(":/wifi_on.png"));
            fLoginButton->setEnabled(true);
        }
        break;
    }
}


void MainWindow::on_actionMovement_report_triggered()
{
    addTabWidget(new TableWidget(), QIcon(), tr("Goods movement"));
}

void MainWindow::on_tw_tabCloseRequested(int index)
{
    ui->tw->widget(index)->deleteLater();
    ui->tw->removeTab(index);
}

void MainWindow::on_tw_currentChanged(int index)
{
    SocketWidget *sw = static_cast<SocketWidget*>(ui->tw->widget(index));
    qDeleteAll(ui->toolBar->actions());
}
