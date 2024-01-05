/********************************************************************************
** Form generated from reading UI file 'dlgshowcolumns.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGSHOWCOLUMNS_H
#define UI_DLGSHOWCOLUMNS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DlgShowColumns
{
public:
    QVBoxLayout *verticalLayout;
    QCheckBox *chGroup;
    QCheckBox *chUnit;
    QCheckBox *chBox;
    QCheckBox *chDiscount;
    QCheckBox *chEmarks;
    QSpacerItem *verticalSpacer;
    QPushButton *btnClose;

    void setupUi(QDialog *DlgShowColumns)
    {
        if (DlgShowColumns->objectName().isEmpty())
            DlgShowColumns->setObjectName("DlgShowColumns");
        DlgShowColumns->resize(400, 300);
        verticalLayout = new QVBoxLayout(DlgShowColumns);
        verticalLayout->setObjectName("verticalLayout");
        chGroup = new QCheckBox(DlgShowColumns);
        chGroup->setObjectName("chGroup");

        verticalLayout->addWidget(chGroup);

        chUnit = new QCheckBox(DlgShowColumns);
        chUnit->setObjectName("chUnit");

        verticalLayout->addWidget(chUnit);

        chBox = new QCheckBox(DlgShowColumns);
        chBox->setObjectName("chBox");

        verticalLayout->addWidget(chBox);

        chDiscount = new QCheckBox(DlgShowColumns);
        chDiscount->setObjectName("chDiscount");

        verticalLayout->addWidget(chDiscount);

        chEmarks = new QCheckBox(DlgShowColumns);
        chEmarks->setObjectName("chEmarks");

        verticalLayout->addWidget(chEmarks);

        verticalSpacer = new QSpacerItem(20, 181, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        btnClose = new QPushButton(DlgShowColumns);
        btnClose->setObjectName("btnClose");

        verticalLayout->addWidget(btnClose);


        retranslateUi(DlgShowColumns);

        QMetaObject::connectSlotsByName(DlgShowColumns);
    } // setupUi

    void retranslateUi(QDialog *DlgShowColumns)
    {
        DlgShowColumns->setWindowTitle(QCoreApplication::translate("DlgShowColumns", "Dialog", nullptr));
        chGroup->setText(QCoreApplication::translate("DlgShowColumns", "Group", nullptr));
        chUnit->setText(QCoreApplication::translate("DlgShowColumns", "Unit", nullptr));
        chBox->setText(QCoreApplication::translate("DlgShowColumns", "Box", nullptr));
        chDiscount->setText(QCoreApplication::translate("DlgShowColumns", "Discount", nullptr));
        chEmarks->setText(QCoreApplication::translate("DlgShowColumns", "Emarks", nullptr));
        btnClose->setText(QCoreApplication::translate("DlgShowColumns", "Close", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgShowColumns: public Ui_DlgShowColumns {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGSHOWCOLUMNS_H
