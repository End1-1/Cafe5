#ifndef OUTPUTOFHEADER_H
#define OUTPUTOFHEADER_H

#include <QObject>
#include "c5database.h"

class OutputOfHeader : public QObject
{
    Q_OBJECT
public:
    explicit OutputOfHeader(QObject *parent = nullptr);

    bool make(C5Database &db, const QString &id);

signals:

};

#endif // OUTPUTOFHEADER_H
