/********************************************************************************
** Form generated from reading UI file 'dlgreturnitem.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGRETURNITEM_H
#define UI_DLGRETURNITEM_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <c5cleartablewidget.h>

QT_BEGIN_NAMESPACE

class Ui_DlgReturnItem
{
public:
    QGridLayout *gridLayout;
    QLabel *label_6;
    C5ClearTableWidget *tblBody;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_4;
    QLineEdit *leTaxReceipt;
    QLabel *label_5;
    QPushButton *btnReturnCash;
    QLabel *label_3;
    QLineEdit *leReceiptNumber;
    QLabel *label;
    QLabel *label_7;
    QPushButton *btnReturnCard;
    QComboBox *cbReason;
    QSpacerItem *horizontalSpacer;
    QLabel *label_2;
    QToolButton *btnSearchReceiptNumber;
    QToolButton *btnSearchTax;
    C5ClearTableWidget *tblOrder;
    QPushButton *btnReturn;

    void setupUi(QDialog *DlgReturnItem)
    {
        if (DlgReturnItem->objectName().isEmpty())
            DlgReturnItem->setObjectName("DlgReturnItem");
        DlgReturnItem->resize(905, 588);
        gridLayout = new QGridLayout(DlgReturnItem);
        gridLayout->setObjectName("gridLayout");
        label_6 = new QLabel(DlgReturnItem);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 3, 1, 1, 1);

        tblBody = new C5ClearTableWidget(DlgReturnItem);
        if (tblBody->columnCount() < 10)
            tblBody->setColumnCount(10);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tblBody->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tblBody->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tblBody->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tblBody->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tblBody->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tblBody->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tblBody->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tblBody->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tblBody->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tblBody->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        tblBody->setObjectName("tblBody");
        tblBody->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblBody->setSelectionMode(QAbstractItemView::NoSelection);
        tblBody->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout->addWidget(tblBody, 6, 0, 1, 6);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 1, 3, 1, 1);

        label_4 = new QLabel(DlgReturnItem);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        leTaxReceipt = new QLineEdit(DlgReturnItem);
        leTaxReceipt->setObjectName("leTaxReceipt");

        gridLayout->addWidget(leTaxReceipt, 1, 1, 1, 1);

        label_5 = new QLabel(DlgReturnItem);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 0, 4, 1, 1);

        btnReturnCash = new QPushButton(DlgReturnItem);
        btnReturnCash->setObjectName("btnReturnCash");
        btnReturnCash->setCheckable(true);
        btnReturnCash->setChecked(true);

        gridLayout->addWidget(btnReturnCash, 1, 5, 1, 1);

        label_3 = new QLabel(DlgReturnItem);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 5, 0, 1, 1);

        leReceiptNumber = new QLineEdit(DlgReturnItem);
        leReceiptNumber->setObjectName("leReceiptNumber");

        gridLayout->addWidget(leReceiptNumber, 0, 1, 1, 1);

        label = new QLabel(DlgReturnItem);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_7 = new QLabel(DlgReturnItem);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 1, 4, 1, 1);

        btnReturnCard = new QPushButton(DlgReturnItem);
        btnReturnCard->setObjectName("btnReturnCard");
        btnReturnCard->setCheckable(true);

        gridLayout->addWidget(btnReturnCard, 1, 6, 1, 1);

        cbReason = new QComboBox(DlgReturnItem);
        cbReason->setObjectName("cbReason");
        cbReason->setMinimumSize(QSize(150, 0));

        gridLayout->addWidget(cbReason, 0, 5, 1, 2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 3, 1, 1);

        label_2 = new QLabel(DlgReturnItem);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        btnSearchReceiptNumber = new QToolButton(DlgReturnItem);
        btnSearchReceiptNumber->setObjectName("btnSearchReceiptNumber");
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btnSearchReceiptNumber->sizePolicy().hasHeightForWidth());
        btnSearchReceiptNumber->setSizePolicy(sizePolicy);
        btnSearchReceiptNumber->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/zoom-in.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnSearchReceiptNumber->setIcon(icon);

        gridLayout->addWidget(btnSearchReceiptNumber, 0, 2, 1, 1);

        btnSearchTax = new QToolButton(DlgReturnItem);
        btnSearchTax->setObjectName("btnSearchTax");
        sizePolicy.setHeightForWidth(btnSearchTax->sizePolicy().hasHeightForWidth());
        btnSearchTax->setSizePolicy(sizePolicy);
        btnSearchTax->setFocusPolicy(Qt::NoFocus);
        btnSearchTax->setIcon(icon);

        gridLayout->addWidget(btnSearchTax, 1, 2, 1, 1);

        tblOrder = new C5ClearTableWidget(DlgReturnItem);
        if (tblOrder->columnCount() < 7)
            tblOrder->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tblOrder->setHorizontalHeaderItem(0, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tblOrder->setHorizontalHeaderItem(1, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tblOrder->setHorizontalHeaderItem(2, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tblOrder->setHorizontalHeaderItem(3, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tblOrder->setHorizontalHeaderItem(4, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tblOrder->setHorizontalHeaderItem(5, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tblOrder->setHorizontalHeaderItem(6, __qtablewidgetitem16);
        tblOrder->setObjectName("tblOrder");
        tblOrder->setMaximumSize(QSize(16777215, 120));
        tblOrder->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblOrder->setSelectionMode(QAbstractItemView::SingleSelection);
        tblOrder->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout->addWidget(tblOrder, 4, 0, 1, 6);

        btnReturn = new QPushButton(DlgReturnItem);
        btnReturn->setObjectName("btnReturn");

        gridLayout->addWidget(btnReturn, 2, 4, 1, 3);

        QWidget::setTabOrder(leReceiptNumber, tblOrder);
        QWidget::setTabOrder(tblOrder, tblBody);
        QWidget::setTabOrder(tblBody, btnSearchReceiptNumber);

        retranslateUi(DlgReturnItem);

        QMetaObject::connectSlotsByName(DlgReturnItem);
    } // setupUi

    void retranslateUi(QDialog *DlgReturnItem)
    {
        DlgReturnItem->setWindowTitle(QCoreApplication::translate("DlgReturnItem", "Return item", nullptr));
        label_6->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = tblBody->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("DlgReturnItem", "Row uuid", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tblBody->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("DlgReturnItem", "X", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tblBody->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("DlgReturnItem", "Scancode", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tblBody->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("DlgReturnItem", "Goods", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tblBody->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("DlgReturnItem", "Qty", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tblBody->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("DlgReturnItem", "Unit", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tblBody->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("DlgReturnItem", "Price", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tblBody->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("DlgReturnItem", "Amount", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tblBody->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("DlgReturnItem", "Goods id", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tblBody->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("DlgReturnItem", "Returned", nullptr));
        label_4->setText(QCoreApplication::translate("DlgReturnItem", "Matching orders", nullptr));
        label_5->setText(QCoreApplication::translate("DlgReturnItem", "Reason", nullptr));
        btnReturnCash->setText(QCoreApplication::translate("DlgReturnItem", "\324\277\325\241\325\266\325\255\325\253\325\257", nullptr));
        label_3->setText(QCoreApplication::translate("DlgReturnItem", "Order contents", nullptr));
        label->setText(QCoreApplication::translate("DlgReturnItem", "Receipt number", nullptr));
        label_7->setText(QCoreApplication::translate("DlgReturnItem", "\324\264\326\200\325\241\325\264\325\241\326\200\325\257\325\262", nullptr));
        btnReturnCard->setText(QCoreApplication::translate("DlgReturnItem", "\324\261\325\266\325\257\325\241\325\266\325\255\325\253\325\257", nullptr));
        label_2->setText(QCoreApplication::translate("DlgReturnItem", "Tax receipt", nullptr));
        btnSearchReceiptNumber->setText(QString());
        btnSearchTax->setText(QCoreApplication::translate("DlgReturnItem", "...", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tblOrder->horizontalHeaderItem(0);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("DlgReturnItem", "Uuid", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tblOrder->horizontalHeaderItem(1);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("DlgReturnItem", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tblOrder->horizontalHeaderItem(2);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("DlgReturnItem", "Receipt", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = tblOrder->horizontalHeaderItem(3);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("DlgReturnItem", "Hall", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = tblOrder->horizontalHeaderItem(4);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("DlgReturnItem", "Tax receipt", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = tblOrder->horizontalHeaderItem(5);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("DlgReturnItem", "Saler", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = tblOrder->horizontalHeaderItem(6);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("DlgReturnItem", "Saler id", nullptr));
        btnReturn->setText(QCoreApplication::translate("DlgReturnItem", "Return", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgReturnItem: public Ui_DlgReturnItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGRETURNITEM_H
