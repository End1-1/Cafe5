#ifndef GTRANSLATOR_H
#define GTRANSLATOR_H

#include <QHash>

#define gr(k) GTranslator::fInstance->translate(k)

class GTranslator
{
public:
    GTranslator(); 
    QString translate(const QString &key) const;

private:
    QHash<QString, QString> fTranslations;
    static GTranslator *fInstance;
};

#endif // GTRANSLATOR_H
