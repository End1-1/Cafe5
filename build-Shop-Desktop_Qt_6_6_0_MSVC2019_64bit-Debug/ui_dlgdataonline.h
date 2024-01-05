/********************************************************************************
** Form generated from reading UI file 'dlgdataonline.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGDATAONLINE_H
#define UI_DLGDATAONLINE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_DlgDataOnline
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    C5LineEdit *leFilter;
    QPushButton *btnRefreshCache;
    QPushButton *btnCheck;
    QWidget *widget_2;
    QHBoxLayout *hl;
    QTableView *tableView;

    void setupUi(QDialog *DlgDataOnline)
    {
        if (DlgDataOnline->objectName().isEmpty())
            DlgDataOnline->setObjectName("DlgDataOnline");
        DlgDataOnline->resize(800, 410);
        DlgDataOnline->setMinimumSize(QSize(800, 0));
        verticalLayout = new QVBoxLayout(DlgDataOnline);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(DlgDataOnline);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(1);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(1, 1, 1, 1);
        leFilter = new C5LineEdit(widget);
        leFilter->setObjectName("leFilter");

        horizontalLayout->addWidget(leFilter);

        btnRefreshCache = new QPushButton(widget);
        btnRefreshCache->setObjectName("btnRefreshCache");
        btnRefreshCache->setMinimumSize(QSize(25, 25));
        btnRefreshCache->setMaximumSize(QSize(25, 25));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRefreshCache->setIcon(icon);

        horizontalLayout->addWidget(btnRefreshCache);

        btnCheck = new QPushButton(widget);
        btnCheck->setObjectName("btnCheck");
        btnCheck->setMinimumSize(QSize(25, 25));
        btnCheck->setMaximumSize(QSize(25, 25));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/checked.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCheck->setIcon(icon1);

        horizontalLayout->addWidget(btnCheck);


        verticalLayout->addWidget(widget);

        widget_2 = new QWidget(DlgDataOnline);
        widget_2->setObjectName("widget_2");
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(5);
        sizePolicy.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy);
        hl = new QHBoxLayout(widget_2);
        hl->setSpacing(1);
        hl->setObjectName("hl");
        hl->setContentsMargins(1, 1, 1, 1);
        tableView = new QTableView(widget_2);
        tableView->setObjectName("tableView");

        hl->addWidget(tableView);


        verticalLayout->addWidget(widget_2);


        retranslateUi(DlgDataOnline);

        QMetaObject::connectSlotsByName(DlgDataOnline);
    } // setupUi

    void retranslateUi(QDialog *DlgDataOnline)
    {
        DlgDataOnline->setWindowTitle(QCoreApplication::translate("DlgDataOnline", "Dialog", nullptr));
        btnRefreshCache->setText(QString());
        btnCheck->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class DlgDataOnline: public Ui_DlgDataOnline {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGDATAONLINE_H
