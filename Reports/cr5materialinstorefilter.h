#ifndef CR5MATERIALINSTOREFILTER_H
#define CR5MATERIALINSTOREFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5MaterialInStoreFilter;
}

class CR5MaterialInStoreFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5MaterialInStoreFilter(const QStringList &dbParams, QWidget *parent = 0);

    ~CR5MaterialInStoreFilter();

    virtual QString condition();

private:
    Ui::CR5MaterialInStoreFilter *ui;
};

#endif // CR5MATERIALINSTOREFILTER_H
