#pragma once

#include <QWidget>
#include <QJsonObject>

class C5User;

class WPWidget : public QWidget
{
    Q_OBJECT
public:
    WPWidget(C5User *user, QWidget *parent = nullptr);

    C5User* mUser;

    virtual void setup() = 0;

    QJsonObject mParams;
};
