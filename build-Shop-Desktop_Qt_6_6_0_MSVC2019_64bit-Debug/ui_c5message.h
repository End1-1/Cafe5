/********************************************************************************
** Form generated from reading UI file 'c5message.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C5MESSAGE_H
#define UI_C5MESSAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_C5Message
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QLabel *img;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnYes;
    QPushButton *btnA3;
    QPushButton *btnCancel;
    QPushButton *btnCopy;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *C5Message)
    {
        if (C5Message->objectName().isEmpty())
            C5Message->setObjectName("C5Message");
        C5Message->resize(400, 292);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(C5Message->sizePolicy().hasHeightForWidth());
        C5Message->setSizePolicy(sizePolicy);
        C5Message->setMinimumSize(QSize(400, 0));
        C5Message->setMaximumSize(QSize(800, 16777215));
        gridLayout = new QGridLayout(C5Message);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(C5Message);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Raised);
        frame->setProperty("smartdlg", QVariant(true));
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName("verticalLayout");
        widget = new QWidget(frame);
        widget->setObjectName("widget");
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalSpacer_3 = new QSpacerItem(147, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        img = new QLabel(widget);
        img->setObjectName("img");
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(img->sizePolicy().hasHeightForWidth());
        img->setSizePolicy(sizePolicy1);
        img->setMinimumSize(QSize(48, 48));
        img->setMaximumSize(QSize(48, 48));
        img->setPixmap(QPixmap(QString::fromUtf8(":/info.png")));
        img->setScaledContents(true);
        img->setAlignment(Qt::AlignCenter);
        img->setWordWrap(true);

        horizontalLayout_2->addWidget(img);

        horizontalSpacer_4 = new QSpacerItem(147, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout->addWidget(widget);

        label = new QLabel(frame);
        label->setObjectName("label");
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy2);
        label->setMinimumSize(QSize(0, 0));
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);

        verticalLayout->addWidget(label);

        widget_2 = new QWidget(frame);
        widget_2->setObjectName("widget_2");
        widget_2->setMaximumSize(QSize(16777215, 16777215));
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 24, 0, 0);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnYes = new QPushButton(widget_2);
        btnYes->setObjectName("btnYes");
        sizePolicy2.setHeightForWidth(btnYes->sizePolicy().hasHeightForWidth());
        btnYes->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(btnYes);

        btnA3 = new QPushButton(widget_2);
        btnA3->setObjectName("btnA3");
        sizePolicy2.setHeightForWidth(btnA3->sizePolicy().hasHeightForWidth());
        btnA3->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(btnA3);

        btnCancel = new QPushButton(widget_2);
        btnCancel->setObjectName("btnCancel");
        sizePolicy2.setHeightForWidth(btnCancel->sizePolicy().hasHeightForWidth());
        btnCancel->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(btnCancel);

        btnCopy = new QPushButton(widget_2);
        btnCopy->setObjectName("btnCopy");
        sizePolicy2.setHeightForWidth(btnCopy->sizePolicy().hasHeightForWidth());
        btnCopy->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(btnCopy);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget_2);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(C5Message);

        QMetaObject::connectSlotsByName(C5Message);
    } // setupUi

    void retranslateUi(QDialog *C5Message)
    {
        img->setText(QString());
        label->setText(QCoreApplication::translate("C5Message", "msg", nullptr));
        btnYes->setText(QCoreApplication::translate("C5Message", "A1", nullptr));
        btnA3->setText(QCoreApplication::translate("C5Message", "A3", nullptr));
        btnCancel->setText(QCoreApplication::translate("C5Message", "A2", nullptr));
        btnCopy->setText(QCoreApplication::translate("C5Message", "Copy", nullptr));
        (void)C5Message;
    } // retranslateUi

};

namespace Ui {
    class C5Message: public Ui_C5Message {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5MESSAGE_H
