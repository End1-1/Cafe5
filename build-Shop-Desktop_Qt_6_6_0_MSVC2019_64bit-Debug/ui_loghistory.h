/********************************************************************************
** Form generated from reading UI file 'loghistory.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGHISTORY_H
#define UI_LOGHISTORY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LogHistory
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnPreviouse;
    QSpacerItem *horizontalSpacer_2;
    QLabel *lbDate;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnNext;
    QPlainTextEdit *ptLog;

    void setupUi(QDialog *LogHistory)
    {
        if (LogHistory->objectName().isEmpty())
            LogHistory->setObjectName("LogHistory");
        LogHistory->resize(1078, 689);
        verticalLayout = new QVBoxLayout(LogHistory);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        btnPreviouse = new QPushButton(LogHistory);
        btnPreviouse->setObjectName("btnPreviouse");

        horizontalLayout->addWidget(btnPreviouse);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        lbDate = new QLabel(LogHistory);
        lbDate->setObjectName("lbDate");

        horizontalLayout->addWidget(lbDate);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnNext = new QPushButton(LogHistory);
        btnNext->setObjectName("btnNext");

        horizontalLayout->addWidget(btnNext);


        verticalLayout->addLayout(horizontalLayout);

        ptLog = new QPlainTextEdit(LogHistory);
        ptLog->setObjectName("ptLog");

        verticalLayout->addWidget(ptLog);


        retranslateUi(LogHistory);

        QMetaObject::connectSlotsByName(LogHistory);
    } // setupUi

    void retranslateUi(QDialog *LogHistory)
    {
        LogHistory->setWindowTitle(QCoreApplication::translate("LogHistory", "Log", nullptr));
        btnPreviouse->setText(QCoreApplication::translate("LogHistory", "Previouse", nullptr));
        lbDate->setText(QCoreApplication::translate("LogHistory", "01/01/2020", nullptr));
        btnNext->setText(QCoreApplication::translate("LogHistory", "Next", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LogHistory: public Ui_LogHistory {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGHISTORY_H
