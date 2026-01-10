#ifndef C5PRINTRECIPTA4_H
#define C5PRINTRECIPTA4_H

#include <QObject>

class C5User;

class C5PrintReciptA4 : public QObject
{
    Q_OBJECT
public:
    explicit C5PrintReciptA4(const QString &orderid, C5User *user, QObject *parent = nullptr);

    bool print(QString &err);

private:
    QString fOrderUUID;

    C5User* mUser;

    QString loadTemplate(const QString &name);

    QString applyTemplate(QString html, const QMap<QString, QString>& vars);

    QString makeGoodsTable(const QList<QMap<QString, QVariant> >& body);
signals:

};

#endif // C5PRINTRECIPTA4_H
