#ifndef CE5MFPROCESSSTAGE_H
#define CE5MFPROCESSSTAGE_H

#include "ce5editor.h"

namespace Ui {
class CE5MFProcessStage;
}

class CE5MFProcessStage : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5MFProcessStage(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5MFProcessStage();

    virtual QString title();

    virtual QString table();

private:
    Ui::CE5MFProcessStage *ui;
};

#endif // CE5MFPROCESSSTAGE_H
