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

    C5User();

    C5User(const QMap<QString, QVariant>& m);

    C5User(C5User *other);

    void copySettings(C5User *other);

    QString error();

    int id();

    QString fullName();

    QString shortFullName();

    QVariant value(const QString &key);

    int group();

    void authByUsernamePass(const QString &username, const QString &pass, NInterface *n, std::function<void (const QJsonObject&)> callback);

    void authByPinPass(const QString &pin, const QString &pass, NInterface *n, std::function<void (const QJsonObject&)> callback);

    void authorize(const QString &pin, NInterface *n, std::function<void(const QJsonObject&)> callback, std::function<void ()> errorCallback);

    bool check(int permission);

    bool enterWork();

    bool leaveWork();

    QString mSessionKey;

    QJsonObject fConfig;

    QMap<QString, QVariant> fUserData;

    QMap<int, QString> fSettings;

    void addPermission(int permission);

    void removePermission(int permission);

private:
    QVector<int> fPermissions;

    QString fError;

    QVariant data(const QString &name); \
    void getState(NInterface *n);

    void setUserData(const QJsonObject &jdoc);

};

#endif // C5USER_H
