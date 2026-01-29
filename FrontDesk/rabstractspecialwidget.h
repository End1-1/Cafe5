#pragma once
#include "c5widget.h"
#include <QJsonArray>

class RAbstractSpecialWidget : public C5Widget
{
    Q_OBJECT
public:
    RAbstractSpecialWidget(QWidget *parent = nullptr);

    virtual QJsonArray getValues();

    virtual void setup(const QJsonObject &jdoc);

    virtual void save();

signals:
    void goAccept();
};
