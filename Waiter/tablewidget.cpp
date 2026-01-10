#include "tablewidget.h"
#include "ui_tablewidget.h"
#include "c5utils.h"
#include <QDebug>
#include <QStyle>

TableWidget::TableWidget(QWidget *parent) :
    C5Widget(parent),
    ui(new Ui::TableWidget)
{
    ui->setupUi(this);
    installEventFilter(this);
}

TableWidget::~TableWidget()
{
    delete ui;
}

void TableWidget::setTable(const TableItem &t)
{
    mTable = t;
    ui->tw1lbName->setText(mTable.name);
    ui->tw1lbStaff->setText(mTable.staffName);
    ui->tw1lbAmount->setText(float_str(mTable.amount, 2));
    ui->tw1lbAmount->setVisible(mTable.amount > 0);
    ui->lbComment->setText(mTable.comment);
    ui->lbComment->setVisible(!mTable.comment.isEmpty());
    ui->frame->setProperty("t1_state", mTable.tableState);
    ui->frame->style()->polish(ui->frame);
}

void TableWidget::updateTable(const TableItem &t)
{
    if(t.id == mTable.id) {
        setTable(t);
    }
}

void TableWidget::mouseReleaseEvent(QMouseEvent *me)
{
    QWidget::mouseReleaseEvent(me);
    emit clicked(mTable.id);
}
