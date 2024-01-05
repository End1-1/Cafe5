/********************************************************************************
** Form generated from reading UI file 'dlgreservgoods.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGRESERVGOODS_H
#define UI_DLGRESERVGOODS_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>
#include <c5dateedit.h>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_DlgReservGoods
{
public:
    QGridLayout *gridLayout;
    QToolButton *btnGoods;
    QLabel *label_5;
    QLabel *label_11;
    QToolButton *btnPrintFiscal;
    C5LineEdit *leReservedQty;
    C5LineEdit *lePrepaid;
    QLabel *label;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_7;
    QLabel *label_3;
    QLabel *label_4;
    QToolButton *btnStore;
    C5LineEdit *leScancode;
    C5LineEdit *leName;
    QLabel *label_2;
    C5DateEdit *leEndDay;
    C5LineEdit *leStore;
    C5LineEdit *leCode;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSave;
    QPushButton *btnCancelReserve;
    QPushButton *btnCompleteReserve;
    QPushButton *btnClose;
    QSpacerItem *horizontalSpacer_2;
    C5LineEdit *leMessage;
    C5LineEdit *leTotalQty;
    QLabel *label_8;
    C5DateEdit *leDate;
    C5LineEdit *leFiscal;
    QLabel *label_6;
    QLabel *label_12;
    C5LineEdit *lePrepaidCard;

    void setupUi(QDialog *DlgReservGoods)
    {
        if (DlgReservGoods->objectName().isEmpty())
            DlgReservGoods->setObjectName("DlgReservGoods");
        DlgReservGoods->resize(420, 373);
        gridLayout = new QGridLayout(DlgReservGoods);
        gridLayout->setObjectName("gridLayout");
        btnGoods = new QToolButton(DlgReservGoods);
        btnGoods->setObjectName("btnGoods");

        gridLayout->addWidget(btnGoods, 3, 2, 1, 1);

        label_5 = new QLabel(DlgReservGoods);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 7, 0, 1, 1);

        label_11 = new QLabel(DlgReservGoods);
        label_11->setObjectName("label_11");

        gridLayout->addWidget(label_11, 10, 0, 1, 1);

        btnPrintFiscal = new QToolButton(DlgReservGoods);
        btnPrintFiscal->setObjectName("btnPrintFiscal");
        btnPrintFiscal->setEnabled(false);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnPrintFiscal->setIcon(icon);

        gridLayout->addWidget(btnPrintFiscal, 10, 2, 1, 1);

        leReservedQty = new C5LineEdit(DlgReservGoods);
        leReservedQty->setObjectName("leReservedQty");

        gridLayout->addWidget(leReservedQty, 7, 1, 1, 1);

        lePrepaid = new C5LineEdit(DlgReservGoods);
        lePrepaid->setObjectName("lePrepaid");

        gridLayout->addWidget(lePrepaid, 8, 1, 1, 1);

        label = new QLabel(DlgReservGoods);
        label->setObjectName("label");

        gridLayout->addWidget(label, 3, 0, 1, 1);

        label_9 = new QLabel(DlgReservGoods);
        label_9->setObjectName("label_9");

        gridLayout->addWidget(label_9, 2, 0, 1, 1);

        label_10 = new QLabel(DlgReservGoods);
        label_10->setObjectName("label_10");

        gridLayout->addWidget(label_10, 8, 0, 1, 1);

        label_7 = new QLabel(DlgReservGoods);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 0, 0, 1, 1);

        label_3 = new QLabel(DlgReservGoods);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 5, 0, 1, 1);

        label_4 = new QLabel(DlgReservGoods);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 6, 0, 1, 1);

        btnStore = new QToolButton(DlgReservGoods);
        btnStore->setObjectName("btnStore");

        gridLayout->addWidget(btnStore, 5, 2, 1, 1);

        leScancode = new C5LineEdit(DlgReservGoods);
        leScancode->setObjectName("leScancode");
        leScancode->setReadOnly(true);

        gridLayout->addWidget(leScancode, 4, 1, 1, 1);

        leName = new C5LineEdit(DlgReservGoods);
        leName->setObjectName("leName");
        leName->setReadOnly(true);

        gridLayout->addWidget(leName, 3, 1, 1, 1);

        label_2 = new QLabel(DlgReservGoods);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 4, 0, 1, 1);

        leEndDay = new C5DateEdit(DlgReservGoods);
        leEndDay->setObjectName("leEndDay");
        leEndDay->setReadOnly(false);

        gridLayout->addWidget(leEndDay, 2, 1, 1, 1);

        leStore = new C5LineEdit(DlgReservGoods);
        leStore->setObjectName("leStore");
        leStore->setReadOnly(true);

        gridLayout->addWidget(leStore, 5, 1, 1, 1);

        leCode = new C5LineEdit(DlgReservGoods);
        leCode->setObjectName("leCode");
        leCode->setFocusPolicy(Qt::NoFocus);
        leCode->setReadOnly(true);

        gridLayout->addWidget(leCode, 0, 1, 1, 1);

        widget = new QWidget(DlgReservGoods);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnSave = new QPushButton(widget);
        btnSave->setObjectName("btnSave");

        horizontalLayout->addWidget(btnSave);

        btnCancelReserve = new QPushButton(widget);
        btnCancelReserve->setObjectName("btnCancelReserve");

        horizontalLayout->addWidget(btnCancelReserve);

        btnCompleteReserve = new QPushButton(widget);
        btnCompleteReserve->setObjectName("btnCompleteReserve");

        horizontalLayout->addWidget(btnCompleteReserve);

        btnClose = new QPushButton(widget);
        btnClose->setObjectName("btnClose");

        horizontalLayout->addWidget(btnClose);

        horizontalSpacer_2 = new QSpacerItem(2, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addWidget(widget, 12, 0, 1, 2);

        leMessage = new C5LineEdit(DlgReservGoods);
        leMessage->setObjectName("leMessage");

        gridLayout->addWidget(leMessage, 11, 1, 1, 1);

        leTotalQty = new C5LineEdit(DlgReservGoods);
        leTotalQty->setObjectName("leTotalQty");
        leTotalQty->setReadOnly(true);

        gridLayout->addWidget(leTotalQty, 6, 1, 1, 1);

        label_8 = new QLabel(DlgReservGoods);
        label_8->setObjectName("label_8");

        gridLayout->addWidget(label_8, 1, 0, 1, 1);

        leDate = new C5DateEdit(DlgReservGoods);
        leDate->setObjectName("leDate");
        leDate->setFocusPolicy(Qt::NoFocus);
        leDate->setReadOnly(true);

        gridLayout->addWidget(leDate, 1, 1, 1, 1);

        leFiscal = new C5LineEdit(DlgReservGoods);
        leFiscal->setObjectName("leFiscal");
        leFiscal->setReadOnly(true);

        gridLayout->addWidget(leFiscal, 10, 1, 1, 1);

        label_6 = new QLabel(DlgReservGoods);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 11, 0, 1, 1);

        label_12 = new QLabel(DlgReservGoods);
        label_12->setObjectName("label_12");

        gridLayout->addWidget(label_12, 9, 0, 1, 1);

        lePrepaidCard = new C5LineEdit(DlgReservGoods);
        lePrepaidCard->setObjectName("lePrepaidCard");

        gridLayout->addWidget(lePrepaidCard, 9, 1, 1, 1);

        QWidget::setTabOrder(leCode, leDate);
        QWidget::setTabOrder(leDate, leEndDay);
        QWidget::setTabOrder(leEndDay, leName);
        QWidget::setTabOrder(leName, leScancode);
        QWidget::setTabOrder(leScancode, leStore);
        QWidget::setTabOrder(leStore, leTotalQty);
        QWidget::setTabOrder(leTotalQty, leReservedQty);
        QWidget::setTabOrder(leReservedQty, leMessage);
        QWidget::setTabOrder(leMessage, btnSave);
        QWidget::setTabOrder(btnSave, btnCancelReserve);
        QWidget::setTabOrder(btnCancelReserve, btnCompleteReserve);
        QWidget::setTabOrder(btnCompleteReserve, btnClose);

        retranslateUi(DlgReservGoods);

        QMetaObject::connectSlotsByName(DlgReservGoods);
    } // setupUi

    void retranslateUi(QDialog *DlgReservGoods)
    {
        DlgReservGoods->setWindowTitle(QCoreApplication::translate("DlgReservGoods", "Goods reservation", nullptr));
        btnGoods->setText(QCoreApplication::translate("DlgReservGoods", "...", nullptr));
        label_5->setText(QCoreApplication::translate("DlgReservGoods", "Reserved qty", nullptr));
        label_11->setText(QCoreApplication::translate("DlgReservGoods", "\325\200\324\264\325\204 \324\277/\325\217", nullptr));
        btnPrintFiscal->setText(QCoreApplication::translate("DlgReservGoods", "...", nullptr));
        label->setText(QCoreApplication::translate("DlgReservGoods", "Name", nullptr));
        label_9->setText(QCoreApplication::translate("DlgReservGoods", "Day of end", nullptr));
        label_10->setText(QCoreApplication::translate("DlgReservGoods", "\324\277\325\241\325\266\325\255\325\253\325\257", nullptr));
        label_7->setText(QCoreApplication::translate("DlgReservGoods", "Reserve number", nullptr));
        label_3->setText(QCoreApplication::translate("DlgReservGoods", "Store", nullptr));
        label_4->setText(QCoreApplication::translate("DlgReservGoods", "Available qty", nullptr));
        btnStore->setText(QCoreApplication::translate("DlgReservGoods", "...", nullptr));
        label_2->setText(QCoreApplication::translate("DlgReservGoods", "Scancode", nullptr));
        btnSave->setText(QCoreApplication::translate("DlgReservGoods", "Save", nullptr));
        btnCancelReserve->setText(QCoreApplication::translate("DlgReservGoods", "Cancel reserve", nullptr));
        btnCompleteReserve->setText(QCoreApplication::translate("DlgReservGoods", "Complete reserve", nullptr));
        btnClose->setText(QCoreApplication::translate("DlgReservGoods", "Close", nullptr));
        label_8->setText(QCoreApplication::translate("DlgReservGoods", "Reserve date", nullptr));
        label_6->setText(QCoreApplication::translate("DlgReservGoods", "Message", nullptr));
        label_12->setText(QCoreApplication::translate("DlgReservGoods", "\324\261\325\266\325\257\325\241\325\266\325\255\325\253\325\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgReservGoods: public Ui_DlgReservGoods {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGRESERVGOODS_H
