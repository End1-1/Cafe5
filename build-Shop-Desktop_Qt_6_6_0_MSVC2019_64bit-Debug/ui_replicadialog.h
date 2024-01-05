/********************************************************************************
** Form generated from reading UI file 'replicadialog.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPLICADIALOG_H
#define UI_REPLICADIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_ReplicaDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *lbMsg;

    void setupUi(QDialog *ReplicaDialog)
    {
        if (ReplicaDialog->objectName().isEmpty())
            ReplicaDialog->setObjectName("ReplicaDialog");
        ReplicaDialog->resize(449, 96);
        horizontalLayout = new QHBoxLayout(ReplicaDialog);
        horizontalLayout->setObjectName("horizontalLayout");
        lbMsg = new QLabel(ReplicaDialog);
        lbMsg->setObjectName("lbMsg");

        horizontalLayout->addWidget(lbMsg);


        retranslateUi(ReplicaDialog);

        QMetaObject::connectSlotsByName(ReplicaDialog);
    } // setupUi

    void retranslateUi(QDialog *ReplicaDialog)
    {
        ReplicaDialog->setWindowTitle(QCoreApplication::translate("ReplicaDialog", "Update database", nullptr));
        lbMsg->setText(QCoreApplication::translate("ReplicaDialog", "Starting", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ReplicaDialog: public Ui_ReplicaDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPLICADIALOG_H
