/********************************************************************************
** Form generated from reading UI file 'worder.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WORDER_H
#define UI_WORDER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <c5cleartablewidget.h>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_WOrder
{
public:
    QVBoxLayout *verticalLayout_2;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *leCode;
    C5ClearTableWidget *tblData;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QGroupBox *grPayment;
    QFormLayout *formLayout;
    QLabel *lbTotal;
    C5LineEdit *leTotal;
    QLabel *lbDisc;
    C5LineEdit *leDisc;
    QGroupBox *grCustomer;
    QGridLayout *gridLayout;
    QLabel *lbPartner_3;
    QLabel *lbPartner;
    C5LineEdit *leCustomerTaxpayerId;
    QLabel *label_6;
    C5LineEdit *leContact;
    C5LineEdit *leGiftCardAmount;
    QLabel *lbGiftCard;
    QLabel *lbCustomer_2;
    C5LineEdit *leOrganization;
    QPushButton *btnSearchPartner;
    QSpacerItem *horizontalSpacer;
    QGroupBox *grFlags;
    QVBoxLayout *verticalLayout_3;
    QPushButton *btnF1;
    QPushButton *btnF2;
    QPushButton *btnF3;
    QPushButton *btnF4;
    QPushButton *btnF5;
    QGroupBox *grImage;
    QVBoxLayout *verticalLayout;
    QLabel *lbGoodsImage;

    void setupUi(QWidget *WOrder)
    {
        if (WOrder->objectName().isEmpty())
            WOrder->setObjectName("WOrder");
        WOrder->resize(1398, 703);
        verticalLayout_2 = new QVBoxLayout(WOrder);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName("verticalLayout_2");
        widget = new QWidget(WOrder);
        widget->setObjectName("widget");
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        leCode = new QLineEdit(widget);
        leCode->setObjectName("leCode");

        horizontalLayout_2->addWidget(leCode);


        verticalLayout_2->addWidget(widget);

        tblData = new C5ClearTableWidget(WOrder);
        if (tblData->columnCount() < 13)
            tblData->setColumnCount(13);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(10, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(11, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tblData->setHorizontalHeaderItem(12, __qtablewidgetitem12);
        tblData->setObjectName("tblData");
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(tblData->sizePolicy().hasHeightForWidth());
        tblData->setSizePolicy(sizePolicy);
        tblData->setFocusPolicy(Qt::NoFocus);
        tblData->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblData->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        tblData->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblData->setSelectionMode(QAbstractItemView::SingleSelection);
        tblData->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_2->addWidget(tblData);

        widget_2 = new QWidget(WOrder);
        widget_2->setObjectName("widget_2");
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        grPayment = new QGroupBox(widget_2);
        grPayment->setObjectName("grPayment");
        grPayment->setMaximumSize(QSize(350, 16777215));
        formLayout = new QFormLayout(grPayment);
        formLayout->setObjectName("formLayout");
        formLayout->setHorizontalSpacing(2);
        formLayout->setVerticalSpacing(2);
        formLayout->setContentsMargins(2, 2, 2, 2);
        lbTotal = new QLabel(grPayment);
        lbTotal->setObjectName("lbTotal");

        formLayout->setWidget(0, QFormLayout::LabelRole, lbTotal);

        leTotal = new C5LineEdit(grPayment);
        leTotal->setObjectName("leTotal");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(leTotal->sizePolicy().hasHeightForWidth());
        leTotal->setSizePolicy(sizePolicy1);
        leTotal->setFocusPolicy(Qt::NoFocus);
        leTotal->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        leTotal->setReadOnly(true);

        formLayout->setWidget(0, QFormLayout::FieldRole, leTotal);

        lbDisc = new QLabel(grPayment);
        lbDisc->setObjectName("lbDisc");
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lbDisc->sizePolicy().hasHeightForWidth());
        lbDisc->setSizePolicy(sizePolicy2);

        formLayout->setWidget(1, QFormLayout::LabelRole, lbDisc);

        leDisc = new C5LineEdit(grPayment);
        leDisc->setObjectName("leDisc");
        leDisc->setFocusPolicy(Qt::NoFocus);
        leDisc->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        leDisc->setReadOnly(true);

        formLayout->setWidget(1, QFormLayout::FieldRole, leDisc);


        horizontalLayout->addWidget(grPayment);

        grCustomer = new QGroupBox(widget_2);
        grCustomer->setObjectName("grCustomer");
        grCustomer->setMaximumSize(QSize(400, 16777215));
        gridLayout = new QGridLayout(grCustomer);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setHorizontalSpacing(2);
        gridLayout->setContentsMargins(2, 2, 2, 2);
        lbPartner_3 = new QLabel(grCustomer);
        lbPartner_3->setObjectName("lbPartner_3");
        sizePolicy2.setHeightForWidth(lbPartner_3->sizePolicy().hasHeightForWidth());
        lbPartner_3->setSizePolicy(sizePolicy2);
        QFont font;
        font.setBold(false);
        lbPartner_3->setFont(font);

        gridLayout->addWidget(lbPartner_3, 3, 0, 1, 1);

        lbPartner = new QLabel(grCustomer);
        lbPartner->setObjectName("lbPartner");
        sizePolicy2.setHeightForWidth(lbPartner->sizePolicy().hasHeightForWidth());
        lbPartner->setSizePolicy(sizePolicy2);
        lbPartner->setFont(font);

        gridLayout->addWidget(lbPartner, 2, 0, 1, 1);

        leCustomerTaxpayerId = new C5LineEdit(grCustomer);
        leCustomerTaxpayerId->setObjectName("leCustomerTaxpayerId");
        leCustomerTaxpayerId->setMinimumSize(QSize(200, 0));
        leCustomerTaxpayerId->setMaximumSize(QSize(16777215, 16777215));
        QFont font1;
        font1.setBold(true);
        leCustomerTaxpayerId->setFont(font1);
        leCustomerTaxpayerId->setFocusPolicy(Qt::StrongFocus);
        leCustomerTaxpayerId->setMaxLength(16);
        leCustomerTaxpayerId->setReadOnly(false);

        gridLayout->addWidget(leCustomerTaxpayerId, 0, 1, 1, 2);

        label_6 = new QLabel(grCustomer);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 4, 0, 1, 1);

        leContact = new C5LineEdit(grCustomer);
        leContact->setObjectName("leContact");
        leContact->setMinimumSize(QSize(200, 0));
        leContact->setMaximumSize(QSize(16777215, 16777215));
        leContact->setFont(font);
        leContact->setFocusPolicy(Qt::NoFocus);
        leContact->setReadOnly(true);

        gridLayout->addWidget(leContact, 3, 1, 1, 2);

        leGiftCardAmount = new C5LineEdit(grCustomer);
        leGiftCardAmount->setObjectName("leGiftCardAmount");
        leGiftCardAmount->setReadOnly(true);

        gridLayout->addWidget(leGiftCardAmount, 5, 1, 1, 2);

        lbGiftCard = new QLabel(grCustomer);
        lbGiftCard->setObjectName("lbGiftCard");

        gridLayout->addWidget(lbGiftCard, 5, 0, 1, 1);

        lbCustomer_2 = new QLabel(grCustomer);
        lbCustomer_2->setObjectName("lbCustomer_2");
        sizePolicy2.setHeightForWidth(lbCustomer_2->sizePolicy().hasHeightForWidth());
        lbCustomer_2->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(lbCustomer_2, 0, 0, 1, 1);

        leOrganization = new C5LineEdit(grCustomer);
        leOrganization->setObjectName("leOrganization");
        leOrganization->setMinimumSize(QSize(200, 0));
        leOrganization->setMaximumSize(QSize(16777215, 16777215));
        leOrganization->setFont(font);
        leOrganization->setFocusPolicy(Qt::NoFocus);
        leOrganization->setReadOnly(true);

        gridLayout->addWidget(leOrganization, 2, 1, 1, 2);

        btnSearchPartner = new QPushButton(grCustomer);
        btnSearchPartner->setObjectName("btnSearchPartner");
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(btnSearchPartner->sizePolicy().hasHeightForWidth());
        btnSearchPartner->setSizePolicy(sizePolicy3);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/search.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnSearchPartner->setIcon(icon);

        gridLayout->addWidget(btnSearchPartner, 0, 3, 1, 1);


        horizontalLayout->addWidget(grCustomer);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        grFlags = new QGroupBox(widget_2);
        grFlags->setObjectName("grFlags");
        verticalLayout_3 = new QVBoxLayout(grFlags);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(2, 2, 2, 2);
        btnF1 = new QPushButton(grFlags);
        btnF1->setObjectName("btnF1");
        btnF1->setFocusPolicy(Qt::NoFocus);
        btnF1->setStyleSheet(QString::fromUtf8("QPushButton:checked{\n"
"                    background-color: rgb(250, 0, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnF1->setCheckable(true);
        btnF1->setChecked(false);
        btnF1->setProperty("id", QVariant(1));

        verticalLayout_3->addWidget(btnF1);

        btnF2 = new QPushButton(grFlags);
        btnF2->setObjectName("btnF2");
        btnF2->setFocusPolicy(Qt::NoFocus);
        btnF2->setStyleSheet(QString::fromUtf8("QPushButton:checked{\n"
"                    background-color: rgb(250, 0, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnF2->setCheckable(true);
        btnF2->setChecked(false);
        btnF2->setProperty("id", QVariant(2));

        verticalLayout_3->addWidget(btnF2);

        btnF3 = new QPushButton(grFlags);
        btnF3->setObjectName("btnF3");
        btnF3->setFocusPolicy(Qt::NoFocus);
        btnF3->setStyleSheet(QString::fromUtf8("QPushButton:checked{\n"
"                    background-color: rgb(250, 0, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnF3->setCheckable(true);
        btnF3->setChecked(false);
        btnF3->setProperty("id", QVariant(3));

        verticalLayout_3->addWidget(btnF3);

        btnF4 = new QPushButton(grFlags);
        btnF4->setObjectName("btnF4");
        btnF4->setFocusPolicy(Qt::NoFocus);
        btnF4->setStyleSheet(QString::fromUtf8("QPushButton:checked{\n"
"                    background-color: rgb(250, 0, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnF4->setCheckable(true);
        btnF4->setChecked(false);
        btnF4->setProperty("id", QVariant(4));

        verticalLayout_3->addWidget(btnF4);

        btnF5 = new QPushButton(grFlags);
        btnF5->setObjectName("btnF5");
        btnF5->setFocusPolicy(Qt::NoFocus);
        btnF5->setStyleSheet(QString::fromUtf8("QPushButton:checked{\n"
"                    background-color: rgb(250, 0, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnF5->setCheckable(true);
        btnF5->setChecked(false);
        btnF5->setProperty("id", QVariant(5));

        verticalLayout_3->addWidget(btnF5);


        horizontalLayout->addWidget(grFlags);

        grImage = new QGroupBox(widget_2);
        grImage->setObjectName("grImage");
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(1);
        sizePolicy4.setHeightForWidth(grImage->sizePolicy().hasHeightForWidth());
        grImage->setSizePolicy(sizePolicy4);
        grImage->setMaximumSize(QSize(200, 200));
        verticalLayout = new QVBoxLayout(grImage);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        lbGoodsImage = new QLabel(grImage);
        lbGoodsImage->setObjectName("lbGoodsImage");
        lbGoodsImage->setMinimumSize(QSize(200, 0));
        lbGoodsImage->setScaledContents(true);
        lbGoodsImage->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lbGoodsImage);


        horizontalLayout->addWidget(grImage);


        verticalLayout_2->addWidget(widget_2);

        QWidget::setTabOrder(leCode, btnF4);
        QWidget::setTabOrder(btnF4, btnF5);
        QWidget::setTabOrder(btnF5, btnF1);
        QWidget::setTabOrder(btnF1, btnF2);
        QWidget::setTabOrder(btnF2, btnF3);

        retranslateUi(WOrder);

        QMetaObject::connectSlotsByName(WOrder);
    } // setupUi

    void retranslateUi(QWidget *WOrder)
    {
        WOrder->setWindowTitle(QCoreApplication::translate("WOrder", "Form", nullptr));
        label->setText(QCoreApplication::translate("WOrder", "Goods code (F7)", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tblData->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("WOrder", "Barcode", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tblData->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("WOrder", "Group", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tblData->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("WOrder", "Goods name", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tblData->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("WOrder", "Qty", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tblData->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("WOrder", "Box", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tblData->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("WOrder", "Unit", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tblData->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("WOrder", "Price", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tblData->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("WOrder", "Total", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tblData->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("WOrder", "Discount", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tblData->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("WOrder", "Discount mode", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tblData->horizontalHeaderItem(10);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("WOrder", "Discount factor", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tblData->horizontalHeaderItem(11);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("WOrder", "Stock", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tblData->horizontalHeaderItem(12);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("WOrder", "QR", nullptr));
        grPayment->setTitle(QCoreApplication::translate("WOrder", "Payment", nullptr));
        lbTotal->setText(QCoreApplication::translate("WOrder", "Total", nullptr));
        leTotal->setText(QCoreApplication::translate("WOrder", "0", nullptr));
        lbDisc->setText(QCoreApplication::translate("WOrder", "Discount", nullptr));
        leDisc->setText(QCoreApplication::translate("WOrder", "0", nullptr));
        grCustomer->setTitle(QCoreApplication::translate("WOrder", "Customer", nullptr));
        lbPartner_3->setText(QCoreApplication::translate("WOrder", "Contact", nullptr));
        lbPartner->setText(QCoreApplication::translate("WOrder", "Organization", nullptr));
        label_6->setText(QCoreApplication::translate("WOrder", "Gift card balance", nullptr));
        lbGiftCard->setText(QCoreApplication::translate("WOrder", "*****", nullptr));
        lbCustomer_2->setText(QCoreApplication::translate("WOrder", "Customer taxpayer id (F6)", nullptr));
        btnSearchPartner->setText(QString());
        grFlags->setTitle(QCoreApplication::translate("WOrder", "Flags", nullptr));
        btnF1->setText(QCoreApplication::translate("WOrder", "F1", nullptr));
        btnF2->setText(QCoreApplication::translate("WOrder", "F2", nullptr));
        btnF3->setText(QCoreApplication::translate("WOrder", "F3", nullptr));
        btnF4->setText(QCoreApplication::translate("WOrder", "F4", nullptr));
        btnF5->setText(QCoreApplication::translate("WOrder", "F5", nullptr));
        grImage->setTitle(QCoreApplication::translate("WOrder", "Goods Image", nullptr));
        lbGoodsImage->setText(QCoreApplication::translate("WOrder", "Image", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WOrder: public Ui_WOrder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WORDER_H
