/********************************************************************************
** Form generated from reading UI file 'c5printtaxanywhere.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C5PRINTTAXANYWHERE_H
#define UI_C5PRINTTAXANYWHERE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <c5cleartablewidget.h>

QT_BEGIN_NAMESPACE

class Ui_C5PrintTaxAnywhere
{
public:
    QVBoxLayout *verticalLayout;
    C5ClearTableWidget *tbl;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnPrintTax;
    QLabel *lbProgress;

    void setupUi(QDialog *C5PrintTaxAnywhere)
    {
        if (C5PrintTaxAnywhere->objectName().isEmpty())
            C5PrintTaxAnywhere->setObjectName("C5PrintTaxAnywhere");
        C5PrintTaxAnywhere->resize(670, 393);
        verticalLayout = new QVBoxLayout(C5PrintTaxAnywhere);
        verticalLayout->setObjectName("verticalLayout");
        tbl = new C5ClearTableWidget(C5PrintTaxAnywhere);
        if (tbl->columnCount() < 3)
            tbl->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        tbl->setObjectName("tbl");
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(2);
        sizePolicy.setHeightForWidth(tbl->sizePolicy().hasHeightForWidth());
        tbl->setSizePolicy(sizePolicy);
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout->addWidget(tbl);

        widget = new QWidget(C5PrintTaxAnywhere);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(471, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnPrintTax = new QPushButton(widget);
        btnPrintTax->setObjectName("btnPrintTax");

        horizontalLayout->addWidget(btnPrintTax);

        lbProgress = new QLabel(widget);
        lbProgress->setObjectName("lbProgress");
        lbProgress->setEnabled(true);
        lbProgress->setMinimumSize(QSize(32, 32));
        lbProgress->setMaximumSize(QSize(32, 32));
        lbProgress->setScaledContents(true);

        horizontalLayout->addWidget(lbProgress);


        verticalLayout->addWidget(widget);


        retranslateUi(C5PrintTaxAnywhere);

        QMetaObject::connectSlotsByName(C5PrintTaxAnywhere);
    } // setupUi

    void retranslateUi(QDialog *C5PrintTaxAnywhere)
    {
        C5PrintTaxAnywhere->setWindowTitle(QCoreApplication::translate("C5PrintTaxAnywhere", "Print tax", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("C5PrintTaxAnywhere", "Hall", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("C5PrintTaxAnywhere", "Cashbox", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("C5PrintTaxAnywhere", "Store", nullptr));
        tbl->setProperty("columns", QVariant(QCoreApplication::translate("C5PrintTaxAnywhere", "200, 200,200", nullptr)));
        btnPrintTax->setText(QCoreApplication::translate("C5PrintTaxAnywhere", "Print", nullptr));
        lbProgress->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class C5PrintTaxAnywhere: public Ui_C5PrintTaxAnywhere {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5PRINTTAXANYWHERE_H
