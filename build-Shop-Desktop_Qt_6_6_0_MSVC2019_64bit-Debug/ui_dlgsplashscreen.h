/********************************************************************************
** Form generated from reading UI file 'dlgsplashscreen.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGSPLASHSCREEN_H
#define UI_DLGSPLASHSCREEN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_DlgSplashScreen
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *lbText;

    void setupUi(QDialog *DlgSplashScreen)
    {
        if (DlgSplashScreen->objectName().isEmpty())
            DlgSplashScreen->setObjectName("DlgSplashScreen");
        DlgSplashScreen->resize(400, 90);
        horizontalLayout = new QHBoxLayout(DlgSplashScreen);
        horizontalLayout->setObjectName("horizontalLayout");
        lbText = new QLabel(DlgSplashScreen);
        lbText->setObjectName("lbText");
        lbText->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(lbText);


        retranslateUi(DlgSplashScreen);

        QMetaObject::connectSlotsByName(DlgSplashScreen);
    } // setupUi

    void retranslateUi(QDialog *DlgSplashScreen)
    {
        DlgSplashScreen->setWindowTitle(QCoreApplication::translate("DlgSplashScreen", "Dialog", nullptr));
        lbText->setText(QCoreApplication::translate("DlgSplashScreen", "Text", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgSplashScreen: public Ui_DlgSplashScreen {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGSPLASHSCREEN_H
