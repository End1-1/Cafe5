#ifndef OUTPUTOFHEADER_H
#define OUTPUTOFHEADER_H

#include <QObject>

class OutputOfHeader : public QObject
{
    Q_OBJECT
public:
    explicit OutputOfHeader(QObject *parent = nullptr);

    bool make(const QString &id);

signals:

};

#endif // OUTPUTOFHEADER_H
