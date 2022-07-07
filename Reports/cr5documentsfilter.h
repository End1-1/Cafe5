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
    explicit CR5DocumentsFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5DocumentsFilter();

    virtual QString condition();

    virtual QString filterText() override;

    QDate date1() const;

    QDate date2() const;

private:
    Ui::CR5DocumentsFilter *ui;
};

#endif // CR5DOCUMENTSFILTER_H
