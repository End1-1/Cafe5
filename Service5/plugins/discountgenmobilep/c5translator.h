#ifndef C5TRANSLATOR_H
#define C5TRANSLATOR_H

#include <QObject>
#include <QString>
#include <QMap>

#define ntr_am "am"
#define ntr_ru "ru"
#define ntr_en "en"
#define ntr(x, a) C5Translator::translate(x, a)
#define tr_am(x) ntr(x, ntr_am)

class C5Translator : public QObject
{
    Q_OBJECT
public:
    explicit C5Translator(QObject *parent = nullptr);

    static QString translate(const QString &str, const QString &lang = "am");

private:
    QMap<QString, QMap<QString, QString> > fTranslations;

    static C5Translator *fInstance;

signals:

};

#endif // C5TRANSLATOR_H
