/********************************************************************************
** Form generated from reading UI file 'viewinputitem.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWINPUTITEM_H
#define UI_VIEWINPUTITEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <c5cleartablewidget.h>

QT_BEGIN_NAMESPACE

class Ui_ViewInputItem
{
public:
    QHBoxLayout *horizontalLayout;
    C5ClearTableWidget *tbl;

    void setupUi(QDialog *ViewInputItem)
    {
        if (ViewInputItem->objectName().isEmpty())
            ViewInputItem->setObjectName("ViewInputItem");
        ViewInputItem->resize(824, 506);
        horizontalLayout = new QHBoxLayout(ViewInputItem);
        horizontalLayout->setObjectName("horizontalLayout");
        tbl = new C5ClearTableWidget(ViewInputItem);
        if (tbl->columnCount() < 5)
            tbl->setColumnCount(5);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        tbl->setObjectName("tbl");

        horizontalLayout->addWidget(tbl);


        retranslateUi(ViewInputItem);

        QMetaObject::connectSlotsByName(ViewInputItem);
    } // setupUi

    void retranslateUi(QDialog *ViewInputItem)
    {
        ViewInputItem->setWindowTitle(QCoreApplication::translate("ViewInputItem", "Input item", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("ViewInputItem", "UUID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("ViewInputItem", "Goods", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("ViewInputItem", "Scancode", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("ViewInputItem", "Qty", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("ViewInputItem", "Retail", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ViewInputItem: public Ui_ViewInputItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWINPUTITEM_H
