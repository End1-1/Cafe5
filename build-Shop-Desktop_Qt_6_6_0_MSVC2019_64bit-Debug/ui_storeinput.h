/********************************************************************************
** Form generated from reading UI file 'storeinput.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STOREINPUT_H
#define UI_STOREINPUT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
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

class Ui_StoreInput
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnDateLeft;
    C5DateEdit *deStart;
    C5DateEdit *deEnd;
    QPushButton *btnDateRight;
    QPushButton *btnRefresh;
    QLabel *label_2;
    QLineEdit *leFilter;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnMinimize;
    QPushButton *btnExit;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *btnAcceptMode;
    QPushButton *btnStore;
    QPushButton *btnStoreGoods;
    QPushButton *btnHistoryMode;
    QPushButton *btnAccept;
    QPushButton *btnView;
    QSpacerItem *horizontalSpacer_3;
    C5ClearTableWidget *tbl;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *chAll;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_3;
    C5LineEdit *leRows;
    QLabel *label_4;
    C5LineEdit *leQty;
    QLabel *label;
    C5LineEdit *leTotal;

    void setupUi(QDialog *StoreInput)
    {
        if (StoreInput->objectName().isEmpty())
            StoreInput->setObjectName("StoreInput");
        StoreInput->resize(868, 539);
        verticalLayout = new QVBoxLayout(StoreInput);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        widget = new QWidget(StoreInput);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btnDateLeft = new QPushButton(widget);
        btnDateLeft->setObjectName("btnDateLeft");
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btnDateLeft->sizePolicy().hasHeightForWidth());
        btnDateLeft->setSizePolicy(sizePolicy);
        btnDateLeft->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/left-arrow.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnDateLeft->setIcon(icon);

        horizontalLayout->addWidget(btnDateLeft);

        deStart = new C5DateEdit(widget);
        deStart->setObjectName("deStart");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(deStart->sizePolicy().hasHeightForWidth());
        deStart->setSizePolicy(sizePolicy1);
        deStart->setMaximumSize(QSize(120, 16777215));

        horizontalLayout->addWidget(deStart);

        deEnd = new C5DateEdit(widget);
        deEnd->setObjectName("deEnd");
        sizePolicy1.setHeightForWidth(deEnd->sizePolicy().hasHeightForWidth());
        deEnd->setSizePolicy(sizePolicy1);
        deEnd->setMaximumSize(QSize(120, 16777215));

        horizontalLayout->addWidget(deEnd);

        btnDateRight = new QPushButton(widget);
        btnDateRight->setObjectName("btnDateRight");
        sizePolicy.setHeightForWidth(btnDateRight->sizePolicy().hasHeightForWidth());
        btnDateRight->setSizePolicy(sizePolicy);
        btnDateRight->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/right-arrow.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnDateRight->setIcon(icon1);

        horizontalLayout->addWidget(btnDateRight);

        btnRefresh = new QPushButton(widget);
        btnRefresh->setObjectName("btnRefresh");
        sizePolicy.setHeightForWidth(btnRefresh->sizePolicy().hasHeightForWidth());
        btnRefresh->setSizePolicy(sizePolicy);
        btnRefresh->setFocusPolicy(Qt::NoFocus);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRefresh->setIcon(icon2);

        horizontalLayout->addWidget(btnRefresh);

        label_2 = new QLabel(widget);
        label_2->setObjectName("label_2");

        horizontalLayout->addWidget(label_2);

        leFilter = new QLineEdit(widget);
        leFilter->setObjectName("leFilter");
        sizePolicy1.setHeightForWidth(leFilter->sizePolicy().hasHeightForWidth());
        leFilter->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(leFilter);

        horizontalSpacer = new QSpacerItem(445, 17, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnMinimize = new QPushButton(widget);
        btnMinimize->setObjectName("btnMinimize");
        sizePolicy.setHeightForWidth(btnMinimize->sizePolicy().hasHeightForWidth());
        btnMinimize->setSizePolicy(sizePolicy);
        btnMinimize->setFocusPolicy(Qt::NoFocus);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/minimize.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnMinimize->setIcon(icon3);

        horizontalLayout->addWidget(btnMinimize);

        btnExit = new QPushButton(widget);
        btnExit->setObjectName("btnExit");
        sizePolicy.setHeightForWidth(btnExit->sizePolicy().hasHeightForWidth());
        btnExit->setSizePolicy(sizePolicy);
        btnExit->setFocusPolicy(Qt::NoFocus);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/logout.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnExit->setIcon(icon4);

        horizontalLayout->addWidget(btnExit);


        verticalLayout->addWidget(widget);

        widget_3 = new QWidget(StoreInput);
        widget_3->setObjectName("widget_3");
        widget_3->setMinimumSize(QSize(0, 50));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setSpacing(2);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        btnAcceptMode = new QPushButton(widget_3);
        btnAcceptMode->setObjectName("btnAcceptMode");
        sizePolicy.setHeightForWidth(btnAcceptMode->sizePolicy().hasHeightForWidth());
        btnAcceptMode->setSizePolicy(sizePolicy);
        btnAcceptMode->setStyleSheet(QString::fromUtf8("                QPushButton:checked{\n"
"                    background-color: rgb(0, 150, 0);\n"
"					font-weight:bold;\n"
"                }"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnAcceptMode->setIcon(icon5);
        btnAcceptMode->setCheckable(true);
        btnAcceptMode->setChecked(true);

        horizontalLayout_3->addWidget(btnAcceptMode);

        btnStore = new QPushButton(widget_3);
        btnStore->setObjectName("btnStore");
        sizePolicy.setHeightForWidth(btnStore->sizePolicy().hasHeightForWidth());
        btnStore->setSizePolicy(sizePolicy);
        btnStore->setStyleSheet(QString::fromUtf8("                QPushButton:checked{\n"
"                    background-color: rgb(0, 150, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnStore->setCheckable(true);

        horizontalLayout_3->addWidget(btnStore);

        btnStoreGoods = new QPushButton(widget_3);
        btnStoreGoods->setObjectName("btnStoreGoods");
        sizePolicy.setHeightForWidth(btnStoreGoods->sizePolicy().hasHeightForWidth());
        btnStoreGoods->setSizePolicy(sizePolicy);
        btnStoreGoods->setStyleSheet(QString::fromUtf8("                QPushButton:checked{\n"
"                    background-color: rgb(0, 150, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnStoreGoods->setCheckable(true);

        horizontalLayout_3->addWidget(btnStoreGoods);

        btnHistoryMode = new QPushButton(widget_3);
        btnHistoryMode->setObjectName("btnHistoryMode");
        sizePolicy.setHeightForWidth(btnHistoryMode->sizePolicy().hasHeightForWidth());
        btnHistoryMode->setSizePolicy(sizePolicy);
        btnHistoryMode->setStyleSheet(QString::fromUtf8("                QPushButton:checked{\n"
"                    background-color: rgb(0, 150, 0);\n"
"					font-weight:bold;\n"
"                }"));
        btnHistoryMode->setIcon(icon5);
        btnHistoryMode->setCheckable(true);
        btnHistoryMode->setChecked(false);

        horizontalLayout_3->addWidget(btnHistoryMode);

        btnAccept = new QPushButton(widget_3);
        btnAccept->setObjectName("btnAccept");
        sizePolicy.setHeightForWidth(btnAccept->sizePolicy().hasHeightForWidth());
        btnAccept->setSizePolicy(sizePolicy);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/decision.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnAccept->setIcon(icon6);

        horizontalLayout_3->addWidget(btnAccept);

        btnView = new QPushButton(widget_3);
        btnView->setObjectName("btnView");
        sizePolicy.setHeightForWidth(btnView->sizePolicy().hasHeightForWidth());
        btnView->setSizePolicy(sizePolicy);
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/package.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnView->setIcon(icon7);

        horizontalLayout_3->addWidget(btnView);

        horizontalSpacer_3 = new QSpacerItem(383, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout->addWidget(widget_3);

        tbl = new C5ClearTableWidget(StoreInput);
        if (tbl->columnCount() < 8)
            tbl->setColumnCount(8);
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
        tbl->setObjectName("tbl");
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout->addWidget(tbl);

        widget_2 = new QWidget(StoreInput);
        widget_2->setObjectName("widget_2");
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        chAll = new QCheckBox(widget_2);
        chAll->setObjectName("chAll");

        horizontalLayout_2->addWidget(chAll);

        horizontalSpacer_2 = new QSpacerItem(693, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        label_3 = new QLabel(widget_2);
        label_3->setObjectName("label_3");

        horizontalLayout_2->addWidget(label_3);

        leRows = new C5LineEdit(widget_2);
        leRows->setObjectName("leRows");
        leRows->setMinimumSize(QSize(150, 0));
        leRows->setMaximumSize(QSize(150, 16777215));
        leRows->setReadOnly(true);

        horizontalLayout_2->addWidget(leRows);

        label_4 = new QLabel(widget_2);
        label_4->setObjectName("label_4");

        horizontalLayout_2->addWidget(label_4);

        leQty = new C5LineEdit(widget_2);
        leQty->setObjectName("leQty");
        leQty->setMinimumSize(QSize(150, 0));
        leQty->setMaximumSize(QSize(150, 16777215));
        leQty->setReadOnly(true);

        horizontalLayout_2->addWidget(leQty);

        label = new QLabel(widget_2);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        leTotal = new C5LineEdit(widget_2);
        leTotal->setObjectName("leTotal");
        leTotal->setMinimumSize(QSize(200, 0));
        leTotal->setMaximumSize(QSize(200, 16777215));
        leTotal->setReadOnly(true);

        horizontalLayout_2->addWidget(leTotal);


        verticalLayout->addWidget(widget_2);


        retranslateUi(StoreInput);

        QMetaObject::connectSlotsByName(StoreInput);
    } // setupUi

    void retranslateUi(QDialog *StoreInput)
    {
        StoreInput->setWindowTitle(QCoreApplication::translate("StoreInput", "Store input", nullptr));
        btnDateLeft->setText(QString());
        btnDateRight->setText(QString());
        btnRefresh->setText(QString());
        label_2->setText(QCoreApplication::translate("StoreInput", "Filter", nullptr));
        btnMinimize->setText(QString());
        btnExit->setText(QString());
        btnAcceptMode->setText(QCoreApplication::translate("StoreInput", "Checking", nullptr));
        btnStore->setText(QCoreApplication::translate("StoreInput", "Store by groups", nullptr));
        btnStoreGoods->setText(QCoreApplication::translate("StoreInput", "Store by goods", nullptr));
        btnHistoryMode->setText(QCoreApplication::translate("StoreInput", "History", nullptr));
        btnAccept->setText(QCoreApplication::translate("StoreInput", "Accept", nullptr));
        btnView->setText(QCoreApplication::translate("StoreInput", "View", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("StoreInput", "UUID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("StoreInput", "X", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("StoreInput", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("StoreInput", "Store", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("StoreInput", "Goods", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tbl->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("StoreInput", "Scancode", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tbl->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("StoreInput", "Qty", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tbl->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("StoreInput", "Retail", nullptr));
        chAll->setText(QCoreApplication::translate("StoreInput", "Check all", nullptr));
        label_3->setText(QCoreApplication::translate("StoreInput", "Checked rows", nullptr));
        label_4->setText(QCoreApplication::translate("StoreInput", "Checked qty", nullptr));
        label->setText(QCoreApplication::translate("StoreInput", "Total", nullptr));
    } // retranslateUi

};

namespace Ui {
    class StoreInput: public Ui_StoreInput {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STOREINPUT_H
