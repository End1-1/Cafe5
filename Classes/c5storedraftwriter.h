#ifndef C5STOREDRAFTWRITER_H
#define C5STOREDRAFTWRITER_H

#include "c5database.h"

class C5StoreDraftWriter : QObject
{
    Q_OBJECT

public:
    C5StoreDraftWriter(C5Database &db);

    bool writeFromShopInput(const QDate &date, const QString &doc);

    bool writeFromShopOutput(const QDate &date, const QString &doc);

    QString fErrorMsg;

private:
    C5Database &fDb;
};

#endif // C5STOREDRAFTWRITER_H
