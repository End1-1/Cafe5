#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "map.h"
#include "socketconnection.h"
#include "connections.h"
#include <QScrollBar>
#include <QMdiArea>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    fConnectionStatusLabel = new QLabel("", ui->statusbar);
    fConnectionStatusLabel->setMaximumSize(QSize(24, 24));
    fConnectionStatusLabel->setScaledContents(true);
    QPixmap px(":/wifi_off.png");
    fConnectionStatusLabel->setPixmap(px);
    ui->statusbar->addPermanentWidget(fConnectionStatusLabel);
    fMdiArea = new QMdiArea();
    fMdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    fMdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    connect(SocketConnection::instance(), &SocketConnection::connected, this, &MainWindow::socketConnected);
    connect(SocketConnection::instance(), &SocketConnection::connectionLost, this, &MainWindow::socketDisconnected);
    setCentralWidget(fMdiArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::socketConnected()
{
    QPixmap px(":/wifi_on.png");
    fConnectionStatusLabel->setPixmap(px);
}

void MainWindow::socketDisconnected()
{
    QPixmap px(":/wifi_off.png");
    fConnectionStatusLabel->setPixmap(px);
}


void MainWindow::on_actionOpenMap_triggered()
{
    auto *m = new Map();
    fMdiArea->addSubWindow(m);
    m->show();
}

void MainWindow::on_actionCommon_statistics_triggered()
{
    auto *m = new Connections();
    fMdiArea->addSubWindow(m);
    m->show();
}
