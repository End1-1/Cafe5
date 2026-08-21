#ifndef C5UILANGUAGE_H
#define C5UILANGUAGE_H

#include <QString>

/** UI language switcher (am/ru). Call configure() before loadSaved(). */
class C5UiLanguage
{
public:
    static constexpr const char *kAm = "am";
    static constexpr const char *kRu = "ru";

    /** Armenian .qm and Russian .qm resource paths, e.g. ":/lang/Shop.qm", ":/lang/Shop_ru.qm". */
    static void configure(const QString &amQm, const QString &ruQm);

    static QString current();
    static void loadSaved();
    static void apply(const QString &lang);
    static QString flagIcon(const QString &lang);
    static QString displayName(const QString &lang);
};

#endif // C5UILANGUAGE_H
