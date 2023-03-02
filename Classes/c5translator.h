#ifndef C5TRANSLATOR_H
#define C5TRANSLATOR_H

#include "c5database.h"

static const int LANG_AM = 0;
static const int LANG_EN = 1;
static const int LANG_RU = 2;

#define ntr_am "am"
#define ntr_ru "ru"
#define ntr_en "en"
#define ntr(x, a) C5Translator::translate(x, a)
#define tr_am(x) ntr(x, ntr_am)

class C5Translator : public QObject
{
public:
    C5Translator();

    void initTranslator(const QStringList &dbParams);

    void setLanguage(int language);

    QString tt(const QString &value);

    QString td(int id) const;

    static QString translate(const QString &str, const QString &lang = "am");

private:

    QStringList fDbParams;

    int fLanguage;

    QString AM(const QString &value);

    QString EN(const QString &value);

    QString RU(const QString &value);

    void insertUnknown(const QString &value);

    QMap<QString, QMap<QString, QString> > fTranslations;

    static C5Translator *fInstance;

};

#endif // C5TRANSLATOR_H
