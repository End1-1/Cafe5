/********************************************************************************
** Form generated from reading UI file 'c5message.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C5MESSAGE_H
#define UI_C5MESSAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_C5Message
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *lbMessage;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnYes;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *C5Message)
    {
        if (C5Message->objectName().isEmpty())
            C5Message->setObjectName(QStringLiteral("C5Message"));
        C5Message->resize(721, 382);
        verticalLayout = new QVBoxLayout(C5Message);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        lbMessage = new QLabel(C5Message);
        lbMessage->setObjectName(QStringLiteral("lbMessage"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(lbMessage->sizePolicy().hasHeightForWidth());
        lbMessage->setSizePolicy(sizePolicy);
        lbMessage->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lbMessage);

        widget = new QWidget(C5Message);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(256, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnYes = new QPushButton(widget);
        btnYes->setObjectName(QStringLiteral("btnYes"));

        horizontalLayout->addWidget(btnYes);

        btnCancel = new QPushButton(widget);
        btnCancel->setObjectName(QStringLiteral("btnCancel"));

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(255, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget);


        retranslateUi(C5Message);

        QMetaObject::connectSlotsByName(C5Message);
    } // setupUi

    void retranslateUi(QDialog *C5Message)
    {
        C5Message->setWindowTitle(QString());
        lbMessage->setText(QApplication::translate("C5Message", "MESSAGE", nullptr));
        btnYes->setText(QApplication::translate("C5Message", "OK", nullptr));
        btnCancel->setText(QApplication::translate("C5Message", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class C5Message: public Ui_C5Message {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5MESSAGE_H
