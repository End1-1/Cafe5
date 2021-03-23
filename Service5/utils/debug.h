#ifndef DEBUG_H
#define DEBUG_H

#include <QObject>

#ifdef QT_DEBUG
#include <QDebug>
#endif

#ifdef QT_DEBUG
#define __debug_log(x) qDebug() << x;
#else
#define __debug_log(x)
#endif

#endif // DEBUG_H
