/********************************************************************************
** Form generated from reading UI file 'taxprint.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TAXPRINT_H
#define UI_TAXPRINT_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
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
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_TaxPrint
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLineEdit *leCode;
    QPushButton *btnPlus;
    QPushButton *btnClear;
    QPushButton *btnPrint;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *leTaxpayerPin;
    QSpacerItem *horizontalSpacer_2;
    C5ClearTableWidget *tbl;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    C5LineEdit *leTotal;

    void setupUi(QDialog *TaxPrint)
    {
        if (TaxPrint->objectName().isEmpty())
            TaxPrint->setObjectName("TaxPrint");
        TaxPrint->resize(871, 527);
        verticalLayout = new QVBoxLayout(TaxPrint);
        verticalLayout->setObjectName("verticalLayout");
        widget = new QWidget(TaxPrint);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        leCode = new QLineEdit(widget);
        leCode->setObjectName("leCode");

        horizontalLayout->addWidget(leCode);

        btnPlus = new QPushButton(widget);
        btnPlus->setObjectName("btnPlus");
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/add.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnPlus->setIcon(icon);

        horizontalLayout->addWidget(btnPlus);

        btnClear = new QPushButton(widget);
        btnClear->setObjectName("btnClear");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/broom.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnClear->setIcon(icon1);

        horizontalLayout->addWidget(btnClear);

        btnPrint = new QPushButton(widget);
        btnPrint->setObjectName("btnPrint");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnPrint->setIcon(icon2);

        horizontalLayout->addWidget(btnPrint);


        verticalLayout->addWidget(widget);

        widget_3 = new QWidget(TaxPrint);
        widget_3->setObjectName("widget_3");
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, -1, -1, -1);
        label_2 = new QLabel(widget_3);
        label_2->setObjectName("label_2");

        horizontalLayout_3->addWidget(label_2);

        leTaxpayerPin = new QLineEdit(widget_3);
        leTaxpayerPin->setObjectName("leTaxpayerPin");
        leTaxpayerPin->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_3->addWidget(leTaxpayerPin);

        horizontalSpacer_2 = new QSpacerItem(666, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget_3);

        tbl = new C5ClearTableWidget(TaxPrint);
        if (tbl->columnCount() < 6)
            tbl->setColumnCount(6);
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
        tbl->setObjectName("tbl");

        verticalLayout->addWidget(tbl);

        widget_2 = new QWidget(TaxPrint);
        widget_2->setObjectName("widget_2");
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(656, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        label = new QLabel(widget_2);
        label->setObjectName("label");
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        label->setFont(font);

        horizontalLayout_2->addWidget(label);

        leTotal = new C5LineEdit(widget_2);
        leTotal->setObjectName("leTotal");
        leTotal->setMaximumSize(QSize(150, 16777215));
        leTotal->setReadOnly(true);

        horizontalLayout_2->addWidget(leTotal);


        verticalLayout->addWidget(widget_2);


        retranslateUi(TaxPrint);

        QMetaObject::connectSlotsByName(TaxPrint);
    } // setupUi

    void retranslateUi(QDialog *TaxPrint)
    {
        TaxPrint->setWindowTitle(QCoreApplication::translate("TaxPrint", "Print tax", nullptr));
        btnPlus->setText(QString());
        btnClear->setText(QString());
        btnPrint->setText(QString());
        label_2->setText(QCoreApplication::translate("TaxPrint", "Taxpayer pin", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("TaxPrint", "Code", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("TaxPrint", "Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("TaxPrint", "Qty", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("TaxPrint", "Price", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("TaxPrint", "Total", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tbl->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("TaxPrint", "Adg code", nullptr));
        label->setText(QCoreApplication::translate("TaxPrint", "Total", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TaxPrint: public Ui_TaxPrint {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TAXPRINT_H
