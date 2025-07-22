#ifndef C5TRANSLATOR_H
#define C5TRANSLATOR_H

#include <QMap>
#include <QObject>

static const int LANG_AM = 0;
static const int LANG_EN = 1;
static const int LANG_RU = 2;

#define ntr_am 0
#define ntr_ru 1
#define ntr_en 2
#define ntr(x, a) C5Translator::translate(x, a)
#define tr_am(x) ntr(x, ntr_am)

class C5Translator : public QObject
{
public:
    C5Translator();

    static void initTranslator();

    void setLanguage(int language);

    static QString tt(const QString &value);

    static QString td(int id);

    static QString translate(const QString &str, int lang);

private:

    int fLanguage;

    QMap<QString, QString> fAm;

    QMap<QString, QString> fEn;

    QMap<QString, QString> fRu;

    QMap<int, QString> fAmDish;

    QMap<int, QString> fEnDish;

    QMap<int, QString> fRuDish;

    QString AM(const QString &value);

    QString EN(const QString &value);

    QString RU(const QString &value);

    void insertUnknown(const QString &value);

    QMap<QString, QMap<QString, QString> > fTranslations;

    static C5Translator* fInstance;

};

#endif // C5TRANSLATOR_H
