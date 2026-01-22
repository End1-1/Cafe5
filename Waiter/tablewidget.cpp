#include "tablewidget.h"
#include "ui_tablewidget.h"
#include "c5utils.h"
#include "format_date.h"
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
    QDate dateOpen = QDate::fromString(mTable.dateOpen(), FORMAT_DATE_TO_STR_MYSQL);

    if(dateOpen < QDate::currentDate()) {
        QString htmlDate = QString("<p style='font-size:10px; margin:0;'>%1</p><p style='font-size:10px; margin:0;'>%2</p>")
                           .arg(dateOpen.toString(FORMAT_DATE_TO_STR),
                                mTable.timeOpen());
        ui->tw1lbTime->setTextFormat(Qt::RichText);
        ui->tw1lbTime->setText(htmlDate);
    } else {
        ui->tw1lbTime->setText(mTable.timeOpen());
    }

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
