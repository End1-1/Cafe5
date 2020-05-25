#include "c5random.h"
#include <time.h>
#include <QDateTime>

C5Random::C5Random()
{

}

QString C5Random::randomStr(int min, int max)
{
    return QString::number(randomInt(min, max));
}

int C5Random::randomInt(int min, int max)
{
    int range = max - min + 1;
    int num = rand() % range + min;
    return num;
}
