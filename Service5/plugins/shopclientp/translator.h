#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>
#include <QString>
#include <QMap>

#define ntr_am "am"
#define ntr_ru "ru"
#define ntr_en "en"
#define ntr(x, a) Translator::translate(x, a)
#define tr_am(x) ntr(x, ntr_am)

class Translator : public QObject
{
    Q_OBJECT
public:
    explicit Translator(QObject *parent = nullptr);

    static QString translate(const QString &str, const QString &lang = "am");

private:
    QMap<QString, QMap<QString, QString> > fTranslations;

    static Translator *fInstance;

signals:

};

#endif // TRANSLATOR_H
