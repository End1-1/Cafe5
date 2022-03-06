#include "monitor.h"
#include "ui_monitor.h"
#include "serverthread.h"
#include <QThread>
#include <QDebug>
#include <QDateTime>

Monitor::Monitor(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Monitor)
{
    ui->setupUi(this);
}

Monitor::~Monitor()
{
    delete ui;
}
