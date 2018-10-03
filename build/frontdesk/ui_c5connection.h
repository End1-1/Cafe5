/********************************************************************************
** Form generated from reading UI file 'c5connection.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C5CONNECTION_H
#define UI_C5CONNECTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_C5Connection
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *leHost;
    QLabel *label_2;
    QLineEdit *leDatabase;
    QLabel *label_3;
    QLineEdit *leUsername;
    QLabel *label_4;
    QLineEdit *lePassword;
    QLabel *label_5;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSave;
    QPushButton *btnTest;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;
    QComboBox *cbSettings;

    void setupUi(QDialog *C5Connection)
    {
        if (C5Connection->objectName().isEmpty())
            C5Connection->setObjectName(QStringLiteral("C5Connection"));
        C5Connection->resize(394, 189);
        gridLayout = new QGridLayout(C5Connection);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(C5Connection);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        leHost = new QLineEdit(C5Connection);
        leHost->setObjectName(QStringLiteral("leHost"));

        gridLayout->addWidget(leHost, 0, 1, 1, 1);

        label_2 = new QLabel(C5Connection);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        leDatabase = new QLineEdit(C5Connection);
        leDatabase->setObjectName(QStringLiteral("leDatabase"));

        gridLayout->addWidget(leDatabase, 1, 1, 1, 1);

        label_3 = new QLabel(C5Connection);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 2, 0, 1, 1);

        leUsername = new QLineEdit(C5Connection);
        leUsername->setObjectName(QStringLiteral("leUsername"));

        gridLayout->addWidget(leUsername, 2, 1, 1, 1);

        label_4 = new QLabel(C5Connection);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 3, 0, 1, 1);

        lePassword = new QLineEdit(C5Connection);
        lePassword->setObjectName(QStringLiteral("lePassword"));

        gridLayout->addWidget(lePassword, 3, 1, 1, 1);

        label_5 = new QLabel(C5Connection);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout->addWidget(label_5, 4, 0, 1, 1);

        widget = new QWidget(C5Connection);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(52, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnSave = new QPushButton(widget);
        btnSave->setObjectName(QStringLiteral("btnSave"));

        horizontalLayout->addWidget(btnSave);

        btnTest = new QPushButton(widget);
        btnTest->setObjectName(QStringLiteral("btnTest"));

        horizontalLayout->addWidget(btnTest);

        btnCancel = new QPushButton(widget);
        btnCancel->setObjectName(QStringLiteral("btnCancel"));

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(51, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addWidget(widget, 5, 0, 1, 2);

        cbSettings = new QComboBox(C5Connection);
        cbSettings->setObjectName(QStringLiteral("cbSettings"));

        gridLayout->addWidget(cbSettings, 4, 1, 1, 1);


        retranslateUi(C5Connection);

        QMetaObject::connectSlotsByName(C5Connection);
    } // setupUi

    void retranslateUi(QDialog *C5Connection)
    {
        C5Connection->setWindowTitle(QApplication::translate("C5Connection", "Connection settings", nullptr));
        label->setText(QApplication::translate("C5Connection", "Host", nullptr));
        label_2->setText(QApplication::translate("C5Connection", "Database", nullptr));
        label_3->setText(QApplication::translate("C5Connection", "Username", nullptr));
        label_4->setText(QApplication::translate("C5Connection", "Password", nullptr));
        label_5->setText(QApplication::translate("C5Connection", "Settings", nullptr));
        btnSave->setText(QApplication::translate("C5Connection", "Save", nullptr));
        btnTest->setText(QApplication::translate("C5Connection", "Test", nullptr));
        btnCancel->setText(QApplication::translate("C5Connection", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class C5Connection: public Ui_C5Connection {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5CONNECTION_H
