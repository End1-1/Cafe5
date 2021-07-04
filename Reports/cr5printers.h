#ifndef CR5PRINTERS_H
#define CR5PRINTERS_H

#include <c5reportwidget.h>

class CR5Printers : public C5ReportWidget
{
public:
    CR5Printers(const QStringList &dbParams, QWidget *parent = nullptr);
};

#endif // CR5PRINTERS_H
