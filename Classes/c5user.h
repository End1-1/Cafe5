#ifndef C5USER_H
#define C5USER_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QStringList>
#include <QMap>
#include <QVariant>

class NInterface;

class C5User : public QObject
{
    Q_OBJECT

public:
    enum UserState {usAtWork, usNotAtWork};

    C5User();

    C5User(const QMap<QString, QVariant>& m);

    C5User(const QString &altPassword, NInterface *n);

    C5User(int id, NInterface *n);

    QString error();

    int id();

    QString fullName();

    QVariant value(const QString &key);

    int group();

    void authByUsernamePass(const QString &username, const QString &pass, NInterface *n, std::function<void (const QJsonObject&)> callback);

    bool authByPinPass(const QString &pin, const QString &pass, NInterface *n);

    bool authorize(const QString &altPassword, NInterface *n);

    bool check(int permission);

    bool enterWork();

    bool leaveWork();

    UserState state();

    bool loadFromDB(int id, NInterface *n);

    QJsonObject fConfig;

    QMap<int, QString> fSettings;

private:
    QVector<int> fPermissions;

    QString fError;

    QVariant data(const QString &name);

    QMap<QString, QVariant> fUserData;

    UserState fState;

    void getState(NInterface *n);

    void getPermissions(NInterface *n);
};

#endif // C5USER_H
