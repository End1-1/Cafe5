/********************************************************************************
** Form generated from reading UI file 'c5tempsale.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C5TEMPSALE_H
#define UI_C5TEMPSALE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_C5TempSale
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnOpen;
    QPushButton *btnTrash;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnExit;
    QTableWidget *tbl;

    void setupUi(QDialog *C5TempSale)
    {
        if (C5TempSale->objectName().isEmpty())
            C5TempSale->setObjectName("C5TempSale");
        C5TempSale->resize(879, 517);
        gridLayout = new QGridLayout(C5TempSale);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setHorizontalSpacing(0);
        gridLayout->setVerticalSpacing(2);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(C5TempSale);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        widget = new QWidget(frame);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btnOpen = new QPushButton(widget);
        btnOpen->setObjectName("btnOpen");
        btnOpen->setMinimumSize(QSize(30, 30));
        btnOpen->setMaximumSize(QSize(30, 30));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/folder_open.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnOpen->setIcon(icon);

        horizontalLayout->addWidget(btnOpen);

        btnTrash = new QPushButton(widget);
        btnTrash->setObjectName("btnTrash");
        btnTrash->setMinimumSize(QSize(30, 30));
        btnTrash->setMaximumSize(QSize(30, 30));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnTrash->setIcon(icon1);
        btnTrash->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(btnTrash);

        horizontalSpacer = new QSpacerItem(768, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnExit = new QPushButton(widget);
        btnExit->setObjectName("btnExit");
        btnExit->setMinimumSize(QSize(30, 30));
        btnExit->setMaximumSize(QSize(30, 30));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/remove.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnExit->setIcon(icon2);

        horizontalLayout->addWidget(btnExit);


        verticalLayout->addWidget(widget);

        tbl = new QTableWidget(frame);
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
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setSelectionMode(QAbstractItemView::SingleSelection);
        tbl->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout->addWidget(tbl);


        gridLayout->addWidget(frame, 1, 0, 1, 1);


        retranslateUi(C5TempSale);

        QMetaObject::connectSlotsByName(C5TempSale);
    } // setupUi

    void retranslateUi(QDialog *C5TempSale)
    {
        C5TempSale->setWindowTitle(QCoreApplication::translate("C5TempSale", "Dialog", nullptr));
        btnOpen->setText(QString());
        btnTrash->setText(QString());
        btnExit->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("C5TempSale", "UUID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("C5TempSale", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("C5TempSale", "Time", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("C5TempSale", "Staff", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("C5TempSale", "Total", nullptr));
    } // retranslateUi

};

namespace Ui {
    class C5TempSale: public Ui_C5TempSale {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5TEMPSALE_H
