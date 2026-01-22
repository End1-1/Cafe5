#ifndef C5PRINTJSON_H
#define C5PRINTJSON_H

#include "c5printing.h"
#include <QJsonArray>

class C5PrintJson
{

public:
    C5PrintJson();

    ~C5PrintJson();

    virtual void parse(C5Printing &p, const QJsonArray &cmd);

};

#endif // C5PRINTJSON_H
