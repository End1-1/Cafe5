/********************************************************************************
** Form generated from reading UI file 'dlgqrcode.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGQRCODE_H
#define UI_DLGQRCODE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>

QT_BEGIN_NAMESPACE

class Ui_DlgQrCode
{
public:

    void setupUi(QDialog *DlgQrCode)
    {
        if (DlgQrCode->objectName().isEmpty())
            DlgQrCode->setObjectName("DlgQrCode");
        DlgQrCode->resize(400, 300);

        retranslateUi(DlgQrCode);

        QMetaObject::connectSlotsByName(DlgQrCode);
    } // setupUi

    void retranslateUi(QDialog *DlgQrCode)
    {
        DlgQrCode->setWindowTitle(QCoreApplication::translate("DlgQrCode", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgQrCode: public Ui_DlgQrCode {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGQRCODE_H
