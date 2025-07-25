#ifndef CE5MENUNAME_H
#define CE5MENUNAME_H

#include "ce5editor.h"

namespace Ui {
class CE5MenuName;
}

class CE5MenuName : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5MenuName(QWidget *parent = nullptr);

    ~CE5MenuName();

    virtual QString title();

    virtual QString table();

    virtual bool checkData(QString &err);

private:
    Ui::CE5MenuName *ui;
};

#endif // CE5MENUNAME_H
