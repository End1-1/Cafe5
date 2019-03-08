#ifndef CE5HALLS_H
#define CE5HALLS_H

#include "ce5editor.h"

namespace Ui {
class CE5Halls;
}

class CE5Halls : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Halls(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5Halls();

    virtual QString title() {return tr("Hall");}

    virtual QString table() {return "h_halls";}

private:
    Ui::CE5Halls *ui;
};

#endif // CE5HALLS_H
