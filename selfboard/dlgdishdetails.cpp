#include "dlgdishdetails.h"

#include "ui_dlgdishdetails.h"

#include <QPushButton>
#include <QStringList>

DlgDishDetails::DlgDishDetails(const MenuDish &dish, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgDishDetails)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    ui->lblTitle->setText(dish.name);

    QStringList lines;

    if (!dish.attrType.isEmpty()) {
        lines << tr("Type: %1").arg(dish.attrType);
    }
    if (!dish.attrSize.isEmpty()) {
        QString sizeText = dish.attrSize;
        if (!dish.attrMeasurement.isEmpty()) {
            sizeText += QLatin1Char(' ') + dish.attrMeasurement;
        }
        lines << tr("Size: %1").arg(sizeText);
    }

    const bool hasBju = dish.kcal > 0.0 || dish.protein > 0.0 || dish.fat > 0.0 || dish.carbs > 0.0;
    if (hasBju) {
        lines << tr("Per 100 g: %1 kcal · P %2 g · F %3 g · C %4 g")
                     .arg(QString::number(dish.kcal, 'f', dish.kcal >= 100 ? 0 : 1))
                     .arg(QString::number(dish.protein, 'f', 1))
                     .arg(QString::number(dish.fat, 'f', 1))
                     .arg(QString::number(dish.carbs, 'f', 1));
    }

    if (lines.isEmpty()) {
        ui->lblHint->setText(tr("Detailed description will be added later."));
    } else {
        ui->lblHint->setText(lines.join(QLatin1Char('\n')));
    }

    connect(ui->btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

DlgDishDetails::~DlgDishDetails()
{
    delete ui;
}
