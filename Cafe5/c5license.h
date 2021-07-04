#ifndef C5LICENSE_H
#define C5LICENSE_H

#include <QString>

class C5License
{
public:
    C5License();
    
    static bool isOK(QString &err);
};

#endif // C5LICENSE_H
