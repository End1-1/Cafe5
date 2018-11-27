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
    explicit CE5MenuName(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5MenuName();

    virtual QString title();

    virtual QString table();

private:
    Ui::CE5MenuName *ui;
};

#endif // CE5MENUNAME_H
