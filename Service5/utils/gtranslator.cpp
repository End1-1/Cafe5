#include "gtranslator.h"
#include <QFile>

GTranslator *GTranslator::fInstance = nullptr;

GTranslator::GTranslator()
{
    QFile f(":/gr.txt");
    if (f.open(QIODevice::ReadOnly)) {
        while (f.canReadLine()) {
            QString line = f.readLine();
            QStringList lines = line.split("=", Qt::SkipEmptyParts);
            if (lines.count() == 2) {
                fTranslations[lines.at(0).toLower()] = lines.at(1);
            }
        }
        f.close();
    }
    fInstance = this;
}

QString GTranslator::translate(const QString &key) const
{
    if (fTranslations.contains(key.toLower())) {
        return fTranslations[key.toLower()];
    } else {
        return "translate_me_" + key;
    }
}
