#ifndef CE5DISHPACKAGE_H
#define CE5DISHPACKAGE_H

#include "ce5editor.h"

namespace Ui {
class CE5DishPackage;
}

class CE5DishPackage : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5DishPackage(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5DishPackage();

    virtual QString title() override;

    virtual QString table() override;

    virtual void setId(int id) override;

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data) override;

    virtual void clear() override;

private slots:
    void newDish();

    void removeDish();

    void setPrice(const QString &arg);

    void on_tblDishes_customContextMenuRequested(const QPoint &pos);

private:
    Ui::CE5DishPackage *ui;

    QList<int> fRemovedDish;
};

#endif // CE5DISHPACKAGE_H
