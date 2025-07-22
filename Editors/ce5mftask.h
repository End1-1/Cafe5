#ifndef CE5MFTASK_H
#define CE5MFTASK_H

#include "ce5editor.h"

namespace Ui {
class CE5MFTask;
}

class CE5MFTask : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5MFTask();
    ~CE5MFTask();

    virtual QString title() override;

    virtual QString table() override;

    virtual void clear() override;

    virtual void setId(int id) override;

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data) override;

private:
    Ui::CE5MFTask *ui;

    void setOptions();
};

#endif // CE5MFTASK_H
