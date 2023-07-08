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

    bool event(QEvent *e) override;

private slots:
    void on_btnAddRecipe_clicked();

    void on_btnRemoveRecipe_clicked();

    void setColor();

    void uploadImage();

    void removeImage();

    void recipeHeaderResized(int section, int oldsize, int newsize);

    void tableRecipeScroll(int value);

    void on_btnNewType_clicked();

    void recipeQtyPriceChanged(const QString &arg);

    void on_btnPrintRecipe_clicked();

    void on_btnNewGoods_clicked();

    void on_btnRemoveComment_clicked();

    void on_btnAddComment_clicked();

    void on_leDishComment_returnPressed();

    void on_btnCopy_clicked();

    void on_btnPasteRecipt_clicked();

    void on_leName_textChanged(const QString &arg1);

    void on_lbImage_customContextMenuRequested(const QPoint &pos);

    void on_tabWidget_currentChanged(int index);

    void on_chSameAsInStore_clicked(bool checked);

    void on_btnAddDishRecipe_clicked();

private:
    Ui::C5DishWidget *ui;

    void countTotalSelfCost();

    int addRecipeRow();

    void loadImage();
};

#endif // C5DISHWIDGET_H
