#include "c5translator.h"
#include <QFile>

static C5Translator __tr;

C5Translator *C5Translator::fInstance = nullptr;

C5Translator::C5Translator()
{
    fLanguage = LANG_AM;
}

QString C5Translator::translate(const QString &str, int lang)
{
    if (fInstance == nullptr) {
        fInstance = new C5Translator();
    }
    fInstance->setLanguage(lang);
    return fInstance->tt(str);
    return str;
}

void C5Translator::initTranslator(const QStringList &dbParams)
{
    if (fInstance == nullptr) {
        fInstance = new C5Translator();
        fInstance->fDbParams = dbParams;
        fInstance->fAm.clear();
        fInstance->fEn.clear();
        fInstance->fRu.clear();
        C5Database db(dbParams);
        db.exec("select * from s_translator");
        while (db.nextRow()) {
            fInstance->fEn[db.getString("f_en").toLower()] = db.getString("f_en");
            fInstance->fRu[db.getString("f_en").toLower()] = db.getString("f_ru");
            fInstance->fAm[db.getString("f_en").toLower()] = db.getString("f_am");
        }
        db.exec("select d.f_id, d.f_name, t.f_en, t.f_ru "
                "from d_dish d "
                "left join d_translator t on d.f_id=t.f_id ");
        while (db.nextRow()) {
            fInstance->fAmDish.insert(db.getInt("f_id"), db.getString("f_name"));
            fInstance->fEnDish.insert(db.getInt("f_id"), db.getString("f_en"));
            fInstance->fRuDish.insert(db.getInt("f_id"), db.getString("f_ru"));
        }
    }

}

void C5Translator::setLanguage(int language)
{
    fLanguage = language;
}

QString C5Translator::tt(const QString &value)
{
    switch (fInstance->fLanguage) {
    case LANG_AM:
        return fInstance->AM(value);
    case LANG_EN:
        return fInstance->EN(value);
    case LANG_RU:
        return fInstance->RU(value);
    default:
        return "UNKNOWN LANGUAGE " + QString("%1").arg(fInstance->fLanguage);
    }
}

QString C5Translator::td(int id)
{
    switch (fInstance->fLanguage) {
    case LANG_AM:
        return fInstance->fAmDish[id];
    case LANG_EN:
        if (fInstance->fEnDish.contains(id)) {
            if (fInstance->fEnDish[id].isEmpty()) {
                return fInstance->fAmDish[id];
            } else {
                return fInstance->fEnDish[id];
            }
        } else {
            return fInstance->fAmDish[id];
        }
    case LANG_RU:
        if (fInstance->fRuDish.contains(id)) {
            if (fInstance->fRuDish[id].isEmpty()) {
                return fInstance->fAmDish[id];
            } else {
                return fInstance->fRuDish[id];
            }
        } else {
            return fInstance->fAmDish[id];
        }
    }
    return "DISH TRANSLATE ERROR";
}

QString C5Translator::AM(const QString &value)
{
    if (!fAm.contains(value.toLower())) {
        insertUnknown(value);
       return "UNKNOWN_AM " + value;
    }
    if (fAm[value.toLower()].isEmpty()) {
        return "empty_am " + value;
    }
    return fAm[value.toLower()];
}

QString C5Translator::EN(const QString &value)
{
    if (!fEn.contains(value.toLower())) {
        insertUnknown(value);
        return "UNKNOWN_EN " + value;
    }
    if (fEn[value.toLower()].isEmpty()) {
        return "empty_en " + value;
    }
    return fEn[value.toLower()];
}

QString C5Translator::RU(const QString &value)
{
    if (!fRu.contains(value.toLower())) {
        insertUnknown(value);
        return "UNKNOWN_RU " + value;
    }
    if (fRu[value.toLower()].isEmpty()) {
        return "empty_ru " + value;
    }
    return fRu[value.toLower()];
}

void C5Translator::insertUnknown(const QString &value)
{
    C5Database db(fDbParams);
    db[":f_en"] = value;
    db.insert("s_translator", false);
}
