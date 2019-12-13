#ifndef CE5PACKAGELIST_H
#define CE5PACKAGELIST_H

#include "c5dialog.h"

namespace Ui {
class CE5PackageList;
}

class CE5PackageList : public C5Dialog
{
    Q_OBJECT

public:
    explicit CE5PackageList(const QStringList &dbParams, int package, QWidget *parent = nullptr);

    ~CE5PackageList();

private:
    Ui::CE5PackageList *ui;

    void setPackage(int id);

    void addDish(int row, const QString &typeName, int dishCode, const QString &dishName, double price);

    void countPrices();

private slots:
    void itemPriceChanged(const QString &str);

    void packageChanged(const QList<QVariant> &val);

    void on_btnAdd_clicked();

    void on_btnSave_clicked();

    void on_btnRemove_clicked();
};

#endif // CE5PACKAGELIST_H
