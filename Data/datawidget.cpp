#include "datawidget.h"

DataWidget::DataWidget(QWidget *parent)
    : QWidget{parent}
{}

void DataWidget::setData(const QJsonObject &data)
{
    setWidgetData(this, data);
}
