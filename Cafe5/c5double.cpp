#include "c5double.h"

bool zerodouble(double d)
{
    return d > -0.00001 && d < 0.00001;
}
