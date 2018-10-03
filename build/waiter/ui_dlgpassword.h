/********************************************************************************
** Form generated from reading UI file 'dlgpassword.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGPASSWORD_H
#define UI_DLGPASSWORD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
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

class Ui_DlgPassword
{
public:
    QGridLayout *gridLayout;
    QPushButton *pushButton_8;
    QPushButton *pushButton_10;
    QPushButton *pushButton_12;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_9;
    QPushButton *pushButton_11;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLineEdit *lePassword;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label;

    void setupUi(QDialog *DlgPassword)
    {
        if (DlgPassword->objectName().isEmpty())
            DlgPassword->setObjectName(QStringLiteral("DlgPassword"));
        DlgPassword->resize(210, 323);
        gridLayout = new QGridLayout(DlgPassword);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pushButton_8 = new QPushButton(DlgPassword);
        pushButton_8->setObjectName(QStringLiteral("pushButton_8"));
        pushButton_8->setMinimumSize(QSize(60, 60));
        pushButton_8->setMaximumSize(QSize(60, 60));
        pushButton_8->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pushButton_8, 4, 1, 1, 1);

        pushButton_10 = new QPushButton(DlgPassword);
        pushButton_10->setObjectName(QStringLiteral("pushButton_10"));
        pushButton_10->setMinimumSize(QSize(60, 60));
        pushButton_10->setMaximumSize(QSize(60, 60));
        pushButton_10->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pushButton_10, 5, 1, 1, 1);

        pushButton_12 = new QPushButton(DlgPassword);
        pushButton_12->setObjectName(QStringLiteral("pushButton_12"));
        pushButton_12->setMinimumSize(QSize(60, 60));
        pushButton_12->setMaximumSize(QSize(60, 60));
        pushButton_12->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QStringLiteral(":/ok.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_12->setIcon(icon);
        pushButton_12->setIconSize(QSize(32, 32));

        gridLayout->addWidget(pushButton_12, 5, 2, 1, 1);

        pushButton = new QPushButton(DlgPassword);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMinimumSize(QSize(60, 60));
        pushButton->setMaximumSize(QSize(60, 60));
        pushButton->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pushButton, 2, 0, 1, 1);

        pushButton_2 = new QPushButton(DlgPassword);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setMinimumSize(QSize(60, 60));
        pushButton_2->setMaximumSize(QSize(60, 60));
        pushButton_2->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pushButton_2, 2, 1, 1, 1);

        pushButton_3 = new QPushButton(DlgPassword);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setMinimumSize(QSize(60, 60));
        pushButton_3->setMaximumSize(QSize(60, 60));
        pushButton_3->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pushButton_3, 2, 2, 1, 1);

        pushButton_4 = new QPushButton(DlgPassword);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setMinimumSize(QSize(60, 60));
        pushButton_4->setMaximumSize(QSize(60, 60));
        pushButton_4->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pushButton_4, 3, 0, 1, 1);

        pushButton_5 = new QPushButton(DlgPassword);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setMinimumSize(QSize(60, 60));
        pushButton_5->setMaximumSize(QSize(60, 60));
        pushButton_5->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pushButton_5, 3, 1, 1, 1);

        pushButton_6 = new QPushButton(DlgPassword);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));
        pushButton_6->setMinimumSize(QSize(60, 60));
        pushButton_6->setMaximumSize(QSize(60, 60));
        pushButton_6->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pushButton_6, 3, 2, 1, 1);

        pushButton_7 = new QPushButton(DlgPassword);
        pushButton_7->setObjectName(QStringLiteral("pushButton_7"));
        pushButton_7->setMinimumSize(QSize(60, 60));
        pushButton_7->setMaximumSize(QSize(60, 60));
        pushButton_7->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pushButton_7, 4, 0, 1, 1);

        pushButton_9 = new QPushButton(DlgPassword);
        pushButton_9->setObjectName(QStringLiteral("pushButton_9"));
        pushButton_9->setMinimumSize(QSize(60, 60));
        pushButton_9->setMaximumSize(QSize(60, 60));
        pushButton_9->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(pushButton_9, 4, 2, 1, 1);

        pushButton_11 = new QPushButton(DlgPassword);
        pushButton_11->setObjectName(QStringLiteral("pushButton_11"));
        pushButton_11->setMinimumSize(QSize(60, 60));
        pushButton_11->setMaximumSize(QSize(60, 60));
        pushButton_11->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_11->setIcon(icon1);
        pushButton_11->setIconSize(QSize(32, 32));

        gridLayout->addWidget(pushButton_11, 5, 0, 1, 1);

        widget = new QWidget(DlgPassword);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(21, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        lePassword = new QLineEdit(widget);
        lePassword->setObjectName(QStringLiteral("lePassword"));
        lePassword->setEchoMode(QLineEdit::Password);
        lePassword->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(lePassword);

        horizontalSpacer_2 = new QSpacerItem(21, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addWidget(widget, 1, 0, 1, 3);

        label = new QLabel(DlgPassword);
        label->setObjectName(QStringLiteral("label"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 3);


        retranslateUi(DlgPassword);

        QMetaObject::connectSlotsByName(DlgPassword);
    } // setupUi

    void retranslateUi(QDialog *DlgPassword)
    {
        DlgPassword->setWindowTitle(QApplication::translate("DlgPassword", "Dialog", nullptr));
        pushButton_8->setText(QApplication::translate("DlgPassword", "8", nullptr));
        pushButton_10->setText(QApplication::translate("DlgPassword", "0", nullptr));
        pushButton_12->setText(QString());
        pushButton->setText(QApplication::translate("DlgPassword", "1", nullptr));
        pushButton_2->setText(QApplication::translate("DlgPassword", "2", nullptr));
        pushButton_3->setText(QApplication::translate("DlgPassword", "3", nullptr));
        pushButton_4->setText(QApplication::translate("DlgPassword", "4", nullptr));
        pushButton_5->setText(QApplication::translate("DlgPassword", "5", nullptr));
        pushButton_6->setText(QApplication::translate("DlgPassword", "6", nullptr));
        pushButton_7->setText(QApplication::translate("DlgPassword", "7", nullptr));
        pushButton_9->setText(QApplication::translate("DlgPassword", "9", nullptr));
        pushButton_11->setText(QString());
        label->setText(QApplication::translate("DlgPassword", "TABLE", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgPassword: public Ui_DlgPassword {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGPASSWORD_H
