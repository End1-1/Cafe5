#include "c5uilanguage.h"

#include "c5registrysettings.h"

#include <QApplication>
#include <QCoreApplication>
#include <QEvent>
#include <QSettings>
#include <QTranslator>
#include <QWidget>

namespace {

QString &amQmPath()
{
    static QString path;
    return path;
}

QString &ruQmPath()
{
    static QString path;
    return path;
}

QTranslator *appTranslator()
{
    static QTranslator *t = new QTranslator(qApp);
    return t;
}

QString normalizeLang(const QString &lang)
{
    return lang == QLatin1String(C5UiLanguage::kRu) ? QLatin1String(C5UiLanguage::kRu)
                                                    : QLatin1String(C5UiLanguage::kAm);
}

} // namespace

void C5UiLanguage::configure(const QString &amQm, const QString &ruQm)
{
    amQmPath() = amQm;
    ruQmPath() = ruQm;
}

QString C5UiLanguage::current()
{
    QSettings s(_ORGANIZATION_, C5RegistrySettings::registryPath());
    return normalizeLang(s.value(QStringLiteral("ui_language"), QLatin1String(kAm)).toString());
}

void C5UiLanguage::loadSaved()
{
    apply(current());
}

void C5UiLanguage::apply(const QString &lang)
{
    const QString code = normalizeLang(lang);
    qApp->removeTranslator(appTranslator());

    const QString qm = (code == QLatin1String(kRu)) ? ruQmPath() : amQmPath();
    if(!qm.isEmpty() && appTranslator()->load(qm)) {
        qApp->installTranslator(appTranslator());
    }

    QSettings s(_ORGANIZATION_, C5RegistrySettings::registryPath());
    s.setValue(QStringLiteral("ui_language"), code);

    const QList<QWidget *> widgets = qApp->topLevelWidgets();
    for(QWidget *widget : widgets) {
        QEvent languageEvent(QEvent::LanguageChange);
        QCoreApplication::sendEvent(widget, &languageEvent);
    }
}

QString C5UiLanguage::flagIcon(const QString &lang)
{
    if(normalizeLang(lang) == QLatin1String(kRu)) {
        return QStringLiteral(":/russia.png");
    }
    return QStringLiteral(":/armenia.png");
}

QString C5UiLanguage::displayName(const QString &lang)
{
    if(normalizeLang(lang) == QLatin1String(kRu)) {
        return QStringLiteral("Русский");
    }
    return QStringLiteral("Հայերեն");
}
