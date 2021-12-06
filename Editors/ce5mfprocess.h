#ifndef CE5MFPROCESS_H
#define CE5MFPROCESS_H

#include "ce5editor.h"

namespace Ui {
class CE5MFProcess;
}

class CE5MFProcess : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5MFProcess(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5MFProcess();

    virtual QString title();

    virtual QString table();

private:
    Ui::CE5MFProcess *ui;
};

#endif // CE5MFPROCESS_H
