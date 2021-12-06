#ifndef CR5DRAFTOUTPUTBYRECIPE_H
#define CR5DRAFTOUTPUTBYRECIPE_H

#include "c5reportwidget.h"

class CR5DraftOutputByRecipeFilter;

class CR5DraftOutputByRecipe : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5DraftOutputByRecipe(const QStringList &dbParams, QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

private:
    CR5DraftOutputByRecipeFilter *fFilter;

private slots:
    void dateBack();

    void dateForward();

    void createStoreOutput();
};

#endif // CR5DRAFTOUTPUTBYRECIPE_H
