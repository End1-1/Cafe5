/********************************************************************************
** Form generated from reading UI file 'dlgpin.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGPIN_H
#define UI_DLGPIN_H

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

class Ui_DlgPin
{
public:
    QGridLayout *gridLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QPushButton *btn4;
    QPushButton *btn5;
    QPushButton *btn2;
    QPushButton *btn3;
    QPushButton *btnClear;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    C5LineEdit *leUser;
    C5LineEdit *lePin;
    QPushButton *btn8;
    QPushButton *btn6;
    QPushButton *btn9;
    QPushButton *btn1;
    QPushButton *btn7;
    QPushButton *btnEnter;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QPushButton *btnClose;
    QPushButton *btn0;

    void setupUi(QDialog *DlgPin)
    {
        if (DlgPin->objectName().isEmpty())
            DlgPin->setObjectName("DlgPin");
        DlgPin->resize(246, 415);
        gridLayout = new QGridLayout(DlgPin);
        gridLayout->setObjectName("gridLayout");
        gridLayout->setContentsMargins(2, 2, 2, 2);
        frame = new QFrame(DlgPin);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::Panel);
        frame->setFrameShadow(QFrame::Plain);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName("gridLayout_2");
        btn4 = new QPushButton(frame);
        btn4->setObjectName("btn4");
        btn4->setMinimumSize(QSize(70, 70));
        btn4->setMaximumSize(QSize(70, 70));
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        btn4->setFont(font);
        btn4->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(btn4, 3, 0, 1, 1);

        btn5 = new QPushButton(frame);
        btn5->setObjectName("btn5");
        btn5->setMinimumSize(QSize(70, 70));
        btn5->setMaximumSize(QSize(70, 70));
        btn5->setFont(font);
        btn5->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(btn5, 3, 1, 1, 1);

        btn2 = new QPushButton(frame);
        btn2->setObjectName("btn2");
        btn2->setMinimumSize(QSize(70, 70));
        btn2->setMaximumSize(QSize(70, 70));
        btn2->setFont(font);
        btn2->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(btn2, 2, 1, 1, 1);

        btn3 = new QPushButton(frame);
        btn3->setObjectName("btn3");
        btn3->setMinimumSize(QSize(70, 70));
        btn3->setMaximumSize(QSize(70, 70));
        btn3->setFont(font);
        btn3->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(btn3, 2, 2, 1, 1);

        btnClear = new QPushButton(frame);
        btnClear->setObjectName("btnClear");
        btnClear->setMinimumSize(QSize(70, 70));
        btnClear->setMaximumSize(QSize(70, 70));
        btnClear->setFont(font);
        btnClear->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/broom.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnClear->setIcon(icon);
        btnClear->setIconSize(QSize(40, 40));

        gridLayout_2->addWidget(btnClear, 5, 0, 1, 1);

        widget = new QWidget(frame);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        leUser = new C5LineEdit(widget);
        leUser->setObjectName("leUser");
        QFont font1;
        font1.setPointSize(16);
        leUser->setFont(font1);
        leUser->setMaxLength(4);

        horizontalLayout->addWidget(leUser);

        lePin = new C5LineEdit(widget);
        lePin->setObjectName("lePin");
        lePin->setFont(font1);
        lePin->setMaxLength(4);
        lePin->setEchoMode(QLineEdit::Password);

        horizontalLayout->addWidget(lePin);


        gridLayout_2->addWidget(widget, 1, 0, 1, 3);

        btn8 = new QPushButton(frame);
        btn8->setObjectName("btn8");
        btn8->setMinimumSize(QSize(70, 70));
        btn8->setMaximumSize(QSize(70, 70));
        btn8->setFont(font);
        btn8->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(btn8, 4, 1, 1, 1);

        btn6 = new QPushButton(frame);
        btn6->setObjectName("btn6");
        btn6->setMinimumSize(QSize(70, 70));
        btn6->setMaximumSize(QSize(70, 70));
        btn6->setFont(font);
        btn6->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(btn6, 3, 2, 1, 1);

        btn9 = new QPushButton(frame);
        btn9->setObjectName("btn9");
        btn9->setMinimumSize(QSize(70, 70));
        btn9->setMaximumSize(QSize(70, 70));
        btn9->setFont(font);
        btn9->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(btn9, 4, 2, 1, 1);

        btn1 = new QPushButton(frame);
        btn1->setObjectName("btn1");
        btn1->setMinimumSize(QSize(70, 70));
        btn1->setMaximumSize(QSize(70, 70));
        btn1->setFont(font);
        btn1->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(btn1, 2, 0, 1, 1);

        btn7 = new QPushButton(frame);
        btn7->setObjectName("btn7");
        btn7->setMinimumSize(QSize(70, 70));
        btn7->setMaximumSize(QSize(70, 70));
        btn7->setFont(font);
        btn7->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(btn7, 4, 0, 1, 1);

        btnEnter = new QPushButton(frame);
        btnEnter->setObjectName("btnEnter");
        btnEnter->setMinimumSize(QSize(70, 70));
        btnEnter->setMaximumSize(QSize(70, 70));
        btnEnter->setFont(font);
        btnEnter->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/checked.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEnter->setIcon(icon1);
        btnEnter->setIconSize(QSize(40, 40));

        gridLayout_2->addWidget(btnEnter, 5, 2, 1, 1);

        widget_2 = new QWidget(frame);
        widget_2->setObjectName("widget_2");
        widget_2->setMinimumSize(QSize(0, 50));
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(widget_2);
        label->setObjectName("label");
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);

        horizontalLayout_2->addWidget(label);

        btnClose = new QPushButton(widget_2);
        btnClose->setObjectName("btnClose");
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btnClose->sizePolicy().hasHeightForWidth());
        btnClose->setSizePolicy(sizePolicy);
        btnClose->setMinimumSize(QSize(40, 40));
        btnClose->setMaximumSize(QSize(40, 40));
        QFont font2;
        font2.setPointSize(12);
        font2.setBold(true);
        btnClose->setFont(font2);
        btnClose->setFocusPolicy(Qt::NoFocus);

        horizontalLayout_2->addWidget(btnClose);


        gridLayout_2->addWidget(widget_2, 0, 0, 1, 3);

        btn0 = new QPushButton(frame);
        btn0->setObjectName("btn0");
        btn0->setMinimumSize(QSize(70, 70));
        btn0->setMaximumSize(QSize(70, 70));
        btn0->setFont(font);
        btn0->setFocusPolicy(Qt::NoFocus);

        gridLayout_2->addWidget(btn0, 5, 1, 1, 1);


        gridLayout->addWidget(frame, 0, 1, 1, 1);


        retranslateUi(DlgPin);

        QMetaObject::connectSlotsByName(DlgPin);
    } // setupUi

    void retranslateUi(QDialog *DlgPin)
    {
        DlgPin->setWindowTitle(QCoreApplication::translate("DlgPin", "Shop", nullptr));
        btn4->setText(QCoreApplication::translate("DlgPin", "4", nullptr));
        btn5->setText(QCoreApplication::translate("DlgPin", "5", nullptr));
        btn2->setText(QCoreApplication::translate("DlgPin", "2", nullptr));
        btn3->setText(QCoreApplication::translate("DlgPin", "3", nullptr));
        btnClear->setText(QString());
        leUser->setInputMask(QString());
        leUser->setText(QString());
        leUser->setPlaceholderText(QString());
        lePin->setInputMask(QString());
        lePin->setPlaceholderText(QString());
        btn8->setText(QCoreApplication::translate("DlgPin", "8", nullptr));
        btn6->setText(QCoreApplication::translate("DlgPin", "6", nullptr));
        btn9->setText(QCoreApplication::translate("DlgPin", "9", nullptr));
        btn1->setText(QCoreApplication::translate("DlgPin", "1", nullptr));
        btn7->setText(QCoreApplication::translate("DlgPin", "7", nullptr));
        btnEnter->setText(QString());
        label->setText(QCoreApplication::translate("DlgPin", "Enter your username and pin", nullptr));
        btnClose->setText(QCoreApplication::translate("DlgPin", "X", nullptr));
        btn0->setText(QCoreApplication::translate("DlgPin", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgPin: public Ui_DlgPin {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGPIN_H
