/********************************************************************************
** Form generated from reading UI file 'searchitems.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SEARCHITEMS_H
#define UI_SEARCHITEMS_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <c5cleartablewidget.h>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_SearchItems
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    C5LineEdit *leCode;
    QToolButton *btnSearch;
    QToolButton *btnExit;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_4;
    C5ClearTableWidget *tbl;
    C5ClearTableWidget *tblReserve;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QToolButton *btnReserve;
    QToolButton *btnViewReservations;
    QToolButton *btnViewAllReservations;
    QToolButton *btnEditReserve;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label;
    C5LineEdit *leTotalQty;

    void setupUi(QDialog *SearchItems)
    {
        if (SearchItems->objectName().isEmpty())
            SearchItems->setObjectName("SearchItems");
        SearchItems->resize(866, 682);
        verticalLayout = new QVBoxLayout(SearchItems);
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        widget = new QWidget(SearchItems);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        leCode = new C5LineEdit(widget);
        leCode->setObjectName("leCode");

        horizontalLayout->addWidget(leCode);

        btnSearch = new QToolButton(widget);
        btnSearch->setObjectName("btnSearch");
        btnSearch->setMinimumSize(QSize(26, 26));
        btnSearch->setMaximumSize(QSize(26, 26));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/search.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnSearch->setIcon(icon);
        btnSearch->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(btnSearch);

        btnExit = new QToolButton(widget);
        btnExit->setObjectName("btnExit");
        btnExit->setMinimumSize(QSize(26, 26));
        btnExit->setMaximumSize(QSize(26, 26));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/logout.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnExit->setIcon(icon1);
        btnExit->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(btnExit);


        verticalLayout->addWidget(widget);

        widget_4 = new QWidget(SearchItems);
        widget_4->setObjectName("widget_4");
        horizontalLayout_4 = new QHBoxLayout(widget_4);
        horizontalLayout_4->setSpacing(2);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        tbl = new C5ClearTableWidget(widget_4);
        if (tbl->columnCount() < 10)
            tbl->setColumnCount(10);
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
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(9, __qtablewidgetitem9);
        tbl->setObjectName("tbl");
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setSelectionMode(QAbstractItemView::SingleSelection);
        tbl->setSelectionBehavior(QAbstractItemView::SelectRows);

        horizontalLayout_4->addWidget(tbl);

        tblReserve = new C5ClearTableWidget(widget_4);
        if (tblReserve->columnCount() < 9)
            tblReserve->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tblReserve->setHorizontalHeaderItem(0, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tblReserve->setHorizontalHeaderItem(1, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tblReserve->setHorizontalHeaderItem(2, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tblReserve->setHorizontalHeaderItem(3, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tblReserve->setHorizontalHeaderItem(4, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tblReserve->setHorizontalHeaderItem(5, __qtablewidgetitem15);
        QTableWidgetItem *__qtablewidgetitem16 = new QTableWidgetItem();
        tblReserve->setHorizontalHeaderItem(6, __qtablewidgetitem16);
        QTableWidgetItem *__qtablewidgetitem17 = new QTableWidgetItem();
        tblReserve->setHorizontalHeaderItem(7, __qtablewidgetitem17);
        QTableWidgetItem *__qtablewidgetitem18 = new QTableWidgetItem();
        tblReserve->setHorizontalHeaderItem(8, __qtablewidgetitem18);
        tblReserve->setObjectName("tblReserve");
        tblReserve->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblReserve->setSelectionMode(QAbstractItemView::SingleSelection);
        tblReserve->setSelectionBehavior(QAbstractItemView::SelectRows);

        horizontalLayout_4->addWidget(tblReserve);


        verticalLayout->addWidget(widget_4);

        widget_3 = new QWidget(SearchItems);
        widget_3->setObjectName("widget_3");
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setSpacing(2);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        btnReserve = new QToolButton(widget_3);
        btnReserve->setObjectName("btnReserve");
        btnReserve->setMinimumSize(QSize(0, 0));
        btnReserve->setMaximumSize(QSize(16777215, 16777215));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/reserve.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnReserve->setIcon(icon2);
        btnReserve->setIconSize(QSize(35, 32));
        btnReserve->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(btnReserve);

        btnViewReservations = new QToolButton(widget_3);
        btnViewReservations->setObjectName("btnViewReservations");
        btnViewReservations->setMinimumSize(QSize(0, 0));
        btnViewReservations->setMaximumSize(QSize(16777215, 16777215));
        btnViewReservations->setIcon(icon2);
        btnViewReservations->setIconSize(QSize(35, 32));
        btnViewReservations->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(btnViewReservations);

        btnViewAllReservations = new QToolButton(widget_3);
        btnViewAllReservations->setObjectName("btnViewAllReservations");
        btnViewAllReservations->setMinimumSize(QSize(0, 0));
        btnViewAllReservations->setMaximumSize(QSize(16777215, 16777215));
        btnViewAllReservations->setIcon(icon2);
        btnViewAllReservations->setIconSize(QSize(35, 32));
        btnViewAllReservations->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(btnViewAllReservations);

        btnEditReserve = new QToolButton(widget_3);
        btnEditReserve->setObjectName("btnEditReserve");
        btnEditReserve->setMinimumSize(QSize(0, 0));
        btnEditReserve->setMaximumSize(QSize(16777215, 16777215));
        btnEditReserve->setIcon(icon2);
        btnEditReserve->setIconSize(QSize(35, 32));
        btnEditReserve->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(btnEditReserve);

        horizontalSpacer_2 = new QSpacerItem(554, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        label = new QLabel(widget_3);
        label->setObjectName("label");

        horizontalLayout_3->addWidget(label);

        leTotalQty = new C5LineEdit(widget_3);
        leTotalQty->setObjectName("leTotalQty");
        leTotalQty->setMaximumSize(QSize(100, 16777215));
        leTotalQty->setReadOnly(true);

        horizontalLayout_3->addWidget(leTotalQty);


        verticalLayout->addWidget(widget_3);


        retranslateUi(SearchItems);

        QMetaObject::connectSlotsByName(SearchItems);
    } // setupUi

    void retranslateUi(QDialog *SearchItems)
    {
        SearchItems->setWindowTitle(QCoreApplication::translate("SearchItems", "Search", nullptr));
        btnSearch->setText(QString());
        btnExit->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("SearchItems", "Store", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("SearchItems", "Goods", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("SearchItems", "Code", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("SearchItems", "Unit", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("SearchItems", "Retail price", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tbl->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("SearchItems", "Whosale price", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tbl->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("SearchItems", "Qty", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tbl->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("SearchItems", "Reserved", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tbl->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("SearchItems", "Store Code", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tbl->horizontalHeaderItem(9);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("SearchItems", "Goods Code", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tblReserve->horizontalHeaderItem(0);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("SearchItems", "ID", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tblReserve->horizontalHeaderItem(1);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("SearchItems", "State", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tblReserve->horizontalHeaderItem(2);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("SearchItems", "Date", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = tblReserve->horizontalHeaderItem(3);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("SearchItems", "Source", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = tblReserve->horizontalHeaderItem(4);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("SearchItems", "Store", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = tblReserve->horizontalHeaderItem(5);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("SearchItems", "Name", nullptr));
        QTableWidgetItem *___qtablewidgetitem16 = tblReserve->horizontalHeaderItem(6);
        ___qtablewidgetitem16->setText(QCoreApplication::translate("SearchItems", "Scancode", nullptr));
        QTableWidgetItem *___qtablewidgetitem17 = tblReserve->horizontalHeaderItem(7);
        ___qtablewidgetitem17->setText(QCoreApplication::translate("SearchItems", "Qty", nullptr));
        QTableWidgetItem *___qtablewidgetitem18 = tblReserve->horizontalHeaderItem(8);
        ___qtablewidgetitem18->setText(QCoreApplication::translate("SearchItems", "Message", nullptr));
        btnReserve->setText(QCoreApplication::translate("SearchItems", "Reserve\n"
"", nullptr));
        btnViewReservations->setText(QCoreApplication::translate("SearchItems", "View\n"
"reservations", nullptr));
        btnViewAllReservations->setText(QCoreApplication::translate("SearchItems", "View all\n"
"reservations", nullptr));
        btnEditReserve->setText(QCoreApplication::translate("SearchItems", "Edit\n"
"reservation", nullptr));
        label->setText(QCoreApplication::translate("SearchItems", "Total quantity", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SearchItems: public Ui_SearchItems {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SEARCHITEMS_H
