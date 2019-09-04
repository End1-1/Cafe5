#ifndef USER_H
#define USER_H

#include <QList>
#include <QJsonObject>

class User
{
public:
    User();
    bool pr(int permission);
    void reg(const QJsonObject &jo);
    void unreg();
    
private:
    QList<int> fPermissions;
    QJsonObject fData;
};

extern User *__user;

#endif // USER_H
