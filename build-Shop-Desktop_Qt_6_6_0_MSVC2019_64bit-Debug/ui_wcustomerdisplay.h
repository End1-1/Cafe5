/********************************************************************************
** Form generated from reading UI file 'wcustomerdisplay.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WCUSTOMERDISPLAY_H
#define UI_WCUSTOMERDISPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WCustomerDisplay
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *WCustomerDisplayWidget;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *tbl;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QLabel *lbTotalText;
    QSpacerItem *horizontalSpacer;
    QLabel *lbAmount;

    void setupUi(QWidget *WCustomerDisplay)
    {
        if (WCustomerDisplay->objectName().isEmpty())
            WCustomerDisplay->setObjectName("WCustomerDisplay");
        WCustomerDisplay->resize(800, 480);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(WCustomerDisplay->sizePolicy().hasHeightForWidth());
        WCustomerDisplay->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(WCustomerDisplay);
        verticalLayout->setObjectName("verticalLayout");
        WCustomerDisplayWidget = new QWidget(WCustomerDisplay);
        WCustomerDisplayWidget->setObjectName("WCustomerDisplayWidget");
        verticalLayout_2 = new QVBoxLayout(WCustomerDisplayWidget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        tbl = new QTableWidget(WCustomerDisplayWidget);
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
        QFont font;
        font.setBold(true);
        tbl->setFont(font);
        tbl->setStyleSheet(QString::fromUtf8("QHeaderView::section \n"
"{\n"
"    background-color: transparent;\n"
"    color: black;\n"
"    font: 16pt;\n"
"    padding-left: 4px;\n"
"    border: 1px solid #e0e0e0;\n"
"}"));
        tbl->setFrameShape(QFrame::NoFrame);
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setSelectionMode(QAbstractItemView::NoSelection);
        tbl->setGridStyle(Qt::NoPen);
        tbl->setCornerButtonEnabled(false);

        verticalLayout_2->addWidget(tbl);

        widget_2 = new QWidget(WCustomerDisplayWidget);
        widget_2->setObjectName("widget_2");
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        lbTotalText = new QLabel(widget_2);
        lbTotalText->setObjectName("lbTotalText");
        lbTotalText->setStyleSheet(QString::fromUtf8("font: 70 36pt ;"));

        horizontalLayout->addWidget(lbTotalText);

        horizontalSpacer = new QSpacerItem(134, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        lbAmount = new QLabel(widget_2);
        lbAmount->setObjectName("lbAmount");
        QFont font1;
        font1.setPointSize(48);
        font1.setBold(false);
        font1.setItalic(false);
        lbAmount->setFont(font1);
        lbAmount->setStyleSheet(QString::fromUtf8("font: 75 48pt ;"));

        horizontalLayout->addWidget(lbAmount);


        verticalLayout_2->addWidget(widget_2);


        verticalLayout->addWidget(WCustomerDisplayWidget);


        retranslateUi(WCustomerDisplay);

        QMetaObject::connectSlotsByName(WCustomerDisplay);
    } // setupUi

    void retranslateUi(QWidget *WCustomerDisplay)
    {
        WCustomerDisplay->setWindowTitle(QCoreApplication::translate("WCustomerDisplay", "Form", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("WCustomerDisplay", "Goods", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("WCustomerDisplay", "Quantity", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("WCustomerDisplay", "Price", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("WCustomerDisplay", "Total", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("WCustomerDisplay", "Discount", nullptr));
        tbl->setProperty("bold", QVariant(QCoreApplication::translate("WCustomerDisplay", "2", nullptr)));
        lbTotalText->setText(QCoreApplication::translate("WCustomerDisplay", "Grand total", nullptr));
        lbTotalText->setProperty("bold", QVariant(QCoreApplication::translate("WCustomerDisplay", "1", nullptr)));
        lbAmount->setText(QCoreApplication::translate("WCustomerDisplay", "0.0", nullptr));
        lbAmount->setProperty("bold", QVariant(QCoreApplication::translate("WCustomerDisplay", "1", nullptr)));
    } // retranslateUi

};

namespace Ui {
    class WCustomerDisplay: public Ui_WCustomerDisplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WCUSTOMERDISPLAY_H
