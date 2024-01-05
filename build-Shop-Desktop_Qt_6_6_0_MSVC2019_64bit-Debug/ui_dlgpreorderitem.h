/********************************************************************************
** Form generated from reading UI file 'dlgpreorderitem.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGPREORDERITEM_H
#define UI_DLGPREORDERITEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_DlgPreorderItem
{
public:
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *leName;
    C5LineEdit *lePrice;
    QLabel *label_6;
    QLabel *label_3;
    QLabel *label_4;
    C5LineEdit *leTotal;
    C5LineEdit *leScancode;
    QLabel *label_2;
    QLineEdit *leUUID;
    QLabel *label_7;
    C5LineEdit *leQty;
    C5LineEdit *leGoodsId;
    QLabel *label_5;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnSave;
    QPushButton *btnCancel;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *btnGetGoods;

    void setupUi(QDialog *DlgPreorderItem)
    {
        if (DlgPreorderItem->objectName().isEmpty())
            DlgPreorderItem->setObjectName("DlgPreorderItem");
        DlgPreorderItem->resize(599, 241);
        gridLayout = new QGridLayout(DlgPreorderItem);
        gridLayout->setObjectName("gridLayout");
        label = new QLabel(DlgPreorderItem);
        label->setObjectName("label");

        gridLayout->addWidget(label, 1, 0, 1, 1);

        leName = new QLineEdit(DlgPreorderItem);
        leName->setObjectName("leName");
        leName->setReadOnly(true);

        gridLayout->addWidget(leName, 4, 1, 1, 1);

        lePrice = new C5LineEdit(DlgPreorderItem);
        lePrice->setObjectName("lePrice");

        gridLayout->addWidget(lePrice, 6, 1, 1, 1);

        label_6 = new QLabel(DlgPreorderItem);
        label_6->setObjectName("label_6");

        gridLayout->addWidget(label_6, 7, 0, 1, 1);

        label_3 = new QLabel(DlgPreorderItem);
        label_3->setObjectName("label_3");

        gridLayout->addWidget(label_3, 4, 0, 1, 1);

        label_4 = new QLabel(DlgPreorderItem);
        label_4->setObjectName("label_4");

        gridLayout->addWidget(label_4, 5, 0, 1, 1);

        leTotal = new C5LineEdit(DlgPreorderItem);
        leTotal->setObjectName("leTotal");

        gridLayout->addWidget(leTotal, 7, 1, 1, 1);

        leScancode = new C5LineEdit(DlgPreorderItem);
        leScancode->setObjectName("leScancode");
        leScancode->setReadOnly(true);

        gridLayout->addWidget(leScancode, 3, 1, 1, 1);

        label_2 = new QLabel(DlgPreorderItem);
        label_2->setObjectName("label_2");

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        leUUID = new QLineEdit(DlgPreorderItem);
        leUUID->setObjectName("leUUID");
        leUUID->setReadOnly(true);

        gridLayout->addWidget(leUUID, 1, 1, 1, 1);

        label_7 = new QLabel(DlgPreorderItem);
        label_7->setObjectName("label_7");

        gridLayout->addWidget(label_7, 3, 0, 1, 1);

        leQty = new C5LineEdit(DlgPreorderItem);
        leQty->setObjectName("leQty");

        gridLayout->addWidget(leQty, 5, 1, 1, 1);

        leGoodsId = new C5LineEdit(DlgPreorderItem);
        leGoodsId->setObjectName("leGoodsId");
        leGoodsId->setReadOnly(true);

        gridLayout->addWidget(leGoodsId, 2, 1, 1, 1);

        label_5 = new QLabel(DlgPreorderItem);
        label_5->setObjectName("label_5");

        gridLayout->addWidget(label_5, 6, 0, 1, 1);

        widget = new QWidget(DlgPreorderItem);
        widget->setObjectName("widget");
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalSpacer = new QSpacerItem(179, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnSave = new QPushButton(widget);
        btnSave->setObjectName("btnSave");

        horizontalLayout->addWidget(btnSave);

        btnCancel = new QPushButton(widget);
        btnCancel->setObjectName("btnCancel");

        horizontalLayout->addWidget(btnCancel);

        horizontalSpacer_2 = new QSpacerItem(179, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        gridLayout->addWidget(widget, 8, 0, 1, 2);

        btnGetGoods = new QToolButton(DlgPreorderItem);
        btnGetGoods->setObjectName("btnGetGoods");

        gridLayout->addWidget(btnGetGoods, 3, 2, 1, 1);


        retranslateUi(DlgPreorderItem);

        QMetaObject::connectSlotsByName(DlgPreorderItem);
    } // setupUi

    void retranslateUi(QDialog *DlgPreorderItem)
    {
        DlgPreorderItem->setWindowTitle(QCoreApplication::translate("DlgPreorderItem", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("DlgPreorderItem", "UUID", nullptr));
        label_6->setText(QCoreApplication::translate("DlgPreorderItem", "\324\270\325\266\325\244\325\241\325\264\325\245\325\266\325\250", nullptr));
        label_3->setText(QCoreApplication::translate("DlgPreorderItem", "\324\261\325\266\325\276\325\241\325\266\325\270\326\202\325\264", nullptr));
        label_4->setText(QCoreApplication::translate("DlgPreorderItem", "\325\224\325\241\325\266\325\241\325\257", nullptr));
        label_2->setText(QCoreApplication::translate("DlgPreorderItem", "ID", nullptr));
        label_7->setText(QCoreApplication::translate("DlgPreorderItem", "\324\277\325\270\325\244", nullptr));
        label_5->setText(QCoreApplication::translate("DlgPreorderItem", "\324\263\325\253\325\266", nullptr));
        btnSave->setText(QCoreApplication::translate("DlgPreorderItem", "\325\212\325\241\325\260\325\272\325\241\325\266\325\245\325\254", nullptr));
        btnCancel->setText(QCoreApplication::translate("DlgPreorderItem", "\325\223\325\241\325\257\325\245\325\254", nullptr));
        btnGetGoods->setText(QCoreApplication::translate("DlgPreorderItem", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgPreorderItem: public Ui_DlgPreorderItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGPREORDERITEM_H
