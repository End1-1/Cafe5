#pragma once

#include "wpwidget.h"

namespace Ui
{
class WPLast30Sessions;
}

class QListWidgetItem;

class WPLast30Sessions : public WPWidget
{
    Q_OBJECT
public:
    explicit WPLast30Sessions(C5User *user, QWidget *parent = nullptr);

    ~WPLast30Sessions();

private slots:
    void on_lst_itemClicked(QListWidgetItem *item);

private:
    Ui::WPLast30Sessions* ui;

    virtual void setup() override;
};
