#ifndef REMOVESHOPSALE_H
#define REMOVESHOPSALE_H

#include "c5database.h"
#include <QObject>

class RemoveShopSale : public QObject
{
    Q_OBJECT
public:
    explicit RemoveShopSale(const QStringList dbParams, QObject *parent = nullptr);

    bool remove(C5Database &db, const QString &id);

private:
    QStringList fDbParams;

};

#endif // REMOVESHOPSALE_H
