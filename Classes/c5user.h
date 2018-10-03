#ifndef C5USER_H
#define C5USER_H

#include <QString>
#include <QJsonObject>

class C5User
{
public:
    C5User();
    QString fFirst;
    QString fLast;
    QString fFull;
    QString fGroup;
    int fId;
    void fromJson(const QJsonObject &obj);
};

#endif // C5USER_H
