/********************************************************************************
** Form generated from reading UI file 'dlgdate.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGDATE_H
#define UI_DLGDATE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>
#include <c5dateedit.h>

QT_BEGIN_NAMESPACE

class Ui_DlgDate
{
public:
    QFormLayout *formLayout;
    QLabel *label;
    C5DateEdit *leDate;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnOK;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *DlgDate)
    {
        if (DlgDate->objectName().isEmpty())
            DlgDate->setObjectName("DlgDate");
        DlgDate->resize(284, 85);
        formLayout = new QFormLayout(DlgDate);
        formLayout->setObjectName("formLayout");
        label = new QLabel(DlgDate);
        label->setObjectName("label");

        formLayout->setWidget(0, QFormLayout::LabelRole, label);

        leDate = new C5DateEdit(DlgDate);
        leDate->setObjectName("leDate");

        formLayout->setWidget(0, QFormLayout::FieldRole, leDate);

        widget = new QWidget(DlgDate);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(37, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnOK = new QPushButton(widget);
        btnOK->setObjectName("btnOK");

        horizontalLayout->addWidget(btnOK);

        btnCancel = new QPushButton(widget);
        btnCancel->setObjectName("btnCancel");

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(37, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        formLayout->setWidget(1, QFormLayout::SpanningRole, widget);


        retranslateUi(DlgDate);

        QMetaObject::connectSlotsByName(DlgDate);
    } // setupUi

    void retranslateUi(QDialog *DlgDate)
    {
        DlgDate->setWindowTitle(QCoreApplication::translate("DlgDate", "Date", nullptr));
        label->setText(QCoreApplication::translate("DlgDate", "Date", nullptr));
        btnOK->setText(QCoreApplication::translate("DlgDate", "OK", nullptr));
        btnCancel->setText(QCoreApplication::translate("DlgDate", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgDate: public Ui_DlgDate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGDATE_H
