/********************************************************************************
** Form generated from reading UI file 'dlgserversettings.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGSERVERSETTINGS_H
#define UI_DLGSERVERSETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DlgServerSettings
{
public:
    QGridLayout *gridLayout;
    QLabel *label_3;
    QLabel *label_4;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSave;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_2;
    QLabel *label_5;
    QLineEdit *leServerIP;
    QLineEdit *lePassword2;
    QLabel *label;
    QLineEdit *lePassword;
    QLineEdit *leServerPort;
    QLineEdit *leUsername;

    void setupUi(QDialog *DlgServerSettings)
    {
        if (DlgServerSettings->objectName().isEmpty())
            DlgServerSettings->setObjectName("DlgServerSettings");
        DlgServerSettings->resize(271, 200);
        gridLayout = new QGridLayout(DlgServerSettings);
        gridLayout->setObjectName("gridLayout");
        label_3 = new QLabel(DlgServerSettings);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        label_4 = new QLabel(DlgServerSettings);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 1, 0, 1, 1);

        widget = new QWidget(DlgServerSettings);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(30, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnSave = new QPushButton(widget);
        btnSave->setObjectName("btnSave");

        horizontalLayout->addWidget(btnSave);

        btnCancel = new QPushButton(widget);
        btnCancel->setObjectName("btnCancel");

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(31, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addWidget(widget, 5, 0, 1, 2);

        label_2 = new QLabel(DlgServerSettings);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        label_5 = new QLabel(DlgServerSettings);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        leServerIP = new QLineEdit(DlgServerSettings);
        leServerIP->setObjectName("leServerIP");

        gridLayout->addWidget(leServerIP, 0, 1, 1, 1);

        lePassword2 = new QLineEdit(DlgServerSettings);
        lePassword2->setObjectName("lePassword2");
        lePassword2->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(lePassword2, 4, 1, 1, 1);

        label = new QLabel(DlgServerSettings);
        label->setObjectName("label");

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lePassword = new QLineEdit(DlgServerSettings);
        lePassword->setObjectName("lePassword");
        lePassword->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(lePassword, 3, 1, 1, 1);

        leServerPort = new QLineEdit(DlgServerSettings);
        leServerPort->setObjectName("leServerPort");

        gridLayout->addWidget(leServerPort, 1, 1, 1, 1);

        leUsername = new QLineEdit(DlgServerSettings);
        leUsername->setObjectName("leUsername");

        gridLayout->addWidget(leUsername, 2, 1, 1, 1);

        QWidget::setTabOrder(leServerIP, leServerPort);
        QWidget::setTabOrder(leServerPort, leUsername);
        QWidget::setTabOrder(leUsername, lePassword);
        QWidget::setTabOrder(lePassword, lePassword2);
        QWidget::setTabOrder(lePassword2, btnSave);
        QWidget::setTabOrder(btnSave, btnCancel);

        retranslateUi(DlgServerSettings);

        QMetaObject::connectSlotsByName(DlgServerSettings);
    } // setupUi

    void retranslateUi(QDialog *DlgServerSettings)
    {
        DlgServerSettings->setWindowTitle(QCoreApplication::translate("DlgServerSettings", "Connection settings", nullptr));
        label_3->setText(QCoreApplication::translate("DlgServerSettings", "Password", nullptr));
        label_4->setText(QCoreApplication::translate("DlgServerSettings", "Server port", nullptr));
        btnSave->setText(QCoreApplication::translate("DlgServerSettings", "Save", nullptr));
        btnCancel->setText(QCoreApplication::translate("DlgServerSettings", "Cancel", nullptr));
        label_2->setText(QCoreApplication::translate("DlgServerSettings", "Username", nullptr));
        label_5->setText(QCoreApplication::translate("DlgServerSettings", "Confirm password", nullptr));
        label->setText(QCoreApplication::translate("DlgServerSettings", "Server IP", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgServerSettings: public Ui_DlgServerSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGSERVERSETTINGS_H
