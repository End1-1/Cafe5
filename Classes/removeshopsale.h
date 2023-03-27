#ifndef REMOVESHOPSALE_H
#define REMOVESHOPSALE_H

#include <QObject>

class RemoveShopSale : public QObject
{
    Q_OBJECT
public:
    explicit RemoveShopSale(const QStringList dbParams, QObject *parent = nullptr);

    bool remove(const QString &id);

private:
    QStringList fDbParams;

};

#endif // REMOVESHOPSALE_H
