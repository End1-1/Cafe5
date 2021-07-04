#ifndef C5USER_H
#define C5USER_H

#include <QString>
#include <QJsonObject>
#include <QStringList>

class C5User : public QObject
{
    Q_OBJECT

public:
    C5User(const QMap<QString, QVariant> &m);

    C5User(const QString &altPassword);

    C5User(int id);

    bool isValid();

    QString error();

    int id();

    QString fullName();

    int group();

    int fId;

    bool authorize(const QString &altPassword);

    bool check(int permission);

private:
    C5User();

    QStringList fPermissions;

    bool fValid;

    QString fError;

    QVariant data(const QString &name);

    QMap<QString, QVariant> fUserData;
};

#endif // C5USER_H
