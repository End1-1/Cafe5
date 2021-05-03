#ifndef C5PRINTRECEIPTTHREAD50MM_H
#define C5PRINTRECEIPTTHREAD50MM_H

#include <QThread>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

class C5PrintReceiptThread50mm : public QThread
{
    Q_OBJECT

public:
    C5PrintReceiptThread50mm(const QStringList &dbParams, const QJsonObject &header, const QJsonArray &body,
                             const QString &printer,  int paperWidth, QObject *parent = nullptr);

    bool fBill;

protected:
    virtual void run();

private:
    QJsonObject fHeader;

    int fPaperWidth;

    QJsonArray fBody;

    QString fPrinter;

    QStringList fDbParams;

private slots:
    void print();

signals:
    void startPrint();
};

#endif // C5PRINTRECEIPTTHREAD50MM_H
