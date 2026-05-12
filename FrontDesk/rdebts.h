#ifndef RDEBTS_H
#define RDEBTS_H

#include "rabstracteditorreport.h"

class RDebts : public RAbstractEditorReport
{
public:
    RDebts(const QString &title, QIcon icon, const QString &editorName);
};

#endif // RDEBTS_H
