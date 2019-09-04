#ifndef DOUBLEUTILS_H
#define DOUBLEUTILS_H

#include <QLocale>

#define float_str(value, f) QLocale().toString(value, 'f', f).remove(QRegExp("(?!\\d[\\.\\,][1-9]+)0+$")).remove(QRegExp("[\\.\\,]$"))

#endif // DOUBLEUTILS_H
