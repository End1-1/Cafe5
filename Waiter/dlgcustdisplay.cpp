#include "dlgcustdisplay.h"
#include "ui_dlgcustdisplay.h"

#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QScreen>
#include <QScrollBar>
#include <QSpacerItem>
#include <QTimer>

namespace {

QLabel *makeCellLabel(const QString &text, int minWidth, Qt::Alignment align, QWidget *parent)
{
    auto *lb = new QLabel(text, parent);
    lb->setMinimumWidth(minWidth);
    lb->setAlignment(align);
    lb->setStyleSheet(QStringLiteral("color: #f5f5f5; font-size: 22pt; padding: 4px 0;"));
    lb->setWordWrap(false);
    return lb;
}

}

DlgCustDisplay::DlgCustDisplay(QWidget *parent)
    : QWidget(parent, Qt::Window)
    , ui(new Ui::DlgCustDisplay)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint, true);
    ui->scrollDishes->setStyleSheet(QStringLiteral(
        "QScrollArea { background-color: #1a1a2e; border: none; }"));
    ui->scrollDishes->viewport()->setStyleSheet(QStringLiteral("background-color: #1a1a2e;"));
    ui->scrollDishesContents->setStyleSheet(QStringLiteral("background-color: #1a1a2e;"));
    ui->horizontalLayout_header->setStretch(0, 1);
}

DlgCustDisplay::~DlgCustDisplay()
{
    delete ui;
}

QWidget *DlgCustDisplay::createDishRowWidget(const CustDisplayLine &line)
{
    auto *row = new QWidget(ui->scrollDishesContents);
    auto *hl = new QHBoxLayout(row);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(8);

    auto *nameLb = makeCellLabel(line.name, 0, Qt::AlignLeft | Qt::AlignVCenter, row);
    nameLb->setWordWrap(true);
    nameLb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hl->addWidget(nameLb, 1);
    hl->addWidget(makeCellLabel(line.qtyText, 90, Qt::AlignRight | Qt::AlignVCenter, row));
    hl->addWidget(makeCellLabel(line.priceText, 120, Qt::AlignRight | Qt::AlignVCenter, row));
    hl->addWidget(makeCellLabel(line.amountText, 140, Qt::AlignRight | Qt::AlignVCenter, row));

    return row;
}

void DlgCustDisplay::setContent(const QList<CustDisplayLine> &lines, const CustDisplayTotals &totals)
{
    QLayout *lay = ui->verticalLayout_dishes;
    while(QLayoutItem *item = lay->takeAt(0)) {
        if(QWidget *w = item->widget()) {
            w->deleteLater();
        }

        delete item;
    }

    for(const CustDisplayLine &line : lines) {
        if(line.name.trimmed().isEmpty()) {
            continue;
        }

        lay->addWidget(createDishRowWidget(line));
    }

    lay->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
    ui->scrollDishesContents->adjustSize();

    ui->lbSubtotalAmount->setText(totals.subtotalText);

    ui->lbServiceCaption->setText(totals.serviceCaption);
    ui->lbServiceValue->setText(totals.serviceText);
    ui->lbServiceCaption->setVisible(totals.showService);
    ui->lbServiceValue->setVisible(totals.showService);

    ui->lbDiscountCaption->setText(totals.discountCaption);
    ui->lbDiscountValue->setText(totals.discountText);
    ui->lbDiscountCaption->setVisible(totals.showDiscount);
    ui->lbDiscountValue->setVisible(totals.showDiscount);

    ui->lbTotalAmount->setText(totals.totalDueText);

    QTimer::singleShot(0, this, [this]() {
        if(QScrollBar *sb = ui->scrollDishes->verticalScrollBar()) {
            sb->setValue(sb->maximum());
        }
    });
}

void DlgCustDisplay::showOnSecondScreen()
{
    const QList<QScreen *> screens = QGuiApplication::screens();

    if(screens.size() > 1) {
        setGeometry(screens.at(1)->availableGeometry());
    } else if(!screens.isEmpty()) {
        setGeometry(screens.first()->availableGeometry());
    }

    showFullScreen();
    raise();
    activateWindow();
}
