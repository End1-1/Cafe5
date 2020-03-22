#ifndef C5STOREDRAFTWRITER_H
#define C5STOREDRAFTWRITER_H

#include "c5database.h"

class C5StoreDraftWriter : QObject
{
    Q_OBJECT

public:
    C5StoreDraftWriter(C5Database &db);

    QString writeDraft(const QDate &date, int doctype, int store, int reason, const QMap<int, double> &data, const QString &comment);

    bool writeFromShopInput(const QDate &date, const QString &doc);

    bool writeFromShopOutput(int op, const QDate &date, const QString &doc);

    bool rollbackOutput(C5Database &db, const QString &id);

    QString fErrorMsg;

private:
    C5Database &fDb;
};

#endif // C5STOREDRAFTWRITER_H
