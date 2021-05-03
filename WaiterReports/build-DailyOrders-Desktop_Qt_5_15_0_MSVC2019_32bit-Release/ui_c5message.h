/********************************************************************************
** Form generated from reading UI file 'c5message.ui'
**
** Created by: Qt User Interface Compiler version 5.15.0
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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_C5Message
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QTextEdit *te;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnYes;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *C5Message)
    {
        if (C5Message->objectName().isEmpty())
            C5Message->setObjectName(QString::fromUtf8("C5Message"));
        C5Message->resize(400, 362);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(C5Message->sizePolicy().hasHeightForWidth());
        C5Message->setSizePolicy(sizePolicy);
        C5Message->setMinimumSize(QSize(400, 0));
        C5Message->setMaximumSize(QSize(16777215, 16777215));
        gridLayout = new QGridLayout(C5Message);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(C5Message);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        te = new QTextEdit(frame);
        te->setObjectName(QString::fromUtf8("te"));
        te->setMaximumSize(QSize(1000, 1000));
        te->setStyleSheet(QString::fromUtf8(""));
        te->setFrameShape(QFrame::Box);
        te->setFrameShadow(QFrame::Sunken);
        te->setReadOnly(true);

        verticalLayout->addWidget(te);

        widget_2 = new QWidget(frame);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setMaximumSize(QSize(16777215, 16777215));
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnYes = new QPushButton(widget_2);
        btnYes->setObjectName(QString::fromUtf8("btnYes"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(btnYes->sizePolicy().hasHeightForWidth());
        btnYes->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(btnYes);

        btnCancel = new QPushButton(widget_2);
        btnCancel->setObjectName(QString::fromUtf8("btnCancel"));
        sizePolicy1.setHeightForWidth(btnCancel->sizePolicy().hasHeightForWidth());
        btnCancel->setSizePolicy(sizePolicy1);

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget_2);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(C5Message);

        QMetaObject::connectSlotsByName(C5Message);
    } // setupUi

    void retranslateUi(QDialog *C5Message)
    {
        C5Message->setWindowTitle(QString());
        btnYes->setText(QCoreApplication::translate("C5Message", "OK", nullptr));
        btnCancel->setText(QCoreApplication::translate("C5Message", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class C5Message: public Ui_C5Message {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5MESSAGE_H
