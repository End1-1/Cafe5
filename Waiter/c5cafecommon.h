#ifndef C5CAFECOMMON_H
#define C5CAFECOMMON_H

#include <QString>
#include <QPixmap>
#include <QJsonArray>

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

    static QStringList fDishRemoveReason;

    static QStringList fDishComments;

    static QJsonArray fHalls;

    static QJsonArray fTables;
};

#endif // C5CAFECOMMON_H
