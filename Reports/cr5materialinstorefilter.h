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
    explicit CR5MaterialInStoreFilter(QWidget *parent = nullptr);

    ~CR5MaterialInStoreFilter();

    virtual QString condition();

    virtual QString filterText() override;

    bool showDrafts();

    bool showZero();

    QString unit();

    QString currency() const;

    QString currencyName() const;

private:
    Ui::CR5MaterialInStoreFilter *ui;
};

#endif // CR5MATERIALINSTOREFILTER_H
