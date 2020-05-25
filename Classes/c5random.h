#ifndef C5RANDOM_H
#define C5RANDOM_H

#include <QString>

class C5Random
{
public:
    C5Random();

    static int randomInt(int min, int max);

    static QString randomStr(int min, int max);
};

#endif // C5RANDOM_H
