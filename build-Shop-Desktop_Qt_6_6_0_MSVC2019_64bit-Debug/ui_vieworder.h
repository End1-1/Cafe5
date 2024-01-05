/********************************************************************************
** Form generated from reading UI file 'vieworder.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIEWORDER_H
#define UI_VIEWORDER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <c5cleartablewidget.h>
#include <c5dateedit.h>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_ViewOrder
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_2;
    QGridLayout *gridLayout_2;
    QLabel *label_12;
    QPushButton *btnCopyUUID;
    QLabel *label_5;
    QLabel *label_11;
    QLabel *label_4;
    QLabel *label_10;
    QLabel *label_3;
    C5LineEdit *leUUID;
    C5LineEdit *leTime;
    C5DateEdit *leDate;
    QLabel *label_6;
    C5LineEdit *leOrderNum;
    QPushButton *btnEditSaler;
    QPushButton *btnClose;
    QPushButton *btnEditBuyer;
    QPushButton *btnEditDeliveryMan;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *leSaler;
    QLineEdit *leBuyer;
    QLineEdit *leDeliveryMan;
    QPushButton *btnEditDate;
    C5ClearTableWidget *tbl;
    QWidget *widget;
    QGridLayout *gridLayout;
    QLabel *label_8;
    QLabel *label;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnTaxReturn;
    QPushButton *btnPrintFiscal;
    QLabel *label_9;
    QPushButton *btnMakeDraft;
    C5LineEdit *leTaxNumber;
    C5LineEdit *leCard;
    C5LineEdit *leIdram;
    QLabel *label_7;
    QPushButton *btnPrintReceiptA4;
    C5LineEdit *leCash;
    QPushButton *btnPrintReceipt;
    QLabel *label_2;
    C5LineEdit *leAmount;
    QPushButton *btnReturn;
    QLabel *label_14;
    QLabel *label_13;
    C5LineEdit *leDebt;
    C5LineEdit *leBank;
    QLabel *label_15;
    C5LineEdit *leTelcell;

    void setupUi(QDialog *ViewOrder)
    {
        if (ViewOrder->objectName().isEmpty())
            ViewOrder->setObjectName("ViewOrder");
        ViewOrder->resize(976, 636);
        verticalLayout = new QVBoxLayout(ViewOrder);
        verticalLayout->setObjectName("verticalLayout");
        widget_2 = new QWidget(ViewOrder);
        widget_2->setObjectName("widget_2");
        gridLayout_2 = new QGridLayout(widget_2);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        label_12 = new QLabel(widget_2);
        label_12->setObjectName("label_12");

        gridLayout_2->addWidget(label_12, 2, 3, 1, 1);

        btnCopyUUID = new QPushButton(widget_2);
        btnCopyUUID->setObjectName("btnCopyUUID");
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btnCopyUUID->sizePolicy().hasHeightForWidth());
        btnCopyUUID->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/copy.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCopyUUID->setIcon(icon);

        gridLayout_2->addWidget(btnCopyUUID, 0, 2, 1, 1);

        label_5 = new QLabel(widget_2);
        label_5->setObjectName("label_5");

        gridLayout_2->addWidget(label_5, 2, 0, 1, 1);

        label_11 = new QLabel(widget_2);
        label_11->setObjectName("label_11");

        gridLayout_2->addWidget(label_11, 3, 3, 1, 1);

        label_4 = new QLabel(widget_2);
        label_4->setObjectName("label_4");

        gridLayout_2->addWidget(label_4, 0, 3, 1, 1);

        label_10 = new QLabel(widget_2);
        label_10->setObjectName("label_10");

        gridLayout_2->addWidget(label_10, 1, 3, 1, 1);

        label_3 = new QLabel(widget_2);
        label_3->setObjectName("label_3");

        gridLayout_2->addWidget(label_3, 1, 0, 1, 1);

        leUUID = new C5LineEdit(widget_2);
        leUUID->setObjectName("leUUID");
        leUUID->setMinimumSize(QSize(300, 0));
        leUUID->setMaximumSize(QSize(16777215, 16777215));
        leUUID->setReadOnly(true);

        gridLayout_2->addWidget(leUUID, 0, 1, 1, 1);

        leTime = new C5LineEdit(widget_2);
        leTime->setObjectName("leTime");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(leTime->sizePolicy().hasHeightForWidth());
        leTime->setSizePolicy(sizePolicy1);
        leTime->setMaximumSize(QSize(16777215, 16777215));
        leTime->setReadOnly(true);

        gridLayout_2->addWidget(leTime, 2, 1, 1, 1);

        leDate = new C5DateEdit(widget_2);
        leDate->setObjectName("leDate");
        sizePolicy1.setHeightForWidth(leDate->sizePolicy().hasHeightForWidth());
        leDate->setSizePolicy(sizePolicy1);
        leDate->setMaximumSize(QSize(16777215, 16777215));
        leDate->setReadOnly(true);

        gridLayout_2->addWidget(leDate, 1, 1, 1, 1);

        label_6 = new QLabel(widget_2);
        label_6->setObjectName("label_6");

        gridLayout_2->addWidget(label_6, 0, 0, 1, 1);

        leOrderNum = new C5LineEdit(widget_2);
        leOrderNum->setObjectName("leOrderNum");
        leOrderNum->setMinimumSize(QSize(100, 0));
        leOrderNum->setMaximumSize(QSize(150, 16777215));
        leOrderNum->setReadOnly(true);

        gridLayout_2->addWidget(leOrderNum, 0, 4, 1, 1);

        btnEditSaler = new QPushButton(widget_2);
        btnEditSaler->setObjectName("btnEditSaler");
        sizePolicy.setHeightForWidth(btnEditSaler->sizePolicy().hasHeightForWidth());
        btnEditSaler->setSizePolicy(sizePolicy);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/edit.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEditSaler->setIcon(icon1);

        gridLayout_2->addWidget(btnEditSaler, 1, 7, 1, 1);

        btnClose = new QPushButton(widget_2);
        btnClose->setObjectName("btnClose");

        gridLayout_2->addWidget(btnClose, 0, 8, 1, 1);

        btnEditBuyer = new QPushButton(widget_2);
        btnEditBuyer->setObjectName("btnEditBuyer");
        sizePolicy.setHeightForWidth(btnEditBuyer->sizePolicy().hasHeightForWidth());
        btnEditBuyer->setSizePolicy(sizePolicy);
        btnEditBuyer->setIcon(icon1);

        gridLayout_2->addWidget(btnEditBuyer, 2, 7, 1, 1);

        btnEditDeliveryMan = new QPushButton(widget_2);
        btnEditDeliveryMan->setObjectName("btnEditDeliveryMan");
        sizePolicy.setHeightForWidth(btnEditDeliveryMan->sizePolicy().hasHeightForWidth());
        btnEditDeliveryMan->setSizePolicy(sizePolicy);
        btnEditDeliveryMan->setIcon(icon1);

        gridLayout_2->addWidget(btnEditDeliveryMan, 3, 7, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(598, 20, QSizePolicy::Preferred, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 0, 6, 1, 1);

        leSaler = new QLineEdit(widget_2);
        leSaler->setObjectName("leSaler");
        leSaler->setReadOnly(true);

        gridLayout_2->addWidget(leSaler, 1, 4, 1, 3);

        leBuyer = new QLineEdit(widget_2);
        leBuyer->setObjectName("leBuyer");
        leBuyer->setReadOnly(true);

        gridLayout_2->addWidget(leBuyer, 2, 4, 1, 3);

        leDeliveryMan = new QLineEdit(widget_2);
        leDeliveryMan->setObjectName("leDeliveryMan");
        leDeliveryMan->setReadOnly(true);

        gridLayout_2->addWidget(leDeliveryMan, 3, 4, 1, 3);

        btnEditDate = new QPushButton(widget_2);
        btnEditDate->setObjectName("btnEditDate");
        sizePolicy.setHeightForWidth(btnEditDate->sizePolicy().hasHeightForWidth());
        btnEditDate->setSizePolicy(sizePolicy);
        btnEditDate->setIcon(icon1);

        gridLayout_2->addWidget(btnEditDate, 1, 2, 1, 1);


        verticalLayout->addWidget(widget_2);

        tbl = new C5ClearTableWidget(ViewOrder);
        if (tbl->columnCount() < 11)
            tbl->setColumnCount(11);
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
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(10, __qtablewidgetitem10);
        tbl->setObjectName("tbl");
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout->addWidget(tbl);

        widget = new QWidget(ViewOrder);
        widget->setObjectName("widget");
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_8 = new QLabel(widget);
        label_8->setObjectName("label_8");
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_8, 4, 0, 1, 1);

        label = new QLabel(widget);
        label->setObjectName("label");
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label, 1, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(453, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 9, 1, 1);

        btnTaxReturn = new QPushButton(widget);
        btnTaxReturn->setObjectName("btnTaxReturn");

        gridLayout->addWidget(btnTaxReturn, 0, 3, 1, 1);

        btnPrintFiscal = new QPushButton(widget);
        btnPrintFiscal->setObjectName("btnPrintFiscal");

        gridLayout->addWidget(btnPrintFiscal, 0, 4, 1, 1);

        label_9 = new QLabel(widget);
        label_9->setObjectName("label_9");
        sizePolicy2.setHeightForWidth(label_9->sizePolicy().hasHeightForWidth());
        label_9->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_9, 6, 0, 1, 1);

        btnMakeDraft = new QPushButton(widget);
        btnMakeDraft->setObjectName("btnMakeDraft");

        gridLayout->addWidget(btnMakeDraft, 0, 7, 1, 1);

        leTaxNumber = new C5LineEdit(widget);
        leTaxNumber->setObjectName("leTaxNumber");
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(leTaxNumber->sizePolicy().hasHeightForWidth());
        leTaxNumber->setSizePolicy(sizePolicy3);
        leTaxNumber->setMinimumSize(QSize(0, 0));
        leTaxNumber->setMaximumSize(QSize(16777215, 16777215));
        leTaxNumber->setFocusPolicy(Qt::NoFocus);
        leTaxNumber->setReadOnly(true);

        gridLayout->addWidget(leTaxNumber, 0, 1, 1, 1);

        leCard = new C5LineEdit(widget);
        leCard->setObjectName("leCard");
        sizePolicy3.setHeightForWidth(leCard->sizePolicy().hasHeightForWidth());
        leCard->setSizePolicy(sizePolicy3);
        leCard->setMinimumSize(QSize(200, 0));
        leCard->setMaximumSize(QSize(16777215, 16777215));
        leCard->setFocusPolicy(Qt::NoFocus);
        leCard->setReadOnly(true);

        gridLayout->addWidget(leCard, 4, 1, 1, 1);

        leIdram = new C5LineEdit(widget);
        leIdram->setObjectName("leIdram");
        sizePolicy3.setHeightForWidth(leIdram->sizePolicy().hasHeightForWidth());
        leIdram->setSizePolicy(sizePolicy3);
        leIdram->setMinimumSize(QSize(200, 0));
        leIdram->setMaximumSize(QSize(16777215, 16777215));
        leIdram->setFocusPolicy(Qt::NoFocus);
        leIdram->setReadOnly(true);

        gridLayout->addWidget(leIdram, 6, 1, 1, 1);

        label_7 = new QLabel(widget);
        label_7->setObjectName("label_7");
        sizePolicy2.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_7, 2, 0, 1, 1);

        btnPrintReceiptA4 = new QPushButton(widget);
        btnPrintReceiptA4->setObjectName("btnPrintReceiptA4");

        gridLayout->addWidget(btnPrintReceiptA4, 0, 6, 1, 1);

        leCash = new C5LineEdit(widget);
        leCash->setObjectName("leCash");
        sizePolicy3.setHeightForWidth(leCash->sizePolicy().hasHeightForWidth());
        leCash->setSizePolicy(sizePolicy3);
        leCash->setMinimumSize(QSize(200, 0));
        leCash->setMaximumSize(QSize(16777215, 16777215));
        leCash->setFocusPolicy(Qt::NoFocus);
        leCash->setReadOnly(true);

        gridLayout->addWidget(leCash, 2, 1, 1, 1);

        btnPrintReceipt = new QPushButton(widget);
        btnPrintReceipt->setObjectName("btnPrintReceipt");

        gridLayout->addWidget(btnPrintReceipt, 0, 5, 1, 1);

        label_2 = new QLabel(widget);
        label_2->setObjectName("label_2");
        sizePolicy2.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        leAmount = new C5LineEdit(widget);
        leAmount->setObjectName("leAmount");
        sizePolicy3.setHeightForWidth(leAmount->sizePolicy().hasHeightForWidth());
        leAmount->setSizePolicy(sizePolicy3);
        leAmount->setMinimumSize(QSize(200, 0));
        leAmount->setMaximumSize(QSize(16777215, 16777215));
        leAmount->setFocusPolicy(Qt::NoFocus);
        leAmount->setReadOnly(true);

        gridLayout->addWidget(leAmount, 1, 1, 1, 1);

        btnReturn = new QPushButton(widget);
        btnReturn->setObjectName("btnReturn");

        gridLayout->addWidget(btnReturn, 0, 8, 1, 1);

        label_14 = new QLabel(widget);
        label_14->setObjectName("label_14");
        sizePolicy2.setHeightForWidth(label_14->sizePolicy().hasHeightForWidth());
        label_14->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_14, 2, 2, 1, 1);

        label_13 = new QLabel(widget);
        label_13->setObjectName("label_13");
        sizePolicy2.setHeightForWidth(label_13->sizePolicy().hasHeightForWidth());
        label_13->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_13, 4, 2, 1, 1);

        leDebt = new C5LineEdit(widget);
        leDebt->setObjectName("leDebt");
        sizePolicy3.setHeightForWidth(leDebt->sizePolicy().hasHeightForWidth());
        leDebt->setSizePolicy(sizePolicy3);
        leDebt->setMinimumSize(QSize(200, 0));
        leDebt->setMaximumSize(QSize(16777215, 16777215));
        leDebt->setFocusPolicy(Qt::NoFocus);
        leDebt->setReadOnly(true);

        gridLayout->addWidget(leDebt, 2, 3, 1, 1);

        leBank = new C5LineEdit(widget);
        leBank->setObjectName("leBank");
        sizePolicy3.setHeightForWidth(leBank->sizePolicy().hasHeightForWidth());
        leBank->setSizePolicy(sizePolicy3);
        leBank->setMinimumSize(QSize(200, 0));
        leBank->setMaximumSize(QSize(16777215, 16777215));
        leBank->setFocusPolicy(Qt::NoFocus);
        leBank->setReadOnly(true);

        gridLayout->addWidget(leBank, 4, 3, 1, 1);

        label_15 = new QLabel(widget);
        label_15->setObjectName("label_15");
        sizePolicy2.setHeightForWidth(label_15->sizePolicy().hasHeightForWidth());
        label_15->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_15, 6, 2, 1, 1);

        leTelcell = new C5LineEdit(widget);
        leTelcell->setObjectName("leTelcell");
        sizePolicy3.setHeightForWidth(leTelcell->sizePolicy().hasHeightForWidth());
        leTelcell->setSizePolicy(sizePolicy3);
        leTelcell->setMinimumSize(QSize(200, 0));
        leTelcell->setMaximumSize(QSize(16777215, 16777215));
        leTelcell->setFocusPolicy(Qt::NoFocus);
        leTelcell->setReadOnly(true);

        gridLayout->addWidget(leTelcell, 6, 3, 1, 1);


        verticalLayout->addWidget(widget);


        retranslateUi(ViewOrder);

        QMetaObject::connectSlotsByName(ViewOrder);
    } // setupUi

    void retranslateUi(QDialog *ViewOrder)
    {
        ViewOrder->setWindowTitle(QCoreApplication::translate("ViewOrder", "Return", nullptr));
        label_12->setText(QCoreApplication::translate("ViewOrder", "Buyer", nullptr));
        btnCopyUUID->setText(QString());
        label_5->setText(QCoreApplication::translate("ViewOrder", "Time", nullptr));
        label_11->setText(QCoreApplication::translate("ViewOrder", "Delivery man", nullptr));
        label_4->setText(QCoreApplication::translate("ViewOrder", "Order number", nullptr));
        label_10->setText(QCoreApplication::translate("ViewOrder", "Saler", nullptr));
        label_3->setText(QCoreApplication::translate("ViewOrder", "Date", nullptr));
        label_6->setText(QCoreApplication::translate("ViewOrder", "UUID", nullptr));
        btnEditSaler->setText(QString());
        btnClose->setText(QCoreApplication::translate("ViewOrder", "Close", nullptr));
        btnEditBuyer->setText(QString());
        btnEditDeliveryMan->setText(QString());
        btnEditDate->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("ViewOrder", "UUID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("ViewOrder", "X", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("ViewOrder", "Goods", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("ViewOrder", "Qty", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("ViewOrder", "Price", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tbl->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("ViewOrder", "Total", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tbl->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("ViewOrder", "Goods id", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tbl->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("ViewOrder", "Scancode", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tbl->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("ViewOrder", "Service", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tbl->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("ViewOrder", "Returned", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tbl->horizontalHeaderItem(10);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("ViewOrder", "Store", nullptr));
        label_8->setText(QCoreApplication::translate("ViewOrder", "Card", nullptr));
        label->setText(QCoreApplication::translate("ViewOrder", "Amount", nullptr));
        btnTaxReturn->setText(QCoreApplication::translate("ViewOrder", "Fiscal return", nullptr));
        btnPrintFiscal->setText(QCoreApplication::translate("ViewOrder", "Print fiscal", nullptr));
        label_9->setText(QCoreApplication::translate("ViewOrder", "Idram", nullptr));
        btnMakeDraft->setText(QCoreApplication::translate("ViewOrder", "Make draft", nullptr));
        label_7->setText(QCoreApplication::translate("ViewOrder", "Cash", nullptr));
        btnPrintReceiptA4->setText(QCoreApplication::translate("ViewOrder", "Print receipt A4", nullptr));
        btnPrintReceipt->setText(QCoreApplication::translate("ViewOrder", "Print receipt", nullptr));
        label_2->setText(QCoreApplication::translate("ViewOrder", "Tax number", nullptr));
        btnReturn->setText(QCoreApplication::translate("ViewOrder", "Items return", nullptr));
        label_14->setText(QCoreApplication::translate("ViewOrder", "Debt", nullptr));
        label_13->setText(QCoreApplication::translate("ViewOrder", "Bank transfer", nullptr));
        label_15->setText(QCoreApplication::translate("ViewOrder", "Telcell", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ViewOrder: public Ui_ViewOrder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIEWORDER_H
