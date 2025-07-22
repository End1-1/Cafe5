#include "c5dishgroupaction.h"
#include "ui_c5dishgroupaction.h"
#include "c5cache.h"

C5DishGroupAction::C5DishGroupAction() :
    C5Dialog(),
    ui(new Ui::C5DishGroupAction)
{
    ui->setupUi(this);
    ui->leState->setSelector(ui->leStateName, cache_dish_menu_state);
    ui->leStore->setSelector(ui->leStoreName, cache_goods_store);
}

C5DishGroupAction::~C5DishGroupAction()
{
    delete ui;
}

bool C5DishGroupAction::setStore(QString &id)
{
    return setParam(id, ui->leStore, ui->chStore);
}

bool C5DishGroupAction::setState(QString &id)
{
    return setParam(id, ui->leState, ui->chState);
}

void C5DishGroupAction::on_btnCancel_clicked()
{
    reject();
}

void C5DishGroupAction::on_btnSave_clicked()
{
    accept();
}

bool C5DishGroupAction::setParam(QString &id, C5LineEditWithSelector *l, QCheckBox *c)
{
    if (c->isChecked()) {
        if (!l->isEmpty()) {
            id = l->text();
            return true;
        }
    }
    return false;
}
