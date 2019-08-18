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
    explicit C5DishWidget(const QStringList &dbParams, QWidget *parent = nullptr);

    ~C5DishWidget();

    virtual QString title();

    virtual QString table();

    virtual void setId(int id);

    virtual void clear();

    void setDish(int id);

    virtual void selectorCallback(int row, const QList<QVariant> &values);

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data);

private slots:
    void complextQtyChanged(const QString &arg);

    void on_btnAddRecipe_clicked();

    void on_btnRemoveRecipe_clicked();

    void setColor();

    void on_btnNewType_clicked();

    void recipeQtyPriceChanged(const QString &arg);

    void on_btnPrintRecipe_clicked();

    void on_btnAddDish_clicked();

    void on_tblComplex_cellClicked(int row, int column);

    void on_btnDeleteDish_clicked();

    void on_btnNewGoods_clicked();

    void on_btnRemoveComment_clicked();

    void on_btnAddComment_clicked();

    void on_leDishComment_returnPressed();

private:
    Ui::C5DishWidget *ui;

    void countTotalSelfCost();
};

#endif // C5DISHWIDGET_H
