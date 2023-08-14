#ifndef GTRANSLATOR_H
#define GTRANSLATOR_H

#include <QHash>

#define gr(k) GTranslator::instance()->translate(k)

class GTranslator
{
public:
    GTranslator(); 
    QString translate(const QString &key) const;
    static GTranslator *instance();

private:
    QHash<QString, QString> fTranslations;
    static GTranslator *fInstance;
};

#endif // GTRANSLATOR_H
