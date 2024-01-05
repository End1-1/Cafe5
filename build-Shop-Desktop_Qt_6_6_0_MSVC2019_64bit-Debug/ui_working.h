/********************************************************************************
** Form generated from reading UI file 'working.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WORKING_H
#define UI_WORKING_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Working
{
public:
    QAction *actionnewPreorder;
    QGridLayout *gridLayout;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnNewRetail;
    QPushButton *btnNewWhosale;
    QPushButton *btnGoodsList;
    QPushButton *btnOpenDraft;
    QPushButton *btnWriteOrder;
    QPushButton *btnManualTax;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnHelp;
    QPushButton *btnPreorder;
    QPushButton *btnCostumerDisplay;
    QPushButton *btnGiftCard;
    QPushButton *btnSalesReport;
    QPushButton *btnGoodsMovement;
    QPushButton *btnDbConnection;
    QPushButton *btnColumns;
    QPushButton *btnClientConfigQR;
    QPushButton *btnServerSettings;
    QPushButton *btnMinimize;
    QPushButton *btnCloseApplication;
    QTabWidget *tab;

    void setupUi(QWidget *Working)
    {
        if (Working->objectName().isEmpty())
            Working->setObjectName("Working");
        Working->resize(986, 681);
        actionnewPreorder = new QAction(Working);
        actionnewPreorder->setObjectName("actionnewPreorder");
        gridLayout = new QGridLayout(Working);
        gridLayout->setSpacing(2);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(1, 1, 1, 1);
        widget = new QWidget(Working);
        widget->setObjectName("widget");
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setHorizontalSpacing(3);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        widget_2 = new QWidget(widget);
        widget_2->setObjectName("widget_2");
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btnNewRetail = new QPushButton(widget_2);
        btnNewRetail->setObjectName("btnNewRetail");
        btnNewRetail->setMinimumSize(QSize(36, 36));
        btnNewRetail->setMaximumSize(QSize(36, 36));
        btnNewRetail->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/new.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnNewRetail->setIcon(icon);
        btnNewRetail->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnNewRetail);

        btnNewWhosale = new QPushButton(widget_2);
        btnNewWhosale->setObjectName("btnNewWhosale");
        btnNewWhosale->setMinimumSize(QSize(36, 36));
        btnNewWhosale->setMaximumSize(QSize(36, 36));
        btnNewWhosale->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/whosale.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnNewWhosale->setIcon(icon1);
        btnNewWhosale->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnNewWhosale);

        btnGoodsList = new QPushButton(widget_2);
        btnGoodsList->setObjectName("btnGoodsList");
        btnGoodsList->setMinimumSize(QSize(36, 36));
        btnGoodsList->setMaximumSize(QSize(36, 36));
        btnGoodsList->setFocusPolicy(Qt::NoFocus);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/goodslist.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnGoodsList->setIcon(icon2);
        btnGoodsList->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnGoodsList);

        btnOpenDraft = new QPushButton(widget_2);
        btnOpenDraft->setObjectName("btnOpenDraft");
        btnOpenDraft->setMinimumSize(QSize(36, 36));
        btnOpenDraft->setMaximumSize(QSize(36, 36));
        btnOpenDraft->setFocusPolicy(Qt::NoFocus);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/draft.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnOpenDraft->setIcon(icon3);
        btnOpenDraft->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnOpenDraft);

        btnWriteOrder = new QPushButton(widget_2);
        btnWriteOrder->setObjectName("btnWriteOrder");
        btnWriteOrder->setMinimumSize(QSize(36, 36));
        btnWriteOrder->setMaximumSize(QSize(36, 36));
        btnWriteOrder->setFocusPolicy(Qt::NoFocus);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnWriteOrder->setIcon(icon4);
        btnWriteOrder->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnWriteOrder);

        btnManualTax = new QPushButton(widget_2);
        btnManualTax->setObjectName("btnManualTax");
        btnManualTax->setMinimumSize(QSize(36, 36));
        btnManualTax->setMaximumSize(QSize(36, 36));
        btnManualTax->setFocusPolicy(Qt::NoFocus);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnManualTax->setIcon(icon5);
        btnManualTax->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnManualTax);

        horizontalSpacer = new QSpacerItem(949, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnHelp = new QPushButton(widget_2);
        btnHelp->setObjectName("btnHelp");
        btnHelp->setMinimumSize(QSize(36, 36));
        btnHelp->setMaximumSize(QSize(36, 36));
        btnHelp->setFocusPolicy(Qt::NoFocus);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/help.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnHelp->setIcon(icon6);
        btnHelp->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnHelp);

        btnPreorder = new QPushButton(widget_2);
        btnPreorder->setObjectName("btnPreorder");
        btnPreorder->setMinimumSize(QSize(36, 36));
        btnPreorder->setMaximumSize(QSize(36, 36));
        btnPreorder->setFocusPolicy(Qt::NoFocus);
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/preorder.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnPreorder->setIcon(icon7);
        btnPreorder->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnPreorder);

        btnCostumerDisplay = new QPushButton(widget_2);
        btnCostumerDisplay->setObjectName("btnCostumerDisplay");
        btnCostumerDisplay->setMinimumSize(QSize(36, 36));
        btnCostumerDisplay->setMaximumSize(QSize(36, 36));
        btnCostumerDisplay->setFocusPolicy(Qt::NoFocus);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/monitor.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCostumerDisplay->setIcon(icon8);
        btnCostumerDisplay->setIconSize(QSize(30, 30));
        btnCostumerDisplay->setCheckable(true);

        horizontalLayout->addWidget(btnCostumerDisplay);

        btnGiftCard = new QPushButton(widget_2);
        btnGiftCard->setObjectName("btnGiftCard");
        btnGiftCard->setMinimumSize(QSize(36, 36));
        btnGiftCard->setMaximumSize(QSize(36, 36));
        btnGiftCard->setFocusPolicy(Qt::NoFocus);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/present.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnGiftCard->setIcon(icon9);
        btnGiftCard->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnGiftCard);

        btnSalesReport = new QPushButton(widget_2);
        btnSalesReport->setObjectName("btnSalesReport");
        btnSalesReport->setMinimumSize(QSize(36, 36));
        btnSalesReport->setMaximumSize(QSize(36, 36));
        btnSalesReport->setFocusPolicy(Qt::NoFocus);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/calendar.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnSalesReport->setIcon(icon10);
        btnSalesReport->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnSalesReport);

        btnGoodsMovement = new QPushButton(widget_2);
        btnGoodsMovement->setObjectName("btnGoodsMovement");
        btnGoodsMovement->setMinimumSize(QSize(36, 36));
        btnGoodsMovement->setMaximumSize(QSize(36, 36));
        btnGoodsMovement->setFocusPolicy(Qt::NoFocus);
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/contract.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnGoodsMovement->setIcon(icon11);
        btnGoodsMovement->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnGoodsMovement);

        btnDbConnection = new QPushButton(widget_2);
        btnDbConnection->setObjectName("btnDbConnection");
        btnDbConnection->setMinimumSize(QSize(36, 36));
        btnDbConnection->setMaximumSize(QSize(36, 36));
        btnDbConnection->setFocusPolicy(Qt::NoFocus);
        QIcon icon12;
        icon12.addFile(QString::fromUtf8(":/configure.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnDbConnection->setIcon(icon12);
        btnDbConnection->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnDbConnection);

        btnColumns = new QPushButton(widget_2);
        btnColumns->setObjectName("btnColumns");
        btnColumns->setMinimumSize(QSize(36, 36));
        btnColumns->setMaximumSize(QSize(36, 36));
        btnColumns->setFocusPolicy(Qt::NoFocus);
        QIcon icon13;
        icon13.addFile(QString::fromUtf8(":/checkall.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnColumns->setIcon(icon13);
        btnColumns->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnColumns);

        btnClientConfigQR = new QPushButton(widget_2);
        btnClientConfigQR->setObjectName("btnClientConfigQR");
        btnClientConfigQR->setMinimumSize(QSize(36, 36));
        btnClientConfigQR->setMaximumSize(QSize(36, 36));
        btnClientConfigQR->setFocusPolicy(Qt::NoFocus);
        QIcon icon14;
        icon14.addFile(QString::fromUtf8(":/qr_code.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnClientConfigQR->setIcon(icon14);
        btnClientConfigQR->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnClientConfigQR);

        btnServerSettings = new QPushButton(widget_2);
        btnServerSettings->setObjectName("btnServerSettings");
        btnServerSettings->setMinimumSize(QSize(36, 36));
        btnServerSettings->setMaximumSize(QSize(36, 36));
        btnServerSettings->setFocusPolicy(Qt::NoFocus);
        QIcon icon15;
        icon15.addFile(QString::fromUtf8(":/wifi_off.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnServerSettings->setIcon(icon15);
        btnServerSettings->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnServerSettings);

        btnMinimize = new QPushButton(widget_2);
        btnMinimize->setObjectName("btnMinimize");
        btnMinimize->setMinimumSize(QSize(36, 36));
        btnMinimize->setMaximumSize(QSize(36, 36));
        btnMinimize->setFocusPolicy(Qt::NoFocus);
        QIcon icon16;
        icon16.addFile(QString::fromUtf8(":/minimize.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnMinimize->setIcon(icon16);
        btnMinimize->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnMinimize);

        btnCloseApplication = new QPushButton(widget_2);
        btnCloseApplication->setObjectName("btnCloseApplication");
        btnCloseApplication->setMinimumSize(QSize(36, 36));
        btnCloseApplication->setMaximumSize(QSize(36, 36));
        btnCloseApplication->setFocusPolicy(Qt::NoFocus);
        QIcon icon17;
        icon17.addFile(QString::fromUtf8(":/logout.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCloseApplication->setIcon(icon17);
        btnCloseApplication->setIconSize(QSize(30, 30));

        horizontalLayout->addWidget(btnCloseApplication);


        gridLayout_2->addWidget(widget_2, 0, 0, 1, 2);


        gridLayout->addWidget(widget, 0, 0, 1, 2);

        tab = new QTabWidget(Working);
        tab->setObjectName("tab");
        tab->setFocusPolicy(Qt::NoFocus);
        tab->setTabsClosable(true);

        gridLayout->addWidget(tab, 1, 0, 1, 1);


        retranslateUi(Working);

        QMetaObject::connectSlotsByName(Working);
    } // setupUi

    void retranslateUi(QWidget *Working)
    {
        Working->setWindowTitle(QCoreApplication::translate("Working", "Working", nullptr));
        actionnewPreorder->setText(QCoreApplication::translate("Working", "newPreorder", nullptr));
#if QT_CONFIG(shortcut)
        actionnewPreorder->setShortcut(QCoreApplication::translate("Working", "Ctrl+A", nullptr));
#endif // QT_CONFIG(shortcut)
#if QT_CONFIG(tooltip)
        btnNewRetail->setToolTip(QCoreApplication::translate("Working", "New retail", nullptr));
#endif // QT_CONFIG(tooltip)
        btnNewRetail->setText(QString());
#if QT_CONFIG(tooltip)
        btnNewWhosale->setToolTip(QCoreApplication::translate("Working", "New whosale", nullptr));
#endif // QT_CONFIG(tooltip)
        btnNewWhosale->setText(QString());
#if QT_CONFIG(tooltip)
        btnGoodsList->setToolTip(QCoreApplication::translate("Working", "Goods list", nullptr));
#endif // QT_CONFIG(tooltip)
        btnGoodsList->setText(QString());
#if QT_CONFIG(tooltip)
        btnOpenDraft->setToolTip(QCoreApplication::translate("Working", "Open draft", nullptr));
#endif // QT_CONFIG(tooltip)
        btnOpenDraft->setText(QString());
#if QT_CONFIG(tooltip)
        btnWriteOrder->setToolTip(QCoreApplication::translate("Working", "Write order", nullptr));
#endif // QT_CONFIG(tooltip)
        btnWriteOrder->setText(QString());
#if QT_CONFIG(tooltip)
        btnManualTax->setToolTip(QCoreApplication::translate("Working", "Manual tax receipt", nullptr));
#endif // QT_CONFIG(tooltip)
        btnManualTax->setText(QString());
#if QT_CONFIG(tooltip)
        btnHelp->setToolTip(QCoreApplication::translate("Working", "Help", nullptr));
#endif // QT_CONFIG(tooltip)
        btnHelp->setText(QString());
#if QT_CONFIG(tooltip)
        btnPreorder->setToolTip(QCoreApplication::translate("Working", "Help", nullptr));
#endif // QT_CONFIG(tooltip)
        btnPreorder->setText(QString());
#if QT_CONFIG(tooltip)
        btnCostumerDisplay->setToolTip(QCoreApplication::translate("Working", "Costumer display", nullptr));
#endif // QT_CONFIG(tooltip)
        btnCostumerDisplay->setText(QString());
#if QT_CONFIG(tooltip)
        btnGiftCard->setToolTip(QCoreApplication::translate("Working", "Gift card", nullptr));
#endif // QT_CONFIG(tooltip)
        btnGiftCard->setText(QString());
#if QT_CONFIG(tooltip)
        btnSalesReport->setToolTip(QCoreApplication::translate("Working", "Sales report", nullptr));
#endif // QT_CONFIG(tooltip)
        btnSalesReport->setText(QString());
#if QT_CONFIG(tooltip)
        btnGoodsMovement->setToolTip(QCoreApplication::translate("Working", "Goods movement", nullptr));
#endif // QT_CONFIG(tooltip)
        btnGoodsMovement->setText(QString());
#if QT_CONFIG(tooltip)
        btnDbConnection->setToolTip(QCoreApplication::translate("Working", "Database connection", nullptr));
#endif // QT_CONFIG(tooltip)
        btnDbConnection->setText(QString());
#if QT_CONFIG(tooltip)
        btnColumns->setToolTip(QCoreApplication::translate("Working", "Database connection", nullptr));
#endif // QT_CONFIG(tooltip)
        btnColumns->setText(QString());
#if QT_CONFIG(tooltip)
        btnClientConfigQR->setToolTip(QCoreApplication::translate("Working", "Database connection", nullptr));
#endif // QT_CONFIG(tooltip)
        btnClientConfigQR->setText(QString());
#if QT_CONFIG(tooltip)
        btnServerSettings->setToolTip(QCoreApplication::translate("Working", "Server connection", nullptr));
#endif // QT_CONFIG(tooltip)
        btnServerSettings->setText(QString());
#if QT_CONFIG(tooltip)
        btnMinimize->setToolTip(QCoreApplication::translate("Working", "Close application", nullptr));
#endif // QT_CONFIG(tooltip)
        btnMinimize->setText(QString());
#if QT_CONFIG(tooltip)
        btnCloseApplication->setToolTip(QCoreApplication::translate("Working", "Close application", nullptr));
#endif // QT_CONFIG(tooltip)
        btnCloseApplication->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Working: public Ui_Working {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WORKING_H
