#include "tablewidgetv1.h"
#include "ui_tablewidgetv1.h"
#include "c5utils.h"
#include "datadriver.h"
#include <QDebug>
#include <QStyle>

TableWidgetV1::TableWidgetV1(QWidget *parent) :
    TableWidget(parent),
    ui(new Ui::TableWidgetV1)
{
    ui->setupUi(this);
    installEventFilter(this);
    configOrder("");
}

TableWidgetV1::~TableWidgetV1()
{
    delete ui;
}

QLabel *TableWidgetV1::labelTable()
{
    return ui->tw1lbName;
}

QLabel *TableWidgetV1::labelStaff()
{
    return ui->tw1lbStaff;
}

QLabel *TableWidgetV1::labelTime()
{
    return ui->tw1lbTime;
}

QLabel *TableWidgetV1::labelAmount()
{
    return ui->tw1lbAmount;
}

QLabel *TableWidgetV1::labelComment()
{
    return ui->lbComment;
}

QFrame *TableWidgetV1::frame()
{
    return ui->frame;
}
