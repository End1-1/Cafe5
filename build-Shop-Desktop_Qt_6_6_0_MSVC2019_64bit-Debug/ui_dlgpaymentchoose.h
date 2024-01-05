/********************************************************************************
** Form generated from reading UI file 'dlgpaymentchoose.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGPAYMENTCHOOSE_H
#define UI_DLGPAYMENTCHOOSE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_DlgPaymentChoose
{
public:
    QGridLayout *gridLayout;
    C5LineEdit *leCashIn;
    QPushButton *btnCard;
    QPushButton *btnBankTransfer;
    QPushButton *btnChange;
    QPushButton *btnPrepaid;
    C5LineEdit *leTotal;
    QPushButton *btnCashin;
    C5LineEdit *lePrepaid;
    C5LineEdit *leBankTransfer;
    C5LineEdit *leIdram;
    C5LineEdit *leDebt;
    QPushButton *btnCash;
    C5LineEdit *leChange;
    C5LineEdit *leCash;
    C5LineEdit *leCard;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnPay;
    QPushButton *btnBack;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btnFiscal;
    QLabel *label;
    C5LineEdit *leTelcell;
    QPushButton *btnIdram;
    QPushButton *btnTelcell;
    QPushButton *btnDebt;
    QPushButton *btnCredit;
    C5LineEdit *leCredit;

    void setupUi(QDialog *DlgPaymentChoose)
    {
        if (DlgPaymentChoose->objectName().isEmpty())
            DlgPaymentChoose->setObjectName("DlgPaymentChoose");
        DlgPaymentChoose->resize(667, 710);
        gridLayout = new QGridLayout(DlgPaymentChoose);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setVerticalSpacing(12);
        leCashIn = new C5LineEdit(DlgPaymentChoose);
        leCashIn->setObjectName("leCashIn");
        QFont font;
        font.setPointSize(26);
        leCashIn->setFont(font);

        gridLayout->addWidget(leCashIn, 9, 2, 1, 1);

        btnCard = new QPushButton(DlgPaymentChoose);
        btnCard->setObjectName("btnCard");
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btnCard->sizePolicy().hasHeightForWidth());
        btnCard->setSizePolicy(sizePolicy);
        btnCard->setFont(font);
        btnCard->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnCard, 2, 0, 1, 1);

        btnBankTransfer = new QPushButton(DlgPaymentChoose);
        btnBankTransfer->setObjectName("btnBankTransfer");
        sizePolicy.setHeightForWidth(btnBankTransfer->sizePolicy().hasHeightForWidth());
        btnBankTransfer->setSizePolicy(sizePolicy);
        btnBankTransfer->setFont(font);
        btnBankTransfer->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnBankTransfer, 3, 0, 1, 1);

        btnChange = new QPushButton(DlgPaymentChoose);
        btnChange->setObjectName("btnChange");
        sizePolicy.setHeightForWidth(btnChange->sizePolicy().hasHeightForWidth());
        btnChange->setSizePolicy(sizePolicy);
        btnChange->setFont(font);
        btnChange->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnChange, 10, 0, 1, 1);

        btnPrepaid = new QPushButton(DlgPaymentChoose);
        btnPrepaid->setObjectName("btnPrepaid");
        sizePolicy.setHeightForWidth(btnPrepaid->sizePolicy().hasHeightForWidth());
        btnPrepaid->setSizePolicy(sizePolicy);
        btnPrepaid->setFont(font);
        btnPrepaid->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnPrepaid, 7, 0, 1, 1);

        leTotal = new C5LineEdit(DlgPaymentChoose);
        leTotal->setObjectName("leTotal");
        QFont font1;
        font1.setPointSize(30);
        font1.setBold(true);
        leTotal->setFont(font1);
        leTotal->setFocusPolicy(Qt::NoFocus);
        leTotal->setStyleSheet(QString::fromUtf8("color: red"));
        leTotal->setReadOnly(true);

        gridLayout->addWidget(leTotal, 0, 2, 1, 1);

        btnCashin = new QPushButton(DlgPaymentChoose);
        btnCashin->setObjectName("btnCashin");
        sizePolicy.setHeightForWidth(btnCashin->sizePolicy().hasHeightForWidth());
        btnCashin->setSizePolicy(sizePolicy);
        btnCashin->setFont(font);
        btnCashin->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnCashin, 9, 0, 1, 1);

        lePrepaid = new C5LineEdit(DlgPaymentChoose);
        lePrepaid->setObjectName("lePrepaid");
        lePrepaid->setFont(font);

        gridLayout->addWidget(lePrepaid, 7, 2, 1, 1);

        leBankTransfer = new C5LineEdit(DlgPaymentChoose);
        leBankTransfer->setObjectName("leBankTransfer");
        leBankTransfer->setFont(font);

        gridLayout->addWidget(leBankTransfer, 3, 2, 1, 1);

        leIdram = new C5LineEdit(DlgPaymentChoose);
        leIdram->setObjectName("leIdram");
        leIdram->setFont(font);

        gridLayout->addWidget(leIdram, 5, 2, 1, 1);

        leDebt = new C5LineEdit(DlgPaymentChoose);
        leDebt->setObjectName("leDebt");
        leDebt->setFont(font);

        gridLayout->addWidget(leDebt, 8, 2, 1, 1);

        btnCash = new QPushButton(DlgPaymentChoose);
        btnCash->setObjectName("btnCash");
        sizePolicy.setHeightForWidth(btnCash->sizePolicy().hasHeightForWidth());
        btnCash->setSizePolicy(sizePolicy);
        btnCash->setFont(font);
        btnCash->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnCash, 1, 0, 1, 1);

        leChange = new C5LineEdit(DlgPaymentChoose);
        leChange->setObjectName("leChange");
        leChange->setFont(font);
        leChange->setReadOnly(true);

        gridLayout->addWidget(leChange, 10, 2, 1, 1);

        leCash = new C5LineEdit(DlgPaymentChoose);
        leCash->setObjectName("leCash");
        leCash->setFont(font);

        gridLayout->addWidget(leCash, 1, 2, 1, 1);

        leCard = new C5LineEdit(DlgPaymentChoose);
        leCard->setObjectName("leCard");
        leCard->setFont(font);

        gridLayout->addWidget(leCard, 2, 2, 1, 1);

        widget = new QWidget(DlgPaymentChoose);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(99, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnPay = new QPushButton(widget);
        btnPay->setObjectName("btnPay");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btnPay->sizePolicy().hasHeightForWidth());
        btnPay->setSizePolicy(sizePolicy1);
        btnPay->setMinimumSize(QSize(150, 50));
        btnPay->setFont(font);

        horizontalLayout->addWidget(btnPay);

        btnBack = new QPushButton(widget);
        btnBack->setObjectName("btnBack");
        sizePolicy1.setHeightForWidth(btnBack->sizePolicy().hasHeightForWidth());
        btnBack->setSizePolicy(sizePolicy1);
        btnBack->setMinimumSize(QSize(150, 50));
        btnBack->setFont(font);

        horizontalLayout->addWidget(btnBack);

        horizontalSpacer_2 = new QSpacerItem(100, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        btnFiscal = new QPushButton(widget);
        btnFiscal->setObjectName("btnFiscal");
        sizePolicy1.setHeightForWidth(btnFiscal->sizePolicy().hasHeightForWidth());
        btnFiscal->setSizePolicy(sizePolicy1);
        btnFiscal->setMinimumSize(QSize(0, 50));
        btnFiscal->setFont(font);
        btnFiscal->setStyleSheet(QString::fromUtf8("checked: {\n"
"background:blue;\n"
"}"));
        btnFiscal->setCheckable(true);
        btnFiscal->setChecked(false);

        horizontalLayout->addWidget(btnFiscal);


        gridLayout->addWidget(widget, 12, 0, 1, 3);

        label = new QLabel(DlgPaymentChoose);
        label->setObjectName("label");
        QFont font2;
        font2.setPointSize(30);
        label->setFont(font2);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        leTelcell = new C5LineEdit(DlgPaymentChoose);
        leTelcell->setObjectName("leTelcell");
        leTelcell->setFont(font);

        gridLayout->addWidget(leTelcell, 6, 2, 1, 1);

        btnIdram = new QPushButton(DlgPaymentChoose);
        btnIdram->setObjectName("btnIdram");
        sizePolicy.setHeightForWidth(btnIdram->sizePolicy().hasHeightForWidth());
        btnIdram->setSizePolicy(sizePolicy);
        btnIdram->setFont(font);
        btnIdram->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnIdram, 5, 0, 1, 1);

        btnTelcell = new QPushButton(DlgPaymentChoose);
        btnTelcell->setObjectName("btnTelcell");
        sizePolicy.setHeightForWidth(btnTelcell->sizePolicy().hasHeightForWidth());
        btnTelcell->setSizePolicy(sizePolicy);
        btnTelcell->setFont(font);
        btnTelcell->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnTelcell, 6, 0, 1, 1);

        btnDebt = new QPushButton(DlgPaymentChoose);
        btnDebt->setObjectName("btnDebt");
        sizePolicy.setHeightForWidth(btnDebt->sizePolicy().hasHeightForWidth());
        btnDebt->setSizePolicy(sizePolicy);
        btnDebt->setFont(font);
        btnDebt->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnDebt, 8, 0, 1, 1);

        btnCredit = new QPushButton(DlgPaymentChoose);
        btnCredit->setObjectName("btnCredit");
        sizePolicy.setHeightForWidth(btnCredit->sizePolicy().hasHeightForWidth());
        btnCredit->setSizePolicy(sizePolicy);
        btnCredit->setFont(font);
        btnCredit->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnCredit, 4, 0, 1, 1);

        leCredit = new C5LineEdit(DlgPaymentChoose);
        leCredit->setObjectName("leCredit");
        leCredit->setFont(font);

        gridLayout->addWidget(leCredit, 4, 2, 1, 1);

        QWidget::setTabOrder(leCash, leCard);
        QWidget::setTabOrder(leCard, leBankTransfer);
        QWidget::setTabOrder(leBankTransfer, leCredit);
        QWidget::setTabOrder(leCredit, leIdram);
        QWidget::setTabOrder(leIdram, leTelcell);
        QWidget::setTabOrder(leTelcell, lePrepaid);
        QWidget::setTabOrder(lePrepaid, leDebt);
        QWidget::setTabOrder(leDebt, leCashIn);
        QWidget::setTabOrder(leCashIn, leChange);
        QWidget::setTabOrder(leChange, btnBack);
        QWidget::setTabOrder(btnBack, btnFiscal);
        QWidget::setTabOrder(btnFiscal, btnPay);

        retranslateUi(DlgPaymentChoose);

        QMetaObject::connectSlotsByName(DlgPaymentChoose);
    } // setupUi

    void retranslateUi(QDialog *DlgPaymentChoose)
    {
        DlgPaymentChoose->setWindowTitle(QCoreApplication::translate("DlgPaymentChoose", "Dialog", nullptr));
        leCashIn->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
        btnCard->setText(QCoreApplication::translate("DlgPaymentChoose", "Card", nullptr));
        btnBankTransfer->setText(QCoreApplication::translate("DlgPaymentChoose", "Bank transfer", nullptr));
        btnChange->setText(QCoreApplication::translate("DlgPaymentChoose", "Change", nullptr));
        btnPrepaid->setText(QCoreApplication::translate("DlgPaymentChoose", "Prepaid", nullptr));
        leTotal->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
        btnCashin->setText(QCoreApplication::translate("DlgPaymentChoose", "Cash in", nullptr));
        lePrepaid->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
        leBankTransfer->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
        leIdram->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
        leDebt->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
        btnCash->setText(QCoreApplication::translate("DlgPaymentChoose", "Cash", nullptr));
        leChange->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
        leCash->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
        leCard->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
        btnPay->setText(QCoreApplication::translate("DlgPaymentChoose", "Pay (Enter)", nullptr));
        btnBack->setText(QCoreApplication::translate("DlgPaymentChoose", "Back (Esc)", nullptr));
        btnFiscal->setText(QCoreApplication::translate("DlgPaymentChoose", "Fiscal\n"
"(F12)", nullptr));
        label->setText(QCoreApplication::translate("DlgPaymentChoose", "Total", nullptr));
        leTelcell->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
        btnIdram->setText(QCoreApplication::translate("DlgPaymentChoose", "Idram", nullptr));
        btnTelcell->setText(QCoreApplication::translate("DlgPaymentChoose", "Telcell", nullptr));
        btnDebt->setText(QCoreApplication::translate("DlgPaymentChoose", "Debt", nullptr));
        btnCredit->setText(QCoreApplication::translate("DlgPaymentChoose", "Credit", nullptr));
        leCredit->setText(QCoreApplication::translate("DlgPaymentChoose", "999999999", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgPaymentChoose: public Ui_DlgPaymentChoose {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGPAYMENTCHOOSE_H
