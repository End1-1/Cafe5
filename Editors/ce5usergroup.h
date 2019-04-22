#ifndef CE5USERGROUP_H
#define CE5USERGROUP_H

#include "ce5editor.h"

namespace Ui {
class CE5UserGroup;
}

class CE5UserGroup : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5UserGroup(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5UserGroup();

    virtual QString title() {return tr("User group");}

    virtual QString table() {return "s_user_group";}

private:
    Ui::CE5UserGroup *ui;
};

#endif // CE5USERGROUP_H
