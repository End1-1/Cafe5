/********************************************************************************
** Form generated from reading UI file 'c5tablewithtotal.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C5TABLEWITHTOTAL_H
#define UI_C5TABLEWITHTOTAL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <c5tablewidget.h>
#include <c5tablewidgetwithselector.h>

QT_BEGIN_NAMESPACE

class Ui_C5TableWithTotal
{
public:
    QVBoxLayout *verticalLayout;
    C5TableWidgetWithSelector *tblMain;
    C5TableWidget *tblTotal;

    void setupUi(QWidget *C5TableWithTotal)
    {
        if (C5TableWithTotal->objectName().isEmpty())
            C5TableWithTotal->setObjectName("C5TableWithTotal");
        C5TableWithTotal->resize(817, 654);
        verticalLayout = new QVBoxLayout(C5TableWithTotal);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        tblMain = new C5TableWidgetWithSelector(C5TableWithTotal);
        tblMain->setObjectName("tblMain");
        tblMain->setStyleSheet(QString::fromUtf8("border-top:1px solid #D8D8D8;\n"
"border-left:1px solid #D8D8D8;\n"
"border-right:1px solid #D8D8D8;\n"
"border-bottom: 1px solid #D8D8D8;"));
        tblMain->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        tblMain->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblMain->setEditTriggers(QAbstractItemView::NoEditTriggers);

        verticalLayout->addWidget(tblMain);

        tblTotal = new C5TableWidget(C5TableWithTotal);
        if (tblTotal->rowCount() < 1)
            tblTotal->setRowCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tblTotal->setVerticalHeaderItem(0, __qtablewidgetitem);
        tblTotal->setObjectName("tblTotal");
        tblTotal->setMaximumSize(QSize(16777215, 50));
        tblTotal->setStyleSheet(QString::fromUtf8("border-top:0px solid #D8D8D8;\n"
"border-left:1px solid #D8D8D8;\n"
"border-right:1px solid #D8D8D8;\n"
"border-bottom: 1px solid #D8D8D8;"));
        tblTotal->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        tblTotal->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblTotal->horizontalHeader()->setVisible(false);

        verticalLayout->addWidget(tblTotal);


        retranslateUi(C5TableWithTotal);

        QMetaObject::connectSlotsByName(C5TableWithTotal);
    } // setupUi

    void retranslateUi(QWidget *C5TableWithTotal)
    {
        C5TableWithTotal->setWindowTitle(QCoreApplication::translate("C5TableWithTotal", "Form", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tblTotal->verticalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("C5TableWithTotal", "New Row", nullptr));
    } // retranslateUi

};

namespace Ui {
    class C5TableWithTotal: public Ui_C5TableWithTotal {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5TABLEWITHTOTAL_H
