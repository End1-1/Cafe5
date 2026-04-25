#ifndef DATADIALOG_H
#define DATADIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QObject>
#include "datahelper.h"

class DataDialog : public QDialog, public DataHelper
{
    Q_OBJECT
public:
    DataDialog(QWidget *parent = nullptr);

    void setData(const QJsonObject &data);

    bool saveData();
};

#endif // DATADIALOG_H
