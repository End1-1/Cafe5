/********************************************************************************
** Form generated from reading UI file 'dlggoodslist.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGGOODSLIST_H
#define UI_DLGGOODSLIST_H

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
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_DlgGoodsList
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *leSearch;
    QPushButton *btnMinimize;
    QPushButton *btnExit;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QCheckBox *chName;
    QCheckBox *chScancode;
    QSpacerItem *horizontalSpacer_3;
    C5ClearTableWidget *tbl;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *lbTotalRetail;
    C5LineEdit *leTotalRetail;

    void setupUi(QDialog *DlgGoodsList)
    {
        if (DlgGoodsList->objectName().isEmpty())
            DlgGoodsList->setObjectName("DlgGoodsList");
        DlgGoodsList->resize(857, 547);
        verticalLayout = new QVBoxLayout(DlgGoodsList);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        widget = new QWidget(DlgGoodsList);
        widget->setObjectName("widget");
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        leSearch = new QLineEdit(widget);
        leSearch->setObjectName("leSearch");

        horizontalLayout_2->addWidget(leSearch);

        btnMinimize = new QPushButton(widget);
        btnMinimize->setObjectName("btnMinimize");
        btnMinimize->setMinimumSize(QSize(26, 26));
        btnMinimize->setMaximumSize(QSize(26, 26));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/minimize.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnMinimize->setIcon(icon);
        btnMinimize->setIconSize(QSize(20, 20));

        horizontalLayout_2->addWidget(btnMinimize);

        btnExit = new QPushButton(widget);
        btnExit->setObjectName("btnExit");
        btnExit->setMinimumSize(QSize(26, 26));
        btnExit->setMaximumSize(QSize(26, 26));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/logout.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnExit->setIcon(icon1);
        btnExit->setIconSize(QSize(20, 20));

        horizontalLayout_2->addWidget(btnExit);


        verticalLayout->addWidget(widget);

        widget_3 = new QWidget(DlgGoodsList);
        widget_3->setObjectName("widget_3");
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(widget_3);
        label_2->setObjectName("label_2");

        horizontalLayout_3->addWidget(label_2);

        chName = new QCheckBox(widget_3);
        chName->setObjectName("chName");
        chName->setFocusPolicy(Qt::NoFocus);
        chName->setChecked(true);

        horizontalLayout_3->addWidget(chName);

        chScancode = new QCheckBox(widget_3);
        chScancode->setObjectName("chScancode");
        chScancode->setFocusPolicy(Qt::NoFocus);
        chScancode->setChecked(true);

        horizontalLayout_3->addWidget(chScancode);

        horizontalSpacer_3 = new QSpacerItem(653, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout->addWidget(widget_3);

        tbl = new C5ClearTableWidget(DlgGoodsList);
        if (tbl->columnCount() < 7)
            tbl->setColumnCount(7);
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
        tbl->setObjectName("tbl");
        tbl->setFocusPolicy(Qt::NoFocus);
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setSelectionMode(QAbstractItemView::SingleSelection);
        tbl->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout->addWidget(tbl);

        widget_2 = new QWidget(DlgGoodsList);
        widget_2->setObjectName("widget_2");
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(573, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        lbTotalRetail = new QLabel(widget_2);
        lbTotalRetail->setObjectName("lbTotalRetail");

        horizontalLayout->addWidget(lbTotalRetail);

        leTotalRetail = new C5LineEdit(widget_2);
        leTotalRetail->setObjectName("leTotalRetail");
        leTotalRetail->setMaximumSize(QSize(200, 16777215));
        leTotalRetail->setFocusPolicy(Qt::NoFocus);

        horizontalLayout->addWidget(leTotalRetail);


        verticalLayout->addWidget(widget_2);

        QWidget::setTabOrder(leSearch, tbl);
        QWidget::setTabOrder(tbl, chName);
        QWidget::setTabOrder(chName, chScancode);
        QWidget::setTabOrder(chScancode, leTotalRetail);

        retranslateUi(DlgGoodsList);

        QMetaObject::connectSlotsByName(DlgGoodsList);
    } // setupUi

    void retranslateUi(QDialog *DlgGoodsList)
    {
        DlgGoodsList->setWindowTitle(QCoreApplication::translate("DlgGoodsList", "Goods list", nullptr));
        btnMinimize->setText(QString());
        btnExit->setText(QString());
        label_2->setText(QCoreApplication::translate("DlgGoodsList", "Searchi in", nullptr));
        chName->setText(QCoreApplication::translate("DlgGoodsList", "Name", nullptr));
        chScancode->setText(QCoreApplication::translate("DlgGoodsList", "Scancode", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("DlgGoodsList", "Code", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("DlgGoodsList", "Group", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("DlgGoodsList", "Scancode", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("DlgGoodsList", "Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("DlgGoodsList", "Qty", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tbl->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("DlgGoodsList", "Retail price", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tbl->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("DlgGoodsList", "Wholesale price", nullptr));
        lbTotalRetail->setText(QCoreApplication::translate("DlgGoodsList", "Total retail", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgGoodsList: public Ui_DlgGoodsList {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGGOODSLIST_H
