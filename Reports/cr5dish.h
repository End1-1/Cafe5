#ifndef CR5DISH_H
#define CR5DISH_H

#include "c5reportwidget.h"

class C5DishWidget;

class CR5Dish : public C5ReportWidget
{
    Q_OBJECT

public:
    CR5Dish(QWidget *parent = nullptr);

    virtual QToolBar *toolBar();

public slots:
    virtual bool on_tblView_doubleClicked(const QModelIndex &index);

protected:
    virtual void buildQuery();

    virtual void refreshData();

private:
    C5DishWidget *fDishWidget;

    void setColors();

private slots:
    void translator();

    void descriptionTranslator();

    void deleteDish();

    void printRecipes(bool v);

    void buildWeb();

    void buildWebResponse(const QJsonObject &obj);
};

#endif // CR5DISH_H
