#ifndef C5DISHWIDGET_H
#define C5DISHWIDGET_H

#include "ce5editor.h"

namespace Ui {
class C5DishWidget;
}

class C5DishWidget : public CE5Editor
{
    Q_OBJECT

public:
    explicit C5DishWidget(const QStringList &dbParams, QWidget *parent = 0);

    ~C5DishWidget();

    virtual QString title();

    virtual QString table();

    virtual void setId(int id);

    virtual void clear();

    void setDish(int id);

    virtual void selectorCallback(int row, const QList<QVariant> &values);

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data);

private slots:
    void on_btnAddRecipe_clicked();

    void on_btnRemoveRecipe_clicked();

    void setColor();

    void on_btnNewType_clicked();

    void recipeQtyPriceChanged(const QString &arg);

    void on_btnPrintRecipe_clicked();

private:
    Ui::C5DishWidget *ui;

    void countTotalSelfCost();
};

#endif // C5DISHWIDGET_H
