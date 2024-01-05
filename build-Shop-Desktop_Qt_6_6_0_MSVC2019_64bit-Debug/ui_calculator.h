/********************************************************************************
** Form generated from reading UI file 'calculator.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALCULATOR_H
#define UI_CALCULATOR_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_Calculator
{
public:
    QGridLayout *gridLayout;
    QPushButton *btn8;
    QPushButton *btn3;
    QPushButton *btnDiv;
    QPushButton *btn7;
    QPushButton *btnMinus;
    QPushButton *btnBackspace;
    QPushButton *btnPlus;
    QPushButton *btnEnter;
    QPushButton *btnMult;
    QPushButton *btn4;
    QPushButton *btn0;
    QPushButton *btnMPlus;
    QPushButton *btn1;
    QPushButton *btn2;
    QPushButton *btn5;
    QPushButton *btnClear;
    QPushButton *btn9;
    QPushButton *btnSign;
    C5LineEdit *leText;
    QPushButton *btn6;
    QPushButton *btnDecimal;
    QPushButton *btnMRead;
    QPlainTextEdit *plHistory;

    void setupUi(QDialog *Calculator)
    {
        if (Calculator->objectName().isEmpty())
            Calculator->setObjectName("Calculator");
        Calculator->resize(387, 187);
        gridLayout = new QGridLayout(Calculator);
        gridLayout->setObjectName("gridLayout");
        btn8 = new QPushButton(Calculator);
        btn8->setObjectName("btn8");
        btn8->setMaximumSize(QSize(30, 30));
        btn8->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btn8, 2, 1, 1, 1);

        btn3 = new QPushButton(Calculator);
        btn3->setObjectName("btn3");
        btn3->setMaximumSize(QSize(30, 30));
        btn3->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btn3, 4, 2, 1, 1);

        btnDiv = new QPushButton(Calculator);
        btnDiv->setObjectName("btnDiv");
        btnDiv->setMaximumSize(QSize(30, 30));
        btnDiv->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnDiv, 2, 3, 1, 1);

        btn7 = new QPushButton(Calculator);
        btn7->setObjectName("btn7");
        btn7->setMaximumSize(QSize(30, 30));
        btn7->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btn7, 2, 0, 1, 1);

        btnMinus = new QPushButton(Calculator);
        btnMinus->setObjectName("btnMinus");
        btnMinus->setMaximumSize(QSize(30, 30));
        btnMinus->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnMinus, 3, 3, 1, 1);

        btnBackspace = new QPushButton(Calculator);
        btnBackspace->setObjectName("btnBackspace");
        btnBackspace->setMaximumSize(QSize(30, 30));
        btnBackspace->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/backspace.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnBackspace->setIcon(icon);

        gridLayout->addWidget(btnBackspace, 0, 3, 1, 1);

        btnPlus = new QPushButton(Calculator);
        btnPlus->setObjectName("btnPlus");
        btnPlus->setMaximumSize(QSize(30, 30));
        btnPlus->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnPlus, 4, 3, 1, 1);

        btnEnter = new QPushButton(Calculator);
        btnEnter->setObjectName("btnEnter");
        btnEnter->setMaximumSize(QSize(30, 30));
        btnEnter->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnEnter, 5, 3, 1, 1);

        btnMult = new QPushButton(Calculator);
        btnMult->setObjectName("btnMult");
        btnMult->setMaximumSize(QSize(30, 30));
        btnMult->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnMult, 1, 3, 1, 1);

        btn4 = new QPushButton(Calculator);
        btn4->setObjectName("btn4");
        btn4->setMaximumSize(QSize(30, 30));
        btn4->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btn4, 3, 0, 1, 1);

        btn0 = new QPushButton(Calculator);
        btn0->setObjectName("btn0");
        btn0->setMaximumSize(QSize(30, 30));
        btn0->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btn0, 5, 1, 1, 1);

        btnMPlus = new QPushButton(Calculator);
        btnMPlus->setObjectName("btnMPlus");
        btnMPlus->setMaximumSize(QSize(30, 30));
        btnMPlus->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnMPlus, 1, 1, 1, 1);

        btn1 = new QPushButton(Calculator);
        btn1->setObjectName("btn1");
        btn1->setMaximumSize(QSize(30, 30));
        btn1->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btn1, 4, 0, 1, 1);

        btn2 = new QPushButton(Calculator);
        btn2->setObjectName("btn2");
        btn2->setMaximumSize(QSize(30, 30));
        btn2->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btn2, 4, 1, 1, 1);

        btn5 = new QPushButton(Calculator);
        btn5->setObjectName("btn5");
        btn5->setMaximumSize(QSize(30, 30));
        btn5->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btn5, 3, 1, 1, 1);

        btnClear = new QPushButton(Calculator);
        btnClear->setObjectName("btnClear");
        btnClear->setMaximumSize(QSize(30, 30));
        btnClear->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnClear, 1, 0, 1, 1);

        btn9 = new QPushButton(Calculator);
        btn9->setObjectName("btn9");
        btn9->setMaximumSize(QSize(30, 30));
        btn9->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btn9, 2, 2, 1, 1);

        btnSign = new QPushButton(Calculator);
        btnSign->setObjectName("btnSign");
        btnSign->setMaximumSize(QSize(30, 30));
        btnSign->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnSign, 5, 0, 1, 1);

        leText = new C5LineEdit(Calculator);
        leText->setObjectName("leText");
        leText->setEnabled(true);
        leText->setFocusPolicy(Qt::NoFocus);
        leText->setReadOnly(true);

        gridLayout->addWidget(leText, 0, 0, 1, 3);

        btn6 = new QPushButton(Calculator);
        btn6->setObjectName("btn6");
        btn6->setMaximumSize(QSize(30, 30));
        btn6->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btn6, 3, 2, 1, 1);

        btnDecimal = new QPushButton(Calculator);
        btnDecimal->setObjectName("btnDecimal");
        btnDecimal->setMaximumSize(QSize(30, 30));
        btnDecimal->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnDecimal, 5, 2, 1, 1);

        btnMRead = new QPushButton(Calculator);
        btnMRead->setObjectName("btnMRead");
        btnMRead->setMaximumSize(QSize(30, 30));
        btnMRead->setFocusPolicy(Qt::NoFocus);

        gridLayout->addWidget(btnMRead, 1, 2, 1, 1);

        plHistory = new QPlainTextEdit(Calculator);
        plHistory->setObjectName("plHistory");
        plHistory->setFocusPolicy(Qt::NoFocus);
        plHistory->setUndoRedoEnabled(false);
        plHistory->setReadOnly(true);

        gridLayout->addWidget(plHistory, 0, 4, 6, 1);


        retranslateUi(Calculator);
        QObject::connect(btn1, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btn2, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btn3, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btn0, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btnClear, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btnDecimal, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btnEnter, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btnMult, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btnDiv, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btnMinus, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btnPlus, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btn4, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btn5, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btn6, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btn7, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btn8, SIGNAL(clicked()), Calculator, SLOT(actionKey()));
        QObject::connect(btn9, SIGNAL(clicked()), Calculator, SLOT(actionKey()));

        QMetaObject::connectSlotsByName(Calculator);
    } // setupUi

    void retranslateUi(QDialog *Calculator)
    {
        Calculator->setWindowTitle(QCoreApplication::translate("Calculator", "Calculator", nullptr));
        btn8->setText(QCoreApplication::translate("Calculator", "8", nullptr));
        btn3->setText(QCoreApplication::translate("Calculator", "3", nullptr));
        btnDiv->setText(QCoreApplication::translate("Calculator", "\303\267", nullptr));
        btn7->setText(QCoreApplication::translate("Calculator", "7", nullptr));
        btnMinus->setText(QCoreApplication::translate("Calculator", "-", nullptr));
        btnBackspace->setText(QString());
        btnPlus->setText(QCoreApplication::translate("Calculator", "+", nullptr));
        btnEnter->setText(QCoreApplication::translate("Calculator", "=", nullptr));
        btnMult->setText(QCoreApplication::translate("Calculator", "*", nullptr));
        btn4->setText(QCoreApplication::translate("Calculator", "4", nullptr));
        btn0->setText(QCoreApplication::translate("Calculator", "0", nullptr));
        btnMPlus->setText(QCoreApplication::translate("Calculator", "M+", nullptr));
        btn1->setText(QCoreApplication::translate("Calculator", "1", nullptr));
        btn2->setText(QCoreApplication::translate("Calculator", "2", nullptr));
        btn5->setText(QCoreApplication::translate("Calculator", "5", nullptr));
        btnClear->setText(QCoreApplication::translate("Calculator", "C", nullptr));
        btn9->setText(QCoreApplication::translate("Calculator", "9", nullptr));
        btnSign->setText(QCoreApplication::translate("Calculator", "+/-", nullptr));
        btn6->setText(QCoreApplication::translate("Calculator", "6", nullptr));
        btnDecimal->setText(QCoreApplication::translate("Calculator", ".", nullptr));
        btnMRead->setText(QCoreApplication::translate("Calculator", "MR", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Calculator: public Ui_Calculator {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALCULATOR_H
