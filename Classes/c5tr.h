#ifndef C5TR_H
#define C5TR_H

#include <QMap>

class C5Tr
{
public:
    C5Tr();
    void setLanguage(const QString &lang);
    void add(const QString &en, const QString &am, const QString &ru);
    QString tt(const QString &key);

private:
    QString fLanguage;
    QMap<QString, QString> fAm;
    QMap<QString, QString> fRu;
};

#endif // C5TR_H
