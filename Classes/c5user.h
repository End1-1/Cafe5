#ifndef C5USER_H
#define C5USER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QStringList>
#include <QMap>
#include <QVariant>

class C5User : public QObject
{
    Q_OBJECT

public:
    enum UserState {usAtWork, usNotAtWork};

    C5User(const QMap<QString, QVariant> &m);

    C5User(const QString &altPassword);

    C5User(int id);

    bool isValid();

    QString error();

    int id();

    QString fullName();

    int group();

    bool isActive();

    bool authByUsernamePass(const QString &username, const QString &pass);

    bool authByPinPass(const QString &pin, const QString &pass);

    bool authorize(const QString &altPassword);

    bool check(int permission);

    bool enterWork();

    bool leaveWork();

    UserState state();

    bool loadFromDB(int id);

private:
    C5User();

    QStringList fPermissions;

    bool fValid;

    QString fError;

    QVariant data(const QString &name);

    QMap<QString, QVariant> fUserData;

    UserState fState;

    void getState();

    void getPermissions();
};

#endif // C5USER_H
