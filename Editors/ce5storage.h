#ifndef CE5STORAGE_H
#define CE5STORAGE_H

#include "ce5editor.h"

namespace Ui {
class CE5Storage;
}

class CE5Storage : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Storage(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5Storage();

    virtual QString title();

    virtual QString table();

private:
    Ui::CE5Storage *ui;
};

#endif // CE5STORAGE_H
