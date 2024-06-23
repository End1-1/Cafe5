#ifndef AXREPORTING_H
#define AXREPORTING_H

#include <QObject>
#include <QJsonObject>

class QAxObject;

class AXReporting : public QObject
{
    Q_OBJECT
public:
    explicit AXReporting(QObject *parent = nullptr);

    void printReservation(const QJsonObject &jo);

    void replaceString(QAxObject *find, const QString &oldStr, const QString &newStr);

signals:
};

#endif // AXREPORTING_H
