/********************************************************************************
** Form generated from reading UI file 'c5gridgilter.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C5GRIDGILTER_H
#define UI_C5GRIDGILTER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_C5GridGilter
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab1;
    QHBoxLayout *horizontalLayout_2;
    QWidget *wFilter;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *vl;
    QWidget *tab2;
    QHBoxLayout *horizontalLayout_4;
    QListWidget *lvColumns;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOK;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *C5GridGilter)
    {
        if (C5GridGilter->objectName().isEmpty())
            C5GridGilter->setObjectName("C5GridGilter");
        C5GridGilter->resize(581, 504);
        verticalLayout = new QVBoxLayout(C5GridGilter);
        verticalLayout->setObjectName("verticalLayout");
        tabWidget = new QTabWidget(C5GridGilter);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setFocusPolicy(Qt::NoFocus);
        tab1 = new QWidget();
        tab1->setObjectName("tab1");
        horizontalLayout_2 = new QHBoxLayout(tab1);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        wFilter = new QWidget(tab1);
        wFilter->setObjectName("wFilter");
        horizontalLayout_3 = new QHBoxLayout(wFilter);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        vl = new QVBoxLayout();
        vl->setObjectName("vl");

        horizontalLayout_3->addLayout(vl);


        horizontalLayout_2->addWidget(wFilter);

        tabWidget->addTab(tab1, QString());
        tab2 = new QWidget();
        tab2->setObjectName("tab2");
        horizontalLayout_4 = new QHBoxLayout(tab2);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        lvColumns = new QListWidget(tab2);
        lvColumns->setObjectName("lvColumns");
        lvColumns->setFocusPolicy(Qt::ClickFocus);

        horizontalLayout_4->addWidget(lvColumns);

        tabWidget->addTab(tab2, QString());

        verticalLayout->addWidget(tabWidget);

        widget = new QWidget(C5GridGilter);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(111, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnOK = new QPushButton(widget);
        btnOK->setObjectName("btnOK");
        btnOK->setFocusPolicy(Qt::ClickFocus);

        horizontalLayout->addWidget(btnOK);

        btnCancel = new QPushButton(widget);
        btnCancel->setObjectName("btnCancel");
        btnCancel->setFocusPolicy(Qt::ClickFocus);

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(111, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget);


        retranslateUi(C5GridGilter);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(C5GridGilter);
    } // setupUi

    void retranslateUi(QDialog *C5GridGilter)
    {
        C5GridGilter->setWindowTitle(QCoreApplication::translate("C5GridGilter", "Report parameters", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab1), QCoreApplication::translate("C5GridGilter", "Parameters", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab2), QCoreApplication::translate("C5GridGilter", "Show columns", nullptr));
        btnOK->setText(QCoreApplication::translate("C5GridGilter", "Apply", nullptr));
        btnCancel->setText(QCoreApplication::translate("C5GridGilter", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class C5GridGilter: public Ui_C5GridGilter {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5GRIDGILTER_H
