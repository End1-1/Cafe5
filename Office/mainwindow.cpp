#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "socketconnection.h"
#include "rawmessage.h"
#include "dlgserversettings.h"
#include "messagelist.h"
#include <QMdiArea>
#include <QPushButton>
#include <QByteArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fConnectionStatusLabel = new QPushButton("", ui->statusbar);
    fConnectionStatusLabel->setMaximumSize(QSize(24, 24));
    fConnectionStatusLabel->setIconSize(QSize(20, 20));
    connect(fConnectionStatusLabel, &QPushButton::clicked, this, &MainWindow::connectionButtonClicked);
    QPixmap px(":/wifi_off.png");
    fConnectionStatusLabel->setIcon(px);
    ui->statusbar->addPermanentWidget(fConnectionStatusLabel);
    fMdiArea = new QMdiArea();
    fMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    fMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    connect(SocketConnection::instance(), &SocketConnection::connected, this, &MainWindow::socketConnected);
    connect(SocketConnection::instance(), &SocketConnection::connectionLost, this, &MainWindow::socketDisconnected);
    connect(SocketConnection::instance(), &SocketConnection::externalDataReady, this, &MainWindow::socketDataReceived);
    connect(this, &MainWindow::dataReady, SocketConnection::instance(), &SocketConnection::sendData);
    setCentralWidget(fMdiArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::socketConnected()
{
    QPixmap px(":/wifi_on.png");
    fConnectionStatusLabel->setIcon(px);
}

void MainWindow::socketDisconnected()
{
    QPixmap px(":/wifi_off.png");
    fConnectionStatusLabel->setIcon(px);
}

void MainWindow::connectionButtonClicked()
{
    DlgServerSettings d;
    d.exec();
}

void MainWindow::socketDataReceived(quint16 cmd, const QByteArray &d)
{
    qDebug() << cmd << d;
}


void MainWindow::on_actionMovement_report_triggered()
{
    RawMessage r(nullptr);
    r.setHeader(SocketConnection::instance()->getTcpPacketNumber(), 0, MessageList::dll_op);
    r.putString("rwjzstore");
    r.putUShort(2022);
    r.putUByte(3);
    r.putUByte(2);
    r.putUByte(2);
    emit dataReady(r.data());
}
