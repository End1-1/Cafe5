#ifndef CR5DOCUMENTSFILTER_H
#define CR5DOCUMENTSFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5DocumentsFilter;
}

class CR5DocumentsFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5DocumentsFilter(const QStringList &dbParams, QWidget *parent = 0);

    ~CR5DocumentsFilter();

    virtual QString condition();

private:
    Ui::CR5DocumentsFilter *ui;
};

#endif // CR5DOCUMENTSFILTER_H