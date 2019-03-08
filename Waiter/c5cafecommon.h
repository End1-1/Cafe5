#ifndef C5CAFECOMMON_H
#define C5CAFECOMMON_H

#include <QString>
#include <QPixmap>
#include <QJsonArray>
#include <QJsonObject>
#include <QMap>

typedef struct {
    int id;
    QString name;
    QPixmap image;
} CreditCards;

class C5CafeCommon : public QObject
{
    Q_OBJECT

public:
    C5CafeCommon();

    static QList<CreditCards> fCreditCards;

    static QString creditCardName(int id);

    static QJsonObject table(const QString &id);

    static QJsonObject table(int id);

    static QJsonObject hall(const QString &id);

    static QStringList fDishRemoveReason;

    static QStringList fDishComments;

    static QJsonArray fHalls;

    static QJsonArray fTables;

    static QMap<int, QMap<int, QString> > fHallConfigs;
};

#endif // C5CAFECOMMON_H
