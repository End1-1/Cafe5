#ifndef C5DISHGROUPACTION_H
#define C5DISHGROUPACTION_H

#include "c5dialog.h"

namespace Ui {
class C5DishGroupAction;
}

class C5LineEditWithSelector;
class QCheckBox;

class C5DishGroupAction : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5DishGroupAction(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5DishGroupAction();

    bool setStore(QString &id);

    bool setState(QString &id);

private slots:
    void on_btnCancel_clicked();

    void on_btnSave_clicked();

private:
    Ui::C5DishGroupAction *ui;

    bool setParam(QString &id, C5LineEditWithSelector *l, QCheckBox *c);
};

#endif // C5DISHGROUPACTION_H
