/********************************************************************************
** Form generated from reading UI file 'goodsreturnreason.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GOODSRETURNREASON_H
#define UI_GOODSRETURNREASON_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>

QT_BEGIN_NAMESPACE

class Ui_GoodsReturnReason
{
public:
    QHBoxLayout *horizontalLayout;
    QListWidget *lst;

    void setupUi(QDialog *GoodsReturnReason)
    {
        if (GoodsReturnReason->objectName().isEmpty())
            GoodsReturnReason->setObjectName("GoodsReturnReason");
        GoodsReturnReason->resize(400, 331);
        horizontalLayout = new QHBoxLayout(GoodsReturnReason);
        horizontalLayout->setObjectName("horizontalLayout");
        lst = new QListWidget(GoodsReturnReason);
        lst->setObjectName("lst");
        QFont font;
        font.setPointSize(14);
        lst->setFont(font);

        horizontalLayout->addWidget(lst);


        retranslateUi(GoodsReturnReason);

        QMetaObject::connectSlotsByName(GoodsReturnReason);
    } // setupUi

    void retranslateUi(QDialog *GoodsReturnReason)
    {
        GoodsReturnReason->setWindowTitle(QCoreApplication::translate("GoodsReturnReason", "Return reason", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GoodsReturnReason: public Ui_GoodsReturnReason {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GOODSRETURNREASON_H
