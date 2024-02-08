#ifndef CR5DRAFTOUTPUTBYRECIPEFILTER_H
#define CR5DRAFTOUTPUTBYRECIPEFILTER_H

#include "c5filterwidget.h"

namespace Ui {
class CR5DraftOutputByRecipeFilter;
}

class CR5DraftOutputByRecipeFilter : public C5FilterWidget
{
    Q_OBJECT

public:
    explicit CR5DraftOutputByRecipeFilter(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CR5DraftOutputByRecipeFilter();

    virtual QString condition();

    void addDays(int day);

    QDate date1();

    QString d1();

    QString d2();

    QDate date2();

    int store();

private:
    Ui::CR5DraftOutputByRecipeFilter *ui;
};

#endif // CR5DRAFTOUTPUTBYRECIPEFILTER_H
