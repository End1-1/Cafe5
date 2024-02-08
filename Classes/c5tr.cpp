#include "c5tr.h"

C5Tr::C5Tr()
{

}

void C5Tr::setLanguage(const QString &lang)
{
    fLanguage = lang;
}

void C5Tr::add(const QString &en, const QString &am, const QString &ru)
{
    fAm[en.toLower()] = am;
    fRu[en.toLower()] = ru;
}

QString C5Tr::tt(const QString &key)
{
    if (fLanguage == "am") {
        if (!fAm.contains(key.toLower())) {
            return key;
        }
        return fAm[key.toLower()];
    } else if (fLanguage == "ru") {
        if (!fRu.contains(key.toLower())) {
            return key;
        }
        return fRu[key.toLower()];
    } else {
        return key;
    }
}
