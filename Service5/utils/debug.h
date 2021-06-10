#ifndef DEBUG_H
#define DEBUG_H

#include <QObject>
#include <QElapsedTimer>

#ifdef QT_DEBUG
#include <QDebug>
#endif

#ifdef QT_DEBUG
#define __debug_log(x) qDebug() << x;
#else
#define __debug_log(x) __debug_log_to_file(x)
#endif

void __debug_log_to_file(const QVariant &msg);

#endif // DEBUG_H
