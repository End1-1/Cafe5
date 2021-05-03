#ifndef C5TRANSLATOR_H
#define C5TRANSLATOR_H

#include "c5database.h"

static const int LANG_AM = 0;
static const int LANG_EN = 1;
static const int LANG_RU = 2;

class C5Translator
{
public:
    C5Translator();

    void initTranslator(const QStringList &dbParams);

    void setLanguage(int language);

    QString tt(const QString &value);

    QString td(int id) const;

private:

    QStringList fDbParams;

    int fLanguage;

    QString EN(const QString &value);

    QString RU(const QString &value);

    void insertUnknown(const QString &value);
};

#endif // C5TRANSLATOR_H
