#ifndef CE5USER_H
#define CE5USER_H

#include "ce5editor.h"

namespace Ui {
class CE5User;
}

class CE5User : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5User(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5User();

    virtual QString title() {return tr("User");}

    virtual QString table() {return "s_user";}

private:
    Ui::CE5User *ui;
};

#endif // CE5USER_H