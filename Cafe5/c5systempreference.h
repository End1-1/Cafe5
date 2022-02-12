#ifndef C5SYSTEMPREFERENCE_H
#define C5SYSTEMPREFERENCE_H

#include <QObject>

class C5SystemPreference : public QObject
{
    Q_OBJECT
public:
    explicit C5SystemPreference(QObject *parent = nullptr);

    static bool checkDecimalPointAndSeparator();

signals:

};

#endif // C5SYSTEMPREFERENCE_H
