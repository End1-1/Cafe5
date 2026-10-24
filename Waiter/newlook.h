#pragma once

#include <QPair>
#include <QString>
#include <QVector>

namespace WaiterUi {
inline bool &newLook()
{
    static bool on = false;
    return on;
}

/** Menu departments (d_part1): id and name, filled when the waiter menu is loaded. */
inline QVector<QPair<int, QString>> &menuParts()
{
    static QVector<QPair<int, QString>> parts;
    return parts;
}
}
