#ifndef CE5STOREREASON_H
#define CE5STOREREASON_H

#include "ce5editor.h"

namespace Ui {
class CE5StoreReason;
}

class CE5StoreReason : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5StoreReason(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5StoreReason() override;

    QString title() override;

    QString table() override;

    virtual bool checkData(QString &err) override;

private:
    Ui::CE5StoreReason *ui;
};

#endif // CE5STOREREASON_H
