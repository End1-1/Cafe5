#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QString>

#define tr(x) Translator::translate(x)

class Translator
{
public:
    explicit Translator();

    static QString translate(const QString &s);

};

#endif // TRANSLATOR_H
