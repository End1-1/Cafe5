#ifndef C5PRINTRECEIPTTHREAD_H
#define C5PRINTRECEIPTTHREAD_H

#include <QMap>
#include <QObject>
#include <QVariant>

class C5PrintReceiptThread
{

public:
    C5PrintReceiptThread(const QString &header, const QMap<QString, QVariant> &headerInfo, const QList<QMap<QString, QVariant> > &bodyinfo, const QString &printer, int language, int paperWidth);

    bool fBill;

    bool print(const QStringList &dbParams);

    QString fError;

    QMap<int, QString> fIdram;

private:
    QMap<QString, QVariant> fHeaderInfo;

    QList<QMap<QString, QVariant> > fBodyInfo;

    QString fHeader;

    QString fPrinter;

    int fPaperWidth;

    int fLanguage;


};

#endif // C5PRINTRECEIPTTHREAD_H
