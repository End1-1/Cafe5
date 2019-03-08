#ifndef CE5GOODS_H
#define CE5GOODS_H

#include "ce5editor.h"

namespace Ui {
class CE5Goods;
}

class CE5Goods : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Goods(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5Goods();

    virtual QString title();

    virtual QString table();

    virtual void setId(int id);

    virtual bool save(QString &err, QList<QMap<QString, QVariant> > &data);

    virtual void clear();

private slots:
    void on_leAddScanCode_returnPressed();

    void on_btnRemoveScaneCode_clicked();

    void on_btnNewGroup_clicked();

    void on_btnNewUnit_clicked();

private:
    Ui::CE5Goods *ui;
};

#endif // CE5GOODS_H
