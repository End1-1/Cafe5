#include "datadialog.h"

DataDialog::DataDialog(QWidget *parent)
    : QDialog(parent)
    , DataHelper()
{}

void DataDialog::setData(const QJsonObject &data)
{
    setWidgetData(this, data);
}

bool DataDialog::saveData()
{
    return DataHelper::saveData(this);
}
