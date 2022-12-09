#include "c5translator.h"
#include <QFile>
#include <QDebug>

C5Translator *C5Translator::fInstance = nullptr;

C5Translator::C5Translator(QObject *parent) : QObject(parent)
{
    Q_INIT_RESOURCE(waiterclientp);
    QFile f(":/tr_am.txt");
    if (f.open(QIODevice::ReadOnly)){
        QString src(f.readAll());
        src.replace("\n", "");
        QStringList srclist = src.split("\r");
        for(const QString &s: srclist) {
            QStringList bl = s.split("=");
            if (bl.count() >= 2) {
                fTranslations[ntr_am][bl.at(0).toLower()] = bl.at(1);
            }
        }
    }
}

QString C5Translator::translate(const QString &str, const QString &lang)
{
    if (fInstance == nullptr) {
        fInstance = new C5Translator();
    }
    if (fInstance->fTranslations[lang].contains(str.toLower())) {
        return fInstance->fTranslations[lang][str.toLower()];
    }
    return str;
}
