/********************************************************************************
** Form generated from reading UI file 'dlggiftcardsale.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGGIFTCARDSALE_H
#define UI_DLGGIFTCARDSALE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DlgGiftCardSale
{
public:
    QVBoxLayout *verticalLayout;
    QLineEdit *leCode;
    QLabel *lbCard;
    QListWidget *lstPrices;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnRegister;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *DlgGiftCardSale)
    {
        if (DlgGiftCardSale->objectName().isEmpty())
            DlgGiftCardSale->setObjectName("DlgGiftCardSale");
        DlgGiftCardSale->resize(442, 485);
        verticalLayout = new QVBoxLayout(DlgGiftCardSale);
        verticalLayout->setObjectName("verticalLayout");
        leCode = new QLineEdit(DlgGiftCardSale);
        leCode->setObjectName("leCode");
        leCode->setEchoMode(QLineEdit::Password);

        verticalLayout->addWidget(leCode);

        lbCard = new QLabel(DlgGiftCardSale);
        lbCard->setObjectName("lbCard");
        QFont font;
        font.setPointSize(18);
        lbCard->setFont(font);
        lbCard->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lbCard);

        lstPrices = new QListWidget(DlgGiftCardSale);
        lstPrices->setObjectName("lstPrices");
        lstPrices->setFocusPolicy(Qt::NoFocus);

        verticalLayout->addWidget(lstPrices);

        widget = new QWidget(DlgGiftCardSale);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(116, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnRegister = new QPushButton(widget);
        btnRegister->setObjectName("btnRegister");
        btnRegister->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/checked.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRegister->setIcon(icon);

        horizontalLayout->addWidget(btnRegister);

        btnCancel = new QPushButton(widget);
        btnCancel->setObjectName("btnCancel");
        btnCancel->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCancel->setIcon(icon1);

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(116, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget);


        retranslateUi(DlgGiftCardSale);

        QMetaObject::connectSlotsByName(DlgGiftCardSale);
    } // setupUi

    void retranslateUi(QDialog *DlgGiftCardSale)
    {
        DlgGiftCardSale->setWindowTitle(QCoreApplication::translate("DlgGiftCardSale", "Dialog", nullptr));
        lbCard->setText(QCoreApplication::translate("DlgGiftCardSale", "*******", nullptr));
        btnRegister->setText(QCoreApplication::translate("DlgGiftCardSale", "Register", nullptr));
        btnCancel->setText(QCoreApplication::translate("DlgGiftCardSale", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgGiftCardSale: public Ui_DlgGiftCardSale {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGGIFTCARDSALE_H
