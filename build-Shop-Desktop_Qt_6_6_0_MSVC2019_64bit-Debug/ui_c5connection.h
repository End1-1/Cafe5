/********************************************************************************
** Form generated from reading UI file 'c5connection.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C5CONNECTION_H
#define UI_C5CONNECTION_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
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
    QComboBox *cbSettings;
    QLabel *label;
    QLineEdit *lePassword;
    QLabel *label_3;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_4;
    QPushButton *btnRefreshSettings;
    QLineEdit *leHost;
    QLabel *label_2;
    QLineEdit *leDatabase;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSave;
    QPushButton *btnTest;
    QPushButton *btnInit;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;
    QLineEdit *leServer;
    QLineEdit *leUsername;
    QCheckBox *chFullScreen;
    QLabel *label_7;

    void setupUi(QDialog *C5Connection)
    {
        if (C5Connection->objectName().isEmpty())
            C5Connection->setObjectName("C5Connection");
        C5Connection->resize(366, 243);
        gridLayout = new QGridLayout(C5Connection);
        gridLayout->setObjectName("gridLayout");
        cbSettings = new QComboBox(C5Connection);
        cbSettings->setObjectName("cbSettings");

        gridLayout->addWidget(cbSettings, 5, 1, 1, 1);

        label = new QLabel(C5Connection);
        label->setObjectName("label");

        gridLayout->addWidget(label, 1, 0, 1, 1);

        lePassword = new QLineEdit(C5Connection);
        lePassword->setObjectName("lePassword");

        gridLayout->addWidget(lePassword, 4, 1, 1, 2);

        label_3 = new QLabel(C5Connection);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        label_5 = new QLabel(C5Connection);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 5, 0, 1, 1);

        label_6 = new QLabel(C5Connection);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 0, 0, 1, 1);

        label_4 = new QLabel(C5Connection);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        btnRefreshSettings = new QPushButton(C5Connection);
        btnRefreshSettings->setObjectName("btnRefreshSettings");
        btnRefreshSettings->setMinimumSize(QSize(25, 25));
        btnRefreshSettings->setMaximumSize(QSize(25, 25));
        btnRefreshSettings->setFocusPolicy(Qt::ClickFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnRefreshSettings->setIcon(icon);

        gridLayout->addWidget(btnRefreshSettings, 5, 2, 1, 1);

        leHost = new QLineEdit(C5Connection);
        leHost->setObjectName("leHost");

        gridLayout->addWidget(leHost, 1, 1, 1, 2);

        label_2 = new QLabel(C5Connection);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        leDatabase = new QLineEdit(C5Connection);
        leDatabase->setObjectName("leDatabase");

        gridLayout->addWidget(leDatabase, 2, 1, 1, 2);

        widget = new QWidget(C5Connection);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(52, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnSave = new QPushButton(widget);
        btnSave->setObjectName("btnSave");

        horizontalLayout->addWidget(btnSave);

        btnTest = new QPushButton(widget);
        btnTest->setObjectName("btnTest");

        horizontalLayout->addWidget(btnTest);

        btnInit = new QPushButton(widget);
        btnInit->setObjectName("btnInit");

        horizontalLayout->addWidget(btnInit);

        btnCancel = new QPushButton(widget);
        btnCancel->setObjectName("btnCancel");

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(51, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addWidget(widget, 7, 0, 1, 3);

        leServer = new QLineEdit(C5Connection);
        leServer->setObjectName("leServer");

        gridLayout->addWidget(leServer, 0, 1, 1, 2);

        leUsername = new QLineEdit(C5Connection);
        leUsername->setObjectName("leUsername");

        gridLayout->addWidget(leUsername, 3, 1, 1, 2);

        chFullScreen = new QCheckBox(C5Connection);
        chFullScreen->setObjectName("chFullScreen");

        gridLayout->addWidget(chFullScreen, 6, 1, 1, 1);

        label_7 = new QLabel(C5Connection);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 6, 0, 1, 1);

        QWidget::setTabOrder(leHost, leDatabase);
        QWidget::setTabOrder(leDatabase, leUsername);
        QWidget::setTabOrder(leUsername, lePassword);
        QWidget::setTabOrder(lePassword, cbSettings);
        QWidget::setTabOrder(cbSettings, btnSave);
        QWidget::setTabOrder(btnSave, btnTest);
        QWidget::setTabOrder(btnTest, btnInit);
        QWidget::setTabOrder(btnInit, btnCancel);
        QWidget::setTabOrder(btnCancel, btnRefreshSettings);

        retranslateUi(C5Connection);

        QMetaObject::connectSlotsByName(C5Connection);
    } // setupUi

    void retranslateUi(QDialog *C5Connection)
    {
        C5Connection->setWindowTitle(QCoreApplication::translate("C5Connection", "Connection settings", nullptr));
        label->setText(QCoreApplication::translate("C5Connection", "Database Host", nullptr));
        label_3->setText(QCoreApplication::translate("C5Connection", "Username", nullptr));
        label_5->setText(QCoreApplication::translate("C5Connection", "Settings", nullptr));
        label_6->setText(QCoreApplication::translate("C5Connection", "Server address", nullptr));
        label_4->setText(QCoreApplication::translate("C5Connection", "Password", nullptr));
        btnRefreshSettings->setText(QString());
        label_2->setText(QCoreApplication::translate("C5Connection", "Database", nullptr));
        btnSave->setText(QCoreApplication::translate("C5Connection", "Save", nullptr));
        btnTest->setText(QCoreApplication::translate("C5Connection", "Test", nullptr));
        btnInit->setText(QCoreApplication::translate("C5Connection", "Init", nullptr));
        btnCancel->setText(QCoreApplication::translate("C5Connection", "Cancel", nullptr));
        chFullScreen->setText(QString());
        label_7->setText(QCoreApplication::translate("C5Connection", "Full screen", nullptr));
    } // retranslateUi

};

namespace Ui {
    class C5Connection: public Ui_C5Connection {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5CONNECTION_H
