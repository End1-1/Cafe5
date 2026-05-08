#pragma once
#include <QMap>

#define DISH_STATE_NONE 0
#define DISH_STATE_OK 1
#define DISH_STATE_MISTAKE 2
#define DISH_STATE_VOID 3
#define DISH_STATE_SET 4

extern const QMap<int, const char*> dish_state_names;
