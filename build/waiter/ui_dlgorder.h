/********************************************************************************
** Form generated from reading UI file 'dlgorder.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGORDER_H
#define UI_DLGORDER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DlgOrder
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *widget_4;
    QVBoxLayout *verticalLayout_3;
    QWidget *widget_6;
    QVBoxLayout *verticalLayout_5;
    QWidget *widget_5;
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget_7;
    QVBoxLayout *verticalLayout_4;
    QTableWidget *tblPart1;
    QTableWidget *tblPart2;
    QWidget *widget_8;
    QVBoxLayout *verticalLayout_6;
    QPushButton *btnChangeMenu;
    QSpacerItem *verticalSpacer;
    QPushButton *btnTypeScrollUp;
    QPushButton *btnTypeScrollDown;
    QWidget *widget_9;
    QHBoxLayout *horizontalLayout_3;
    QTableWidget *tblDishes;
    QWidget *widget_10;
    QVBoxLayout *verticalLayout_8;
    QSpacerItem *verticalSpacer_3;
    QPushButton *btnDishScrollUp;
    QPushButton *btnDishScrollDown;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QGridLayout *gridLayout;
    QPushButton *btnPlus1;
    QPushButton *btnPlus05;
    QPushButton *btnMin1;
    QPushButton *btnMin05;
    QPushButton *btnCustom;
    QPushButton *btnVoid;
    QPushButton *btnPrintService;
    QWidget *widget_3;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *tblOrder;

    void setupUi(QDialog *DlgOrder)
    {
        if (DlgOrder->objectName().isEmpty())
            DlgOrder->setObjectName(QStringLiteral("DlgOrder"));
        DlgOrder->resize(1027, 688);
        horizontalLayout = new QHBoxLayout(DlgOrder);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        widget_4 = new QWidget(DlgOrder);
        widget_4->setObjectName(QStringLiteral("widget_4"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(2);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_4->sizePolicy().hasHeightForWidth());
        widget_4->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(widget_4);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        widget_6 = new QWidget(widget_4);
        widget_6->setObjectName(QStringLiteral("widget_6"));
        verticalLayout_5 = new QVBoxLayout(widget_6);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        widget_5 = new QWidget(widget_6);
        widget_5->setObjectName(QStringLiteral("widget_5"));
        widget_5->setMaximumSize(QSize(16777215, 220));
        horizontalLayout_2 = new QHBoxLayout(widget_5);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        widget_7 = new QWidget(widget_5);
        widget_7->setObjectName(QStringLiteral("widget_7"));
        widget_7->setMaximumSize(QSize(165, 16777215));
        verticalLayout_4 = new QVBoxLayout(widget_7);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        tblPart1 = new QTableWidget(widget_7);
        if (tblPart1->columnCount() < 2)
            tblPart1->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tblPart1->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tblPart1->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        if (tblPart1->rowCount() < 3)
            tblPart1->setRowCount(3);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tblPart1->setVerticalHeaderItem(0, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tblPart1->setVerticalHeaderItem(1, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tblPart1->setVerticalHeaderItem(2, __qtablewidgetitem4);
        tblPart1->setObjectName(QStringLiteral("tblPart1"));
        tblPart1->setMaximumSize(QSize(165, 16777215));
        tblPart1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblPart1->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblPart1->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblPart1->setSelectionMode(QAbstractItemView::NoSelection);
        tblPart1->horizontalHeader()->setVisible(false);
        tblPart1->horizontalHeader()->setDefaultSectionSize(80);
        tblPart1->verticalHeader()->setVisible(false);
        tblPart1->verticalHeader()->setDefaultSectionSize(60);

        verticalLayout_4->addWidget(tblPart1);


        horizontalLayout_2->addWidget(widget_7);

        tblPart2 = new QTableWidget(widget_5);
        if (tblPart2->columnCount() < 4)
            tblPart2->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tblPart2->setHorizontalHeaderItem(0, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tblPart2->setHorizontalHeaderItem(1, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tblPart2->setHorizontalHeaderItem(2, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tblPart2->setHorizontalHeaderItem(3, __qtablewidgetitem8);
        if (tblPart2->rowCount() < 4)
            tblPart2->setRowCount(4);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tblPart2->setVerticalHeaderItem(0, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tblPart2->setVerticalHeaderItem(1, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tblPart2->setVerticalHeaderItem(2, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tblPart2->setVerticalHeaderItem(3, __qtablewidgetitem12);
        tblPart2->setObjectName(QStringLiteral("tblPart2"));
        tblPart2->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblPart2->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblPart2->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblPart2->setSelectionMode(QAbstractItemView::NoSelection);
        tblPart2->horizontalHeader()->setVisible(false);
        tblPart2->verticalHeader()->setVisible(false);

        horizontalLayout_2->addWidget(tblPart2);

        widget_8 = new QWidget(widget_5);
        widget_8->setObjectName(QStringLiteral("widget_8"));
        verticalLayout_6 = new QVBoxLayout(widget_8);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        btnChangeMenu = new QPushButton(widget_8);
        btnChangeMenu->setObjectName(QStringLiteral("btnChangeMenu"));
        btnChangeMenu->setMinimumSize(QSize(50, 50));
        btnChangeMenu->setMaximumSize(QSize(50, 50));
        QIcon icon;
        icon.addFile(QStringLiteral(":/menu.PNG"), QSize(), QIcon::Normal, QIcon::Off);
        btnChangeMenu->setIcon(icon);
        btnChangeMenu->setIconSize(QSize(32, 32));

        verticalLayout_6->addWidget(btnChangeMenu);

        verticalSpacer = new QSpacerItem(20, 49, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_6->addItem(verticalSpacer);

        btnTypeScrollUp = new QPushButton(widget_8);
        btnTypeScrollUp->setObjectName(QStringLiteral("btnTypeScrollUp"));
        btnTypeScrollUp->setMinimumSize(QSize(50, 50));
        btnTypeScrollUp->setMaximumSize(QSize(50, 50));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/up.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnTypeScrollUp->setIcon(icon1);
        btnTypeScrollUp->setIconSize(QSize(32, 32));

        verticalLayout_6->addWidget(btnTypeScrollUp);

        btnTypeScrollDown = new QPushButton(widget_8);
        btnTypeScrollDown->setObjectName(QStringLiteral("btnTypeScrollDown"));
        btnTypeScrollDown->setMinimumSize(QSize(50, 50));
        btnTypeScrollDown->setMaximumSize(QSize(50, 50));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/down.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnTypeScrollDown->setIcon(icon2);
        btnTypeScrollDown->setIconSize(QSize(32, 32));

        verticalLayout_6->addWidget(btnTypeScrollDown);


        horizontalLayout_2->addWidget(widget_8);


        verticalLayout_5->addWidget(widget_5);

        widget_9 = new QWidget(widget_6);
        widget_9->setObjectName(QStringLiteral("widget_9"));
        horizontalLayout_3 = new QHBoxLayout(widget_9);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        tblDishes = new QTableWidget(widget_9);
        tblDishes->setObjectName(QStringLiteral("tblDishes"));
        tblDishes->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblDishes->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblDishes->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblDishes->setSelectionMode(QAbstractItemView::NoSelection);
        tblDishes->horizontalHeader()->setVisible(false);
        tblDishes->verticalHeader()->setVisible(false);

        horizontalLayout_3->addWidget(tblDishes);

        widget_10 = new QWidget(widget_9);
        widget_10->setObjectName(QStringLiteral("widget_10"));
        verticalLayout_8 = new QVBoxLayout(widget_10);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        verticalSpacer_3 = new QSpacerItem(20, 105, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_8->addItem(verticalSpacer_3);

        btnDishScrollUp = new QPushButton(widget_10);
        btnDishScrollUp->setObjectName(QStringLiteral("btnDishScrollUp"));
        btnDishScrollUp->setMinimumSize(QSize(50, 50));
        btnDishScrollUp->setMaximumSize(QSize(50, 50));
        btnDishScrollUp->setIcon(icon1);
        btnDishScrollUp->setIconSize(QSize(32, 32));

        verticalLayout_8->addWidget(btnDishScrollUp);

        btnDishScrollDown = new QPushButton(widget_10);
        btnDishScrollDown->setObjectName(QStringLiteral("btnDishScrollDown"));
        btnDishScrollDown->setMinimumSize(QSize(50, 50));
        btnDishScrollDown->setMaximumSize(QSize(50, 50));
        btnDishScrollDown->setIcon(icon2);
        btnDishScrollDown->setIconSize(QSize(32, 32));

        verticalLayout_8->addWidget(btnDishScrollDown);


        horizontalLayout_3->addWidget(widget_10);


        verticalLayout_5->addWidget(widget_9);


        verticalLayout_3->addWidget(widget_6);


        horizontalLayout->addWidget(widget_4);

        widget_2 = new QWidget(DlgOrder);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setMinimumSize(QSize(250, 0));
        widget_2->setMaximumSize(QSize(250, 16777215));
        verticalLayout = new QVBoxLayout(widget_2);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(widget_2);
        widget->setObjectName(QStringLiteral("widget"));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        btnPlus1 = new QPushButton(widget);
        btnPlus1->setObjectName(QStringLiteral("btnPlus1"));
        btnPlus1->setMinimumSize(QSize(50, 50));
        btnPlus1->setMaximumSize(QSize(50, 50));

        gridLayout->addWidget(btnPlus1, 0, 0, 1, 1);

        btnPlus05 = new QPushButton(widget);
        btnPlus05->setObjectName(QStringLiteral("btnPlus05"));
        btnPlus05->setMinimumSize(QSize(50, 50));
        btnPlus05->setMaximumSize(QSize(50, 50));

        gridLayout->addWidget(btnPlus05, 0, 1, 1, 1);

        btnMin1 = new QPushButton(widget);
        btnMin1->setObjectName(QStringLiteral("btnMin1"));
        btnMin1->setMinimumSize(QSize(50, 50));
        btnMin1->setMaximumSize(QSize(50, 50));

        gridLayout->addWidget(btnMin1, 0, 2, 1, 1);

        btnMin05 = new QPushButton(widget);
        btnMin05->setObjectName(QStringLiteral("btnMin05"));
        btnMin05->setMinimumSize(QSize(50, 50));
        btnMin05->setMaximumSize(QSize(50, 50));

        gridLayout->addWidget(btnMin05, 0, 3, 1, 1);

        btnCustom = new QPushButton(widget);
        btnCustom->setObjectName(QStringLiteral("btnCustom"));
        btnCustom->setMinimumSize(QSize(50, 50));
        btnCustom->setMaximumSize(QSize(50, 50));

        gridLayout->addWidget(btnCustom, 1, 0, 1, 1);

        btnVoid = new QPushButton(widget);
        btnVoid->setObjectName(QStringLiteral("btnVoid"));
        btnVoid->setMinimumSize(QSize(50, 50));
        btnVoid->setMaximumSize(QSize(50, 50));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/recycle.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnVoid->setIcon(icon3);
        btnVoid->setIconSize(QSize(32, 32));

        gridLayout->addWidget(btnVoid, 1, 1, 1, 1);

        btnPrintService = new QPushButton(widget);
        btnPrintService->setObjectName(QStringLiteral("btnPrintService"));
        btnPrintService->setMinimumSize(QSize(50, 50));
        btnPrintService->setMaximumSize(QSize(50, 50));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/print_receipt.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnPrintService->setIcon(icon4);
        btnPrintService->setIconSize(QSize(32, 32));

        gridLayout->addWidget(btnPrintService, 1, 3, 1, 1);


        verticalLayout->addWidget(widget);

        widget_3 = new QWidget(widget_2);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        verticalLayout_2 = new QVBoxLayout(widget_3);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        tblOrder = new QTableWidget(widget_3);
        if (tblOrder->columnCount() < 1)
            tblOrder->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tblOrder->setHorizontalHeaderItem(0, __qtablewidgetitem13);
        tblOrder->setObjectName(QStringLiteral("tblOrder"));
        tblOrder->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblOrder->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblOrder->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblOrder->setSelectionMode(QAbstractItemView::SingleSelection);
        tblOrder->setSelectionBehavior(QAbstractItemView::SelectRows);
        tblOrder->horizontalHeader()->setVisible(false);
        tblOrder->verticalHeader()->setVisible(false);

        verticalLayout_2->addWidget(tblOrder);


        verticalLayout->addWidget(widget_3);


        horizontalLayout->addWidget(widget_2);


        retranslateUi(DlgOrder);

        QMetaObject::connectSlotsByName(DlgOrder);
    } // setupUi

    void retranslateUi(QDialog *DlgOrder)
    {
        DlgOrder->setWindowTitle(QApplication::translate("DlgOrder", "Dialog", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tblPart1->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("DlgOrder", "New Column", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tblPart1->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("DlgOrder", "New Column", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tblPart1->verticalHeaderItem(0);
        ___qtablewidgetitem2->setText(QApplication::translate("DlgOrder", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tblPart1->verticalHeaderItem(1);
        ___qtablewidgetitem3->setText(QApplication::translate("DlgOrder", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tblPart1->verticalHeaderItem(2);
        ___qtablewidgetitem4->setText(QApplication::translate("DlgOrder", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tblPart2->horizontalHeaderItem(0);
        ___qtablewidgetitem5->setText(QApplication::translate("DlgOrder", "New Column", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tblPart2->horizontalHeaderItem(1);
        ___qtablewidgetitem6->setText(QApplication::translate("DlgOrder", "New Column", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tblPart2->horizontalHeaderItem(2);
        ___qtablewidgetitem7->setText(QApplication::translate("DlgOrder", "New Column", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tblPart2->horizontalHeaderItem(3);
        ___qtablewidgetitem8->setText(QApplication::translate("DlgOrder", "New Column", nullptr));
        QTableWidgetItem *___qtablewidgetitem9 = tblPart2->verticalHeaderItem(0);
        ___qtablewidgetitem9->setText(QApplication::translate("DlgOrder", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tblPart2->verticalHeaderItem(1);
        ___qtablewidgetitem10->setText(QApplication::translate("DlgOrder", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tblPart2->verticalHeaderItem(2);
        ___qtablewidgetitem11->setText(QApplication::translate("DlgOrder", "New Row", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tblPart2->verticalHeaderItem(3);
        ___qtablewidgetitem12->setText(QApplication::translate("DlgOrder", "New Row", nullptr));
        btnChangeMenu->setText(QString());
        btnTypeScrollUp->setText(QString());
        btnTypeScrollDown->setText(QString());
        btnDishScrollUp->setText(QString());
        btnDishScrollDown->setText(QString());
        btnPlus1->setText(QApplication::translate("DlgOrder", "+1", nullptr));
        btnPlus05->setText(QApplication::translate("DlgOrder", "+0.5", nullptr));
        btnMin1->setText(QApplication::translate("DlgOrder", "-1", nullptr));
        btnMin05->setText(QApplication::translate("DlgOrder", "-0.5", nullptr));
        btnCustom->setText(QApplication::translate("DlgOrder", "+/-", nullptr));
        btnVoid->setText(QString());
        btnPrintService->setText(QString());
        QTableWidgetItem *___qtablewidgetitem13 = tblOrder->horizontalHeaderItem(0);
        ___qtablewidgetitem13->setText(QApplication::translate("DlgOrder", "New Column", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgOrder: public Ui_DlgOrder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGORDER_H
