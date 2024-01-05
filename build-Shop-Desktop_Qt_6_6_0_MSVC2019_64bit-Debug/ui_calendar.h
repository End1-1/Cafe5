/********************************************************************************
** Form generated from reading UI file 'calendar.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALENDAR_H
#define UI_CALENDAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCalendarWidget>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>

QT_BEGIN_NAMESPACE

class Ui_Calendar
{
public:
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *btnOk;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer;
    QCalendarWidget *calendarWidget;
    QCalendarWidget *calendarWidget_2;

    void setupUi(QDialog *Calendar)
    {
        if (Calendar->objectName().isEmpty())
            Calendar->setObjectName("Calendar");
        Calendar->resize(654, 357);
        gridLayout = new QGridLayout(Calendar);
        gridLayout->setObjectName("gridLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        btnOk = new QPushButton(Calendar);
        btnOk->setObjectName("btnOk");

        horizontalLayout->addWidget(btnOk);

        btnCancel = new QPushButton(Calendar);
        btnCancel->setObjectName("btnCancel");

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout, 1, 1, 1, 2);

        calendarWidget = new QCalendarWidget(Calendar);
        calendarWidget->setObjectName("calendarWidget");

        gridLayout->addWidget(calendarWidget, 0, 2, 1, 1);

        calendarWidget_2 = new QCalendarWidget(Calendar);
        calendarWidget_2->setObjectName("calendarWidget_2");

        gridLayout->addWidget(calendarWidget_2, 0, 3, 1, 1);


        retranslateUi(Calendar);

        QMetaObject::connectSlotsByName(Calendar);
    } // setupUi

    void retranslateUi(QDialog *Calendar)
    {
        Calendar->setWindowTitle(QCoreApplication::translate("Calendar", "Calendar", nullptr));
        btnOk->setText(QCoreApplication::translate("Calendar", "OK", nullptr));
        btnCancel->setText(QCoreApplication::translate("Calendar", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Calendar: public Ui_Calendar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALENDAR_H
