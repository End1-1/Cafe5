#include "dishpackage.h"
#include "c5utils.h"
#include <QPainter>

DishPackageDriver DishPackageDriver::fPackageDriver;

DishPackageMember::DishPackageMember()
{

}

DishPackageMember::DishPackageMember(int package, int dish, const QString &name, double price, const QString &adgcode, int store, const QString &printer)
{
    fPackage = package;
    fDish = dish;
    fName = name;
    fPrice = price;
    fAdgCode = adgcode;
    fStore = store;
    fPrinter = printer;
}

DishPackageDriver::DishPackageDriver()
{

}

void DishPackageDriver::addMember(int package, int dish, const QString &name, double price, const QString &adgcode, int store, const QString &printer)
{
    if (!fPackage.contains(package)) {
        fPackage.insert(package, QList<DishPackageMember>());
    }
    fPackage[package].append(DishPackageMember(package, dish, name, price, adgcode, store, printer));
}

int DishPackageDriver::itemHeight(int package, int width, const QString &text)
{
    if (!fPackage.contains(package)) {
        return 10;
    }
    int count = fPackage[package].count();
    QFontMetrics fm(QFont("Arial", 12));
    int height = fm.boundingRect(QRect(0, 0, width, fm.height()), Qt::TextWordWrap | Qt::TextExpandTabs, text).height();
    return height + (fm.height() * (count + 4));
}

void DishMemberDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    QFont font(painter->font());
    const QList<DishPackageMember> &dl = DishPackageDriver::fPackageDriver.fPackage[index.data(Qt::UserRole).toInt()];
    QString name = "";
    for (const DishPackageMember &d: dl) {
        if (!name.isEmpty()) {
            name += "\r\n";
        }
        name += d.fName;
    }
    QRectF textRect = option.rect;
    textRect.adjust(2, 2, -2, -2);
    QTextOption to;
    to.setWrapMode(QTextOption::WordWrap);
    painter->setPen(QColor::fromRgb(100, 100, 100));
    painter->drawRect(option.rect);
    painter->setPen(Qt::black);
    font.setBold(true);
    font.setPointSize(font.pointSize() + 2);
    painter->setFont(font);
    painter->drawText(textRect, index.data(Qt::DisplayRole).toString(), to);
    QFontMetrics fm(painter->font());
    int height = fm.boundingRect(QRect(0, 0, textRect.width(), fm.height()), Qt::TextWordWrap | Qt::TextExpandTabs, index.data(Qt::DisplayRole).toString()).height();
    textRect.adjust(0, height + 2, 0, 0);
    painter->drawText(textRect, float_str(index.data(Qt::UserRole + 1).toDouble(), 2), to);
    textRect.adjust(0, QFontMetrics(painter->font()).height() + 2, 0, 0);
    font.setPointSize(font.pointSize() - 2);
    font.setBold(false);
    painter->setFont(font);
    painter->drawText(textRect, name);
    painter->restore();
}
