#ifndef C5USER_H
#define C5USER_H

#include <QString>
#include <QJsonObject>
#include <QStringList>

class C5User
{
public:
    C5User();
    QString fFirst;
    QString fLast;
    QString fFull;
    QString fGroup;
    int fId;
    bool check(int permission);
    void fromJson(const QJsonObject &obj);
private:
    QStringList fPermissions;
};

#endif // C5USER_H
