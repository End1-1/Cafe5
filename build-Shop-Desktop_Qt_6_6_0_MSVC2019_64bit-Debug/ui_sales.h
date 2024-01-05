/********************************************************************************
** Form generated from reading UI file 'sales.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SALES_H
#define UI_SALES_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QCommandLinkButton>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
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

class Ui_Sales
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *wsalerow1;
    QGridLayout *gridLayout;
    QLabel *label_3;
    C5DateEdit *deEnd;
    QLabel *label_5;
    QSpacerItem *horizontalSpacer;
    C5LineEdit *leLogin;
    QPushButton *btnExit;
    QLabel *label_4;
    C5DateEdit *deStart;
    QPushButton *btnDateRight;
    QPushButton *btnDateLeft;
    QPushButton *btnRefresh;
    QLineEdit *leFilter;
    QLabel *label_2;
    QComboBox *cbHall;
    QWidget *wsale_row2;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnModeTotal;
    QPushButton *btnModeItems;
    QPushButton *btnTotalByItems;
    QPushButton *btnGroups;
    QPushButton *btnViewOrder;
    QPushButton *btnItemBack;
    QPushButton *btnCashColletion;
    QPushButton *btnPrintTaxZ;
    QPushButton *btnPrintTaxX;
    QPushButton *btnChangeDate;
    QSpacerItem *horizontalSpacer_3;
    C5ClearTableWidget *tbl;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QCommandLinkButton *btnRetryUpload;
    QSpacerItem *horizontalSpacer_2;
    QLabel *lbTotalQty;
    C5LineEdit *leTotalQty;
    QLabel *lbRetail;
    C5LineEdit *leRetail;
    QLabel *lbWhosale;
    C5LineEdit *leWhosale;
    QLabel *label;
    C5LineEdit *leTotal;

    void setupUi(QDialog *Sales)
    {
        if (Sales->objectName().isEmpty())
            Sales->setObjectName("Sales");
        Sales->resize(1053, 755);
        verticalLayout = new QVBoxLayout(Sales);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        wsalerow1 = new QWidget(Sales);
        wsalerow1->setObjectName("wsalerow1");
        gridLayout = new QGridLayout(wsalerow1);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(wsalerow1);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 0, 5, 1, 1);

        deEnd = new C5DateEdit(wsalerow1);
        deEnd->setObjectName("deEnd");
        deEnd->setMinimumSize(QSize(100, 0));
        deEnd->setMaximumSize(QSize(130, 16777215));

        gridLayout->addWidget(deEnd, 0, 3, 1, 1);

        label_5 = new QLabel(wsalerow1);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 0, 10, 1, 1);

        horizontalSpacer = new QSpacerItem(521, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 12, 1, 1);

        leLogin = new C5LineEdit(wsalerow1);
        leLogin->setObjectName("leLogin");
        leLogin->setMinimumSize(QSize(80, 0));
        leLogin->setMaximumSize(QSize(80, 16777215));

        gridLayout->addWidget(leLogin, 0, 6, 1, 1);

        btnExit = new QPushButton(wsalerow1);
        btnExit->setObjectName("btnExit");
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btnExit->sizePolicy().hasHeightForWidth());
        btnExit->setSizePolicy(sizePolicy);
        btnExit->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/logout.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnExit->setIcon(icon);

        gridLayout->addWidget(btnExit, 0, 13, 1, 1);

        label_4 = new QLabel(wsalerow1);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 0, 8, 1, 1);

        deStart = new C5DateEdit(wsalerow1);
        deStart->setObjectName("deStart");
        deStart->setMinimumSize(QSize(100, 0));
        deStart->setMaximumSize(QSize(130, 16777215));

        gridLayout->addWidget(deStart, 0, 2, 1, 1);

        btnDateRight = new QPushButton(wsalerow1);
        btnDateRight->setObjectName("btnDateRight");
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btnDateRight->sizePolicy().hasHeightForWidth());
        btnDateRight->setSizePolicy(sizePolicy1);
        btnDateRight->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/right-arrow.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnDateRight->setIcon(icon1);

        gridLayout->addWidget(btnDateRight, 0, 4, 1, 1);

        btnDateLeft = new QPushButton(wsalerow1);
        btnDateLeft->setObjectName("btnDateLeft");
        sizePolicy1.setHeightForWidth(btnDateLeft->sizePolicy().hasHeightForWidth());
        btnDateLeft->setSizePolicy(sizePolicy1);
        btnDateLeft->setFocusPolicy(Qt::NoFocus);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/left-arrow.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnDateLeft->setIcon(icon2);

        gridLayout->addWidget(btnDateLeft, 0, 1, 1, 1);

        btnRefresh = new QPushButton(wsalerow1);
        btnRefresh->setObjectName("btnRefresh");
        sizePolicy1.setHeightForWidth(btnRefresh->sizePolicy().hasHeightForWidth());
        btnRefresh->setSizePolicy(sizePolicy1);
        btnRefresh->setFocusPolicy(Qt::NoFocus);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRefresh->setIcon(icon3);

        gridLayout->addWidget(btnRefresh, 0, 7, 1, 1);

        leFilter = new QLineEdit(wsalerow1);
        leFilter->setObjectName("leFilter");
        leFilter->setMinimumSize(QSize(120, 0));

        gridLayout->addWidget(leFilter, 0, 9, 1, 1);

        label_2 = new QLabel(wsalerow1);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        cbHall = new QComboBox(wsalerow1);
        cbHall->setObjectName("cbHall");

        gridLayout->addWidget(cbHall, 0, 11, 1, 1);


        verticalLayout->addWidget(wsalerow1);

        wsale_row2 = new QWidget(Sales);
        wsale_row2->setObjectName("wsale_row2");
        wsale_row2->setMinimumSize(QSize(0, 0));
        horizontalLayout = new QHBoxLayout(wsale_row2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btnModeTotal = new QPushButton(wsale_row2);
        btnModeTotal->setObjectName("btnModeTotal");
        btnModeTotal->setStyleSheet(QString::fromUtf8("                QPushButton:checked{\n"
"                    background-color: rgb(0, 150, 0);\n"
"					font-weight:bold;\n"
"                }"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnModeTotal->setIcon(icon4);
        btnModeTotal->setCheckable(true);
        btnModeTotal->setChecked(true);

        horizontalLayout->addWidget(btnModeTotal);

        btnModeItems = new QPushButton(wsale_row2);
        btnModeItems->setObjectName("btnModeItems");
        btnModeItems->setStyleSheet(QString::fromUtf8("                QPushButton:checked{\n"
"                    background-color: rgb(0, 150, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnModeItems->setIcon(icon4);
        btnModeItems->setCheckable(true);
        btnModeItems->setChecked(false);

        horizontalLayout->addWidget(btnModeItems);

        btnTotalByItems = new QPushButton(wsale_row2);
        btnTotalByItems->setObjectName("btnTotalByItems");
        btnTotalByItems->setStyleSheet(QString::fromUtf8("                QPushButton:checked{\n"
"                    background-color: rgb(0, 150, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnTotalByItems->setIcon(icon4);
        btnTotalByItems->setCheckable(true);
        btnTotalByItems->setChecked(false);

        horizontalLayout->addWidget(btnTotalByItems);

        btnGroups = new QPushButton(wsale_row2);
        btnGroups->setObjectName("btnGroups");
        btnGroups->setStyleSheet(QString::fromUtf8("                QPushButton:checked{\n"
"                    background-color: rgb(0, 150, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnGroups->setIcon(icon4);
        btnGroups->setCheckable(true);
        btnGroups->setChecked(false);

        horizontalLayout->addWidget(btnGroups);

        btnViewOrder = new QPushButton(wsale_row2);
        btnViewOrder->setObjectName("btnViewOrder");
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/bill.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnViewOrder->setIcon(icon5);

        horizontalLayout->addWidget(btnViewOrder);

        btnItemBack = new QPushButton(wsale_row2);
        btnItemBack->setObjectName("btnItemBack");
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnItemBack->setIcon(icon6);

        horizontalLayout->addWidget(btnItemBack);

        btnCashColletion = new QPushButton(wsale_row2);
        btnCashColletion->setObjectName("btnCashColletion");
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/cashcollection.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCashColletion->setIcon(icon7);

        horizontalLayout->addWidget(btnCashColletion);

        btnPrintTaxZ = new QPushButton(wsale_row2);
        btnPrintTaxZ->setObjectName("btnPrintTaxZ");
        btnPrintTaxZ->setIcon(icon4);

        horizontalLayout->addWidget(btnPrintTaxZ);

        btnPrintTaxX = new QPushButton(wsale_row2);
        btnPrintTaxX->setObjectName("btnPrintTaxX");
        btnPrintTaxX->setIcon(icon4);

        horizontalLayout->addWidget(btnPrintTaxX);

        btnChangeDate = new QPushButton(wsale_row2);
        btnChangeDate->setObjectName("btnChangeDate");
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/calendar.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnChangeDate->setIcon(icon8);

        horizontalLayout->addWidget(btnChangeDate);

        horizontalSpacer_3 = new QSpacerItem(413, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        verticalLayout->addWidget(wsale_row2);

        tbl = new C5ClearTableWidget(Sales);
        if (tbl->columnCount() < 9)
            tbl->setColumnCount(9);
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
        tbl->setObjectName("tbl");
        tbl->setFocusPolicy(Qt::NoFocus);
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setSelectionMode(QAbstractItemView::SingleSelection);
        tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
        tbl->setSortingEnabled(true);
        tbl->verticalHeader()->setDefaultSectionSize(25);

        verticalLayout->addWidget(tbl);

        widget_2 = new QWidget(Sales);
        widget_2->setObjectName("widget_2");
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, -1, 0, -1);
        btnRetryUpload = new QCommandLinkButton(widget_2);
        btnRetryUpload->setObjectName("btnRetryUpload");

        horizontalLayout_2->addWidget(btnRetryUpload);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        lbTotalQty = new QLabel(widget_2);
        lbTotalQty->setObjectName("lbTotalQty");

        horizontalLayout_2->addWidget(lbTotalQty);

        leTotalQty = new C5LineEdit(widget_2);
        leTotalQty->setObjectName("leTotalQty");
        leTotalQty->setMaximumSize(QSize(200, 16777215));
        leTotalQty->setFocusPolicy(Qt::NoFocus);
        leTotalQty->setReadOnly(true);

        horizontalLayout_2->addWidget(leTotalQty);

        lbRetail = new QLabel(widget_2);
        lbRetail->setObjectName("lbRetail");

        horizontalLayout_2->addWidget(lbRetail);

        leRetail = new C5LineEdit(widget_2);
        leRetail->setObjectName("leRetail");
        leRetail->setMaximumSize(QSize(200, 16777215));
        leRetail->setFocusPolicy(Qt::NoFocus);
        leRetail->setReadOnly(true);

        horizontalLayout_2->addWidget(leRetail);

        lbWhosale = new QLabel(widget_2);
        lbWhosale->setObjectName("lbWhosale");

        horizontalLayout_2->addWidget(lbWhosale);

        leWhosale = new C5LineEdit(widget_2);
        leWhosale->setObjectName("leWhosale");
        leWhosale->setMaximumSize(QSize(200, 16777215));
        leWhosale->setFocusPolicy(Qt::NoFocus);
        leWhosale->setReadOnly(true);

        horizontalLayout_2->addWidget(leWhosale);

        label = new QLabel(widget_2);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        leTotal = new C5LineEdit(widget_2);
        leTotal->setObjectName("leTotal");
        leTotal->setMaximumSize(QSize(200, 16777215));
        leTotal->setFocusPolicy(Qt::NoFocus);
        leTotal->setReadOnly(true);

        horizontalLayout_2->addWidget(leTotal);


        verticalLayout->addWidget(widget_2);


        retranslateUi(Sales);

        QMetaObject::connectSlotsByName(Sales);
    } // setupUi

    void retranslateUi(QDialog *Sales)
    {
        Sales->setWindowTitle(QCoreApplication::translate("Sales", "Sales", nullptr));
        label_3->setText(QCoreApplication::translate("Sales", "Seller code", nullptr));
        label_5->setText(QCoreApplication::translate("Sales", "Hall", nullptr));
        btnExit->setText(QString());
        label_4->setText(QCoreApplication::translate("Sales", "Filter", nullptr));
        btnDateRight->setText(QString());
        btnDateLeft->setText(QString());
        btnRefresh->setText(QString());
        label_2->setText(QCoreApplication::translate("Sales", "Date range", nullptr));
        btnModeTotal->setText(QCoreApplication::translate("Sales", "Total mode", nullptr));
        btnModeItems->setText(QCoreApplication::translate("Sales", "Item mode", nullptr));
        btnTotalByItems->setText(QCoreApplication::translate("Sales", "Total items", nullptr));
        btnGroups->setText(QCoreApplication::translate("Sales", "By groups", nullptr));
        btnViewOrder->setText(QCoreApplication::translate("Sales", "Order details", nullptr));
        btnItemBack->setText(QCoreApplication::translate("Sales", "Item return", nullptr));
        btnCashColletion->setText(QCoreApplication::translate("Sales", "Cash collection", nullptr));
        btnPrintTaxZ->setText(QCoreApplication::translate("Sales", "Print tax Z", nullptr));
        btnPrintTaxX->setText(QCoreApplication::translate("Sales", "Print tax X", nullptr));
        btnChangeDate->setText(QCoreApplication::translate("Sales", "Change Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("Sales", "UUID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("Sales", "Sale type code", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("Sales", "Sale type", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("Sales", "Number", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("Sales", "Tax", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tbl->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("Sales", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tbl->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("Sales", "Time", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tbl->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("Sales", "Amount", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tbl->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("Sales", "Customer", nullptr));
        btnRetryUpload->setText(QCoreApplication::translate("Sales", "Upload incomplete, retry", nullptr));
        lbTotalQty->setText(QCoreApplication::translate("Sales", "Total qty", nullptr));
        lbRetail->setText(QCoreApplication::translate("Sales", "Retail", nullptr));
        lbWhosale->setText(QCoreApplication::translate("Sales", "Whosale", nullptr));
        label->setText(QCoreApplication::translate("Sales", "Total amount", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Sales: public Ui_Sales {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SALES_H
