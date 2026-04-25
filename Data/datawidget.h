#ifndef DATAWIDGET_H
#define DATAWIDGET_H

#include <QJsonObject>
#include <QWidget>
#include "datahelper.h"

class DataWidget : public QWidget, public DataHelper
{
    Q_OBJECT
public:
    explicit DataWidget(QWidget *parent = nullptr);

    virtual void setData(const QJsonObject &data);

signals:
};

#endif // DATAWIDGET_H
