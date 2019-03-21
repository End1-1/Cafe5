#include "c5translator.h"

C5Translator __tr;
QMap<QString, QString> fEn;
QMap<QString, QString> fRu;
QMap<QString, QString> fEnDish;
QMap<QString, QString> fRuDish;

C5Translator::C5Translator()
{
    fLanguage = LANG_AM;
}

void C5Translator::initTranslator(const QStringList &dbParams)
{
    fDbParams = dbParams;
    fEn.clear();
    fRu.clear();
    C5Database db(dbParams);
    db.exec("select * from s_translator");
    while (db.nextRow()) {
        fEn[db.getString(0)] = db.getString(1);
        fRu[db.getString(0)] = db.getString(2);
    }
    db.exec("select * from d_translator");
    while (db.nextRow()) {

    }
}

void C5Translator::setLanguage(int language)
{
    fLanguage = language;
}

QString C5Translator::tr(const QString &value)
{
    switch (fLanguage) {
    case LANG_AM:
        return value;
    case LANG_EN:
        return EN(value);
    case LANG_RU:
        return RU(value);
    default:
        return "UNKNOWN LANGUAGE " + QString("%1").arg(fLanguage);
    }
}

QString C5Translator::EN(const QString &value)
{
    if (!fEn.contains(value)) {
        insertUnknown(value);
        return "UNKNOWN_EN " + value;
    }
    if (fRu[value].isEmpty()) {
        return "empty_en " + value;
    }
    return fEn[value];
}

QString C5Translator::RU(const QString &value)
{
    if (!fRu.contains(value)) {
        insertUnknown(value);
        return "UNKNOWN_RU " + value;
    }
    if (fRu[value].isEmpty()) {
        return "empty_ru " + value;
    }
    return fRu[value];
}

void C5Translator::insertUnknown(const QString &value)
{
    C5Database db(fDbParams);
    db[":f_text"] = value;
    db.insert("s_translator", false);
}
