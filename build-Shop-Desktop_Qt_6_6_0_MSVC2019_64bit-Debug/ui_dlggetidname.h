/********************************************************************************
** Form generated from reading UI file 'dlggetidname.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGGETIDNAME_H
#define UI_DLGGETIDNAME_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DlgGetIDName
{
public:
    QGridLayout *gridLayout;
    QLineEdit *leFilter;
    QPushButton *btnRefresh;
    QTableWidget *tbl;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOk;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *DlgGetIDName)
    {
        if (DlgGetIDName->objectName().isEmpty())
            DlgGetIDName->setObjectName("DlgGetIDName");
        DlgGetIDName->resize(651, 430);
        gridLayout = new QGridLayout(DlgGetIDName);
        gridLayout->setObjectName("gridLayout");
        leFilter = new QLineEdit(DlgGetIDName);
        leFilter->setObjectName("leFilter");

        gridLayout->addWidget(leFilter, 0, 0, 1, 1);

        btnRefresh = new QPushButton(DlgGetIDName);
        btnRefresh->setObjectName("btnRefresh");
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btnRefresh->sizePolicy().hasHeightForWidth());
        btnRefresh->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRefresh->setIcon(icon);

        gridLayout->addWidget(btnRefresh, 0, 1, 1, 1);

        tbl = new QTableWidget(DlgGetIDName);
        if (tbl->columnCount() < 2)
            tbl->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tbl->setObjectName("tbl");
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setSelectionMode(QAbstractItemView::SingleSelection);
        tbl->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout->addWidget(tbl, 1, 0, 1, 2);

        widget = new QWidget(DlgGetIDName);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(230, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnOk = new QPushButton(widget);
        btnOk->setObjectName("btnOk");
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/checked.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnOk->setIcon(icon1);

        horizontalLayout->addWidget(btnOk);

        btnCancel = new QPushButton(widget);
        btnCancel->setObjectName("btnCancel");
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/remove.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCancel->setIcon(icon2);

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(229, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addWidget(widget, 2, 0, 1, 2);


        retranslateUi(DlgGetIDName);

        QMetaObject::connectSlotsByName(DlgGetIDName);
    } // setupUi

    void retranslateUi(QDialog *DlgGetIDName)
    {
        DlgGetIDName->setWindowTitle(QString());
        btnRefresh->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("DlgGetIDName", "Code", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("DlgGetIDName", "Name", nullptr));
        btnOk->setText(QCoreApplication::translate("DlgGetIDName", "OK", nullptr));
        btnCancel->setText(QCoreApplication::translate("DlgGetIDName", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgGetIDName: public Ui_DlgGetIDName {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGGETIDNAME_H
