#ifndef ARMSOFT_H
#define ARMSOFT_H

#include <QObject>
#include <QJsonObject>
#include <QSqlDatabase>

class ArmSoft : public QObject
{
    Q_OBJECT
public:
    explicit ArmSoft(const QJsonObject &data, QObject *parent = nullptr);
    bool exportToAS(const QString &orderUuid, QString &err);

private:
    QJsonObject fData;
    QMap<int, QMap<QString, QVariant> > partnersMap;
    QMap<QString, QMap<QString, QVariant> > servicesMap;
    QMap<QString, QMap<QString, QVariant> > goodsMap;
    QMap<QString, QMap<QString, QVariant> > unitsMap;


    bool getIndexes(QString &err, QSqlDatabase &dbas);
    void recordToMap(QMap<QString, QVariant> &m, QSqlQuery &q, QStringList &fields);
};

#endif // ARMSOFT_H
