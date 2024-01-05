/********************************************************************************
** Form generated from reading UI file 'selectprinters.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTPRINTERS_H
#define UI_SELECTPRINTERS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SelectPrinters
{
public:
    QVBoxLayout *verticalLayout;
    QCheckBox *chP1;
    QCheckBox *chP2;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOk;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *SelectPrinters)
    {
        if (SelectPrinters->objectName().isEmpty())
            SelectPrinters->setObjectName("SelectPrinters");
        SelectPrinters->resize(400, 139);
        QFont font;
        font.setPointSize(16);
        SelectPrinters->setFont(font);
        verticalLayout = new QVBoxLayout(SelectPrinters);
        verticalLayout->setObjectName("verticalLayout");
        chP1 = new QCheckBox(SelectPrinters);
        chP1->setObjectName("chP1");
        chP1->setChecked(true);

        verticalLayout->addWidget(chP1);

        chP2 = new QCheckBox(SelectPrinters);
        chP2->setObjectName("chP2");
        chP2->setChecked(true);

        verticalLayout->addWidget(chP2);

        widget = new QWidget(SelectPrinters);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(95, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnOk = new QPushButton(widget);
        btnOk->setObjectName("btnOk");

        horizontalLayout->addWidget(btnOk);

        btnCancel = new QPushButton(widget);
        btnCancel->setObjectName("btnCancel");

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(95, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget);


        retranslateUi(SelectPrinters);

        QMetaObject::connectSlotsByName(SelectPrinters);
    } // setupUi

    void retranslateUi(QDialog *SelectPrinters)
    {
        SelectPrinters->setWindowTitle(QCoreApplication::translate("SelectPrinters", "Select printers", nullptr));
        chP1->setText(QCoreApplication::translate("SelectPrinters", "Print #1", nullptr));
        chP2->setText(QCoreApplication::translate("SelectPrinters", "Print #2", nullptr));
        btnOk->setText(QCoreApplication::translate("SelectPrinters", "OK", nullptr));
        btnCancel->setText(QCoreApplication::translate("SelectPrinters", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SelectPrinters: public Ui_SelectPrinters {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTPRINTERS_H
