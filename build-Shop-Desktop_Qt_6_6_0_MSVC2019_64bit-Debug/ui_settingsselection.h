/********************************************************************************
** Form generated from reading UI file 'settingsselection.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSSELECTION_H
#define UI_SETTINGSSELECTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SettingsSelection
{
public:
    QVBoxLayout *verticalLayout;
    QListWidget *lst;

    void setupUi(QDialog *SettingsSelection)
    {
        if (SettingsSelection->objectName().isEmpty())
            SettingsSelection->setObjectName("SettingsSelection");
        SettingsSelection->resize(408, 463);
        QFont font;
        font.setPointSize(12);
        SettingsSelection->setFont(font);
        verticalLayout = new QVBoxLayout(SettingsSelection);
        verticalLayout->setObjectName("verticalLayout");
        lst = new QListWidget(SettingsSelection);
        lst->setObjectName("lst");

        verticalLayout->addWidget(lst);


        retranslateUi(SettingsSelection);

        QMetaObject::connectSlotsByName(SettingsSelection);
    } // setupUi

    void retranslateUi(QDialog *SettingsSelection)
    {
        SettingsSelection->setWindowTitle(QCoreApplication::translate("SettingsSelection", "Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SettingsSelection: public Ui_SettingsSelection {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSSELECTION_H
