#ifndef CE5ORDERMARK_H
#define CE5ORDERMARK_H

#include "ce5editor.h"

namespace Ui {
class CE5OrderMark;
}

class CE5OrderMark : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5OrderMark(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5OrderMark();

    virtual QString title() override;

    virtual QString table() override;

private:
    Ui::CE5OrderMark *ui;
};

#endif // CE5ORDERMARK_H
