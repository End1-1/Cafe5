#ifndef CE5PARTNER_H
#define CE5PARTNER_H

#include "ce5editor.h"

namespace Ui {
class CE5Partner;
}

class CE5Partner : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Partner(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5Partner();

    virtual QString title();

    virtual QString table();

private:
    Ui::CE5Partner *ui;
};

#endif // CE5PARTNER_H
