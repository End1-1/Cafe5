/********************************************************************************
** Form generated from reading UI file 'dlgpassword.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGPASSWORD_H
#define UI_DLGPASSWORD_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_DlgPassword
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLabel *label;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    C5LineEdit *lePassword;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;
    QPushButton *pushButton_13;
    QPushButton *btnClear;
    QPushButton *pushButton_11;
    QPushButton *pushButton_12;

    void setupUi(QDialog *DlgPassword)
    {
        if (DlgPassword->objectName().isEmpty())
            DlgPassword->setObjectName("DlgPassword");
        DlgPassword->resize(280, 379);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DlgPassword->sizePolicy().hasHeightForWidth());
        DlgPassword->setSizePolicy(sizePolicy);
        DlgPassword->setMaximumSize(QSize(280, 16777215));
        gridLayout = new QGridLayout(DlgPassword);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(1, 1, 1, 1);
        frame = new QFrame(DlgPassword);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        frame->setProperty("smartdlg", QVariant(true));
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName("gridLayout_2");
        label = new QLabel(frame);
        label->setObjectName("label");
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);

        gridLayout_2->addWidget(label, 0, 0, 1, 4);

        widget = new QWidget(frame);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, -1, 0, -1);
        lePassword = new C5LineEdit(widget);
        lePassword->setObjectName("lePassword");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lePassword->sizePolicy().hasHeightForWidth());
        lePassword->setSizePolicy(sizePolicy1);
        lePassword->setMinimumSize(QSize(0, 50));
        QFont font;
        font.setPointSize(16);
        lePassword->setFont(font);
        lePassword->setEchoMode(QLineEdit::Password);
        lePassword->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(lePassword);


        gridLayout_2->addWidget(widget, 1, 0, 1, 4);

        pushButton = new QPushButton(frame);
        pushButton->setObjectName("pushButton");
        pushButton->setMinimumSize(QSize(60, 60));
        pushButton->setMaximumSize(QSize(60, 60));
        pushButton->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton, 2, 0, 1, 1);

        pushButton_2 = new QPushButton(frame);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setMinimumSize(QSize(60, 60));
        pushButton_2->setMaximumSize(QSize(60, 60));
        pushButton_2->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton_2, 2, 1, 1, 1);

        pushButton_3 = new QPushButton(frame);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setMinimumSize(QSize(60, 60));
        pushButton_3->setMaximumSize(QSize(60, 60));
        pushButton_3->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton_3, 2, 2, 1, 1);

        pushButton_4 = new QPushButton(frame);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setMinimumSize(QSize(60, 60));
        pushButton_4->setMaximumSize(QSize(60, 60));
        pushButton_4->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton_4, 2, 3, 1, 1);

        pushButton_5 = new QPushButton(frame);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setMinimumSize(QSize(60, 60));
        pushButton_5->setMaximumSize(QSize(60, 60));
        pushButton_5->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton_5, 3, 0, 1, 1);

        pushButton_6 = new QPushButton(frame);
        pushButton_6->setObjectName("pushButton_6");
        pushButton_6->setMinimumSize(QSize(60, 60));
        pushButton_6->setMaximumSize(QSize(60, 60));
        pushButton_6->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton_6, 3, 1, 1, 1);

        pushButton_7 = new QPushButton(frame);
        pushButton_7->setObjectName("pushButton_7");
        pushButton_7->setMinimumSize(QSize(60, 60));
        pushButton_7->setMaximumSize(QSize(60, 60));
        pushButton_7->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton_7, 3, 2, 1, 1);

        pushButton_8 = new QPushButton(frame);
        pushButton_8->setObjectName("pushButton_8");
        pushButton_8->setMinimumSize(QSize(60, 60));
        pushButton_8->setMaximumSize(QSize(60, 60));
        pushButton_8->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton_8, 3, 3, 1, 1);

        pushButton_9 = new QPushButton(frame);
        pushButton_9->setObjectName("pushButton_9");
        pushButton_9->setMinimumSize(QSize(60, 60));
        pushButton_9->setMaximumSize(QSize(60, 60));
        pushButton_9->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton_9, 4, 0, 1, 1);

        pushButton_10 = new QPushButton(frame);
        pushButton_10->setObjectName("pushButton_10");
        pushButton_10->setMinimumSize(QSize(60, 60));
        pushButton_10->setMaximumSize(QSize(60, 60));
        pushButton_10->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton_10, 4, 1, 1, 1);

        pushButton_13 = new QPushButton(frame);
        pushButton_13->setObjectName("pushButton_13");
        pushButton_13->setMinimumSize(QSize(60, 60));
        pushButton_13->setMaximumSize(QSize(60, 60));
        pushButton_13->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(pushButton_13, 4, 2, 1, 1);

        btnClear = new QPushButton(frame);
        btnClear->setObjectName("btnClear");
        btnClear->setMinimumSize(QSize(60, 60));
        btnClear->setMaximumSize(QSize(60, 60));
        btnClear->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/broom.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnClear->setIcon(icon);
        btnClear->setIconSize(QSize(32, 32));

        gridLayout_2->addWidget(btnClear, 4, 3, 1, 1);

        pushButton_11 = new QPushButton(frame);
        pushButton_11->setObjectName("pushButton_11");
        pushButton_11->setMinimumSize(QSize(60, 60));
        pushButton_11->setMaximumSize(QSize(16777215, 60));
        pushButton_11->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_11->setIcon(icon1);
        pushButton_11->setIconSize(QSize(32, 32));

        gridLayout_2->addWidget(pushButton_11, 5, 0, 1, 2);

        pushButton_12 = new QPushButton(frame);
        pushButton_12->setObjectName("pushButton_12");
        pushButton_12->setMinimumSize(QSize(60, 60));
        pushButton_12->setMaximumSize(QSize(16777215, 60));
        pushButton_12->setFocusPolicy(Qt::NoFocus);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ok.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_12->setIcon(icon2);
        pushButton_12->setIconSize(QSize(32, 32));

        gridLayout_2->addWidget(pushButton_12, 5, 2, 1, 2);


        gridLayout->addWidget(frame, 0, 0, 1, 1);


        retranslateUi(DlgPassword);

        QMetaObject::connectSlotsByName(DlgPassword);
    } // setupUi

    void retranslateUi(QDialog *DlgPassword)
    {
        DlgPassword->setWindowTitle(QString());
        label->setText(QCoreApplication::translate("DlgPassword", "TABLE", nullptr));
        pushButton->setText(QCoreApplication::translate("DlgPassword", "1", nullptr));
        pushButton_2->setText(QCoreApplication::translate("DlgPassword", "2", nullptr));
        pushButton_3->setText(QCoreApplication::translate("DlgPassword", "3", nullptr));
        pushButton_4->setText(QCoreApplication::translate("DlgPassword", "4", nullptr));
        pushButton_5->setText(QCoreApplication::translate("DlgPassword", "5", nullptr));
        pushButton_6->setText(QCoreApplication::translate("DlgPassword", "6", nullptr));
        pushButton_7->setText(QCoreApplication::translate("DlgPassword", "7", nullptr));
        pushButton_8->setText(QCoreApplication::translate("DlgPassword", "8", nullptr));
        pushButton_9->setText(QCoreApplication::translate("DlgPassword", "9", nullptr));
        pushButton_10->setText(QCoreApplication::translate("DlgPassword", "0", nullptr));
        pushButton_13->setText(QCoreApplication::translate("DlgPassword", ".", nullptr));
        btnClear->setText(QString());
        pushButton_11->setText(QString());
        pushButton_12->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class DlgPassword: public Ui_DlgPassword {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGPASSWORD_H
