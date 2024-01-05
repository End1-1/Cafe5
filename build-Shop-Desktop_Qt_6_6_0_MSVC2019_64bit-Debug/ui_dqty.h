/********************************************************************************
** Form generated from reading UI file 'dqty.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DQTY_H
#define UI_DQTY_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DQty
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *lbTitle;
    QLineEdit *leQty;

    void setupUi(QDialog *DQty)
    {
        if (DQty->objectName().isEmpty())
            DQty->setObjectName("DQty");
        DQty->resize(332, 127);
        verticalLayout = new QVBoxLayout(DQty);
        verticalLayout->setObjectName("verticalLayout");
        lbTitle = new QLabel(DQty);
        lbTitle->setObjectName("lbTitle");
        QFont font;
        font.setPointSize(15);
        lbTitle->setFont(font);
        lbTitle->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lbTitle);

        leQty = new QLineEdit(DQty);
        leQty->setObjectName("leQty");
        QFont font1;
        font1.setPointSize(64);
        leQty->setFont(font1);

        verticalLayout->addWidget(leQty);


        retranslateUi(DQty);

        QMetaObject::connectSlotsByName(DQty);
    } // setupUi

    void retranslateUi(QDialog *DQty)
    {
        DQty->setWindowTitle(QString());
        lbTitle->setText(QCoreApplication::translate("DQty", "TITLE", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DQty: public Ui_DQty {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DQTY_H
