/********************************************************************************
** Form generated from reading UI file 'selectstaff.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SELECTSTAFF_H
#define UI_SELECTSTAFF_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "c5lineedit.h"

QT_BEGIN_NAMESPACE

class Ui_SelectStaff
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gl;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    C5LineEdit *leNum;
    QSpacerItem *horizontalSpacer;

    void setupUi(QDialog *SelectStaff)
    {
        if (SelectStaff->objectName().isEmpty())
            SelectStaff->setObjectName("SelectStaff");
        SelectStaff->resize(248, 65);
        verticalLayout = new QVBoxLayout(SelectStaff);
        verticalLayout->setObjectName("verticalLayout");
        gl = new QGridLayout();
        gl->setObjectName("gl");

        verticalLayout->addLayout(gl);

        widget = new QWidget(SelectStaff);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        leNum = new C5LineEdit(widget);
        leNum->setObjectName("leNum");
        leNum->setMaximumSize(QSize(100, 16777215));

        horizontalLayout->addWidget(leNum);

        horizontalSpacer = new QSpacerItem(355, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(widget);


        retranslateUi(SelectStaff);

        QMetaObject::connectSlotsByName(SelectStaff);
    } // setupUi

    void retranslateUi(QDialog *SelectStaff)
    {
        SelectStaff->setWindowTitle(QCoreApplication::translate("SelectStaff", "Staff", nullptr));
        label->setText(QCoreApplication::translate("SelectStaff", "Select number and press enter", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SelectStaff: public Ui_SelectStaff {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SELECTSTAFF_H
