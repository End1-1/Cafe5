#ifndef CE5TABLE_H
#define CE5TABLE_H

#include "ce5editor.h"

namespace Ui {
class CE5Table;
}

class CE5Table : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Table(QWidget *parent = nullptr);

    ~CE5Table();

    virtual QString title() {return tr("Table");}

    virtual QString table() {return "h_tables";}

private:
    Ui::CE5Table *ui;
};

#endif // CE5TABLE_H
