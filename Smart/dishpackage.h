#ifndef DISHPACKAGE_H
#define DISHPACKAGE_H

#include <QObject>
#include <QHash>
#include <QFontMetrics>
#include <QItemDelegate>

class DishMemberDelegate : public QItemDelegate {
protected:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class DishPackageMember
{
public:
    DishPackageMember();
    DishPackageMember(int package, int dish, const QString &name, double price, const QString &adgcode, int store, const QString &printer);
    int fPackage;
    int fDish;
    QString fAdgCode;
    QString fName;
    double fPrice;
    int fStore;
    QString fPrinter;
};

class DishPackageDriver
{
public:
    DishPackageDriver();
    void addMember(int package, int dish, const QString &name, double price, const QString &adgcode, int store, const QString &printer);
    QHash<int, QList<DishPackageMember> > fPackage;
    int itemHeight(int package);
    static DishPackageDriver fPackageDriver;

private:
};

#endif // DISHPACKAGE_H
