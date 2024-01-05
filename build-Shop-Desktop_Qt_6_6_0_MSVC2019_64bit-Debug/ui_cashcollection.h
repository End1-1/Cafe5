/********************************************************************************
** Form generated from reading UI file 'cashcollection.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CASHCOLLECTION_H
#define UI_CASHCOLLECTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>
#include <c5dateedit.h>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_CashCollection
{
public:
    QGridLayout *gridLayout;
    C5LineEdit *lePurpose;
    QLabel *label_2;
    QLabel *label_5;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *btnSave;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_4;
    C5DateEdit *deDate;
    QLabel *label;
    QLabel *label_3;
    QLabel *label_4;
    C5LineEdit *leAmount;
    C5LineEdit *leAmountCard;
    QLabel *label_6;
    C5LineEdit *leAmountPrepaid;

    void setupUi(QDialog *CashCollection)
    {
        if (CashCollection->objectName().isEmpty())
            CashCollection->setObjectName("CashCollection");
        CashCollection->resize(695, 307);
        QFont font;
        font.setPointSize(18);
        CashCollection->setFont(font);
        gridLayout = new QGridLayout(CashCollection);
        gridLayout->setObjectName("gridLayout");
        lePurpose = new C5LineEdit(CashCollection);
        lePurpose->setObjectName("lePurpose");

        gridLayout->addWidget(lePurpose, 1, 1, 1, 1);

        label_2 = new QLabel(CashCollection);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        label_5 = new QLabel(CashCollection);
        label_5->setObjectName("label_5");
        QFont font1;
        font1.setPointSize(14);
        label_5->setFont(font1);

        gridLayout->addWidget(label_5, 6, 0, 1, 2);

        widget_4 = new QWidget(CashCollection);
        widget_4->setObjectName("widget_4");
        horizontalLayout_4 = new QHBoxLayout(widget_4);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalSpacer_3 = new QSpacerItem(212, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);

        btnSave = new QPushButton(widget_4);
        btnSave->setObjectName("btnSave");

        horizontalLayout_4->addWidget(btnSave);

        btnCancel = new QPushButton(widget_4);
        btnCancel->setObjectName("btnCancel");

        horizontalLayout_4->addWidget(btnCancel);

        horizontalSpacer_4 = new QSpacerItem(212, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);


        gridLayout->addWidget(widget_4, 7, 0, 1, 2);

        deDate = new C5DateEdit(CashCollection);
        deDate->setObjectName("deDate");

        gridLayout->addWidget(deDate, 0, 1, 1, 1);

        label = new QLabel(CashCollection);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_3 = new QLabel(CashCollection);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        label_4 = new QLabel(CashCollection);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        leAmount = new C5LineEdit(CashCollection);
        leAmount->setObjectName("leAmount");

        gridLayout->addWidget(leAmount, 2, 1, 1, 1);

        leAmountCard = new C5LineEdit(CashCollection);
        leAmountCard->setObjectName("leAmountCard");
        leAmountCard->setReadOnly(true);

        gridLayout->addWidget(leAmountCard, 4, 1, 1, 1);

        label_6 = new QLabel(CashCollection);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 5, 0, 1, 1);

        leAmountPrepaid = new C5LineEdit(CashCollection);
        leAmountPrepaid->setObjectName("leAmountPrepaid");
        leAmountPrepaid->setReadOnly(true);

        gridLayout->addWidget(leAmountPrepaid, 5, 1, 1, 1);


        retranslateUi(CashCollection);

        QMetaObject::connectSlotsByName(CashCollection);
    } // setupUi

    void retranslateUi(QDialog *CashCollection)
    {
        CashCollection->setWindowTitle(QCoreApplication::translate("CashCollection", "Cash collection", nullptr));
        label_2->setText(QCoreApplication::translate("CashCollection", "Purpose", nullptr));
        label_5->setText(QCoreApplication::translate("CashCollection", "*collection not supported", nullptr));
        btnSave->setText(QCoreApplication::translate("CashCollection", "Save", nullptr));
        btnCancel->setText(QCoreApplication::translate("CashCollection", "Cancel", nullptr));
        label->setText(QCoreApplication::translate("CashCollection", "Date", nullptr));
        label_3->setText(QCoreApplication::translate("CashCollection", "Amount,cash", nullptr));
        label_4->setText(QCoreApplication::translate("CashCollection", "Amount,card *", nullptr));
        leAmount->setText(QCoreApplication::translate("CashCollection", "0", nullptr));
        leAmountCard->setText(QCoreApplication::translate("CashCollection", "0", nullptr));
        label_6->setText(QCoreApplication::translate("CashCollection", "Amount,prepaid", nullptr));
        leAmountPrepaid->setText(QCoreApplication::translate("CashCollection", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CashCollection: public Ui_CashCollection {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CASHCOLLECTION_H
