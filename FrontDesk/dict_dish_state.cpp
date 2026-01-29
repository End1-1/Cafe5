#include "dict_dish_state.h"
#include <QObject>

const QMap<int, const char*> dish_state_names = {
    {DISH_STATE_NONE, QT_TRANSLATE_NOOP("DishState", "None")},
    {DISH_STATE_OK, QT_TRANSLATE_NOOP("DishState", "Normal")},
    {DISH_STATE_MISTAKE, QT_TRANSLATE_NOOP("DishState", "Mistake")},
    {DISH_STATE_VOID, QT_TRANSLATE_NOOP("DishState", "Void")},
    {DISH_STATE_SET, QT_TRANSLATE_NOOP("DishState", "Set")}
};
