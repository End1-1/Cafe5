#include "c5translator.h"

static C5Translator __tr;
static QMap<QString, QString> fEn;
static QMap<QString, QString> fRu;
static QMap<int, QString> fAmDish;
static QMap<int, QString> fEnDish;
static QMap<int, QString> fRuDish;

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
    db.exec("select d.f_id, d.f_name, t.f_en, t.f_ru "
            "from d_dish d "
            "left join d_translator t on d.f_id=t.f_id ");
    while (db.nextRow()) {
        fAmDish.insert(db.getInt("f_id"), db.getString("f_name"));
        fEnDish.insert(db.getInt("f_id"), db.getString("f_en"));
        fRuDish.insert(db.getInt("f_id"), db.getString("f_ru"));
    }
}

void C5Translator::setLanguage(int language)
{
    fLanguage = language;
}

QString C5Translator::tt(const QString &value)
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

QString C5Translator::td(int id) const
{
    switch (fLanguage) {
    case LANG_AM:
        return fAmDish[id];
    case LANG_EN:
        if (fEnDish.contains(id)) {
            if (fEnDish[id].isEmpty()) {
                return fAmDish[id];
            } else {
                return fEnDish[id];
            }
        } else {
            return fAmDish[id];
        }
    case LANG_RU:
        if (fRuDish.contains(id)) {
            if (fRuDish[id].isEmpty()) {
                return fAmDish[id];
            } else {
                return fRuDish[id];
            }
        } else {
            return fAmDish[id];
        }
    }
    return "DISH TRANSLATE ERROR";
}

QString C5Translator::EN(const QString &value)
{
    if (!fEn.contains(value)) {
        insertUnknown(value);
        return "UNKNOWN_EN " + value;
    }
    if (fEn[value].isEmpty()) {
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
