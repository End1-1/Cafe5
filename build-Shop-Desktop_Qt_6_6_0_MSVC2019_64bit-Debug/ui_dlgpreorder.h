/********************************************************************************
** Form generated from reading UI file 'dlgpreorder.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGPREORDER_H
#define UI_DLGPREORDER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>
#include <c5dateedit.h>
#include <c5lineedit.h>

QT_BEGIN_NAMESPACE

class Ui_DlgPreorder
{
public:
    QGridLayout *gridLayout;
    QTableWidget *tbl;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout;
    QToolButton *btnNew;
    QToolButton *btnEdit;
    QSpacerItem *horizontalSpacer;
    QCheckBox *chShowAll;
    QToolButton *btnClose;
    QWidget *winfo;
    QGridLayout *gridLayout_3;
    QLabel *label_5;
    C5DateEdit *leDateFor;
    QLabel *label_9;
    QLabel *label_7;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer_3;
    QWidget *widget_2;
    QGridLayout *gridLayout_2;
    QPushButton *btnSave;
    QPushButton *btnCreateSale;
    QPushButton *btnPrintFiscal;
    C5LineEdit *lePrepaidCart;
    QComboBox *cbState;
    QTableWidget *tgoods;
    QWidget *widget_4;
    QHBoxLayout *horizontalLayout_3;
    QToolButton *btnAddGoods;
    QToolButton *btnEditGoods;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_8;
    QLabel *label_2;
    C5LineEdit *leUUID;
    C5LineEdit *lePhone;
    QLabel *label;
    C5LineEdit *lePrepaidFiscal;
    C5LineEdit *leCustomer;
    C5DateEdit *leDateCreated;
    C5LineEdit *lePrepaidCash;
    QLabel *label_10;
    QLabel *label_6;

    void setupUi(QDialog *DlgPreorder)
    {
        if (DlgPreorder->objectName().isEmpty())
            DlgPreorder->setObjectName("DlgPreorder");
        DlgPreorder->resize(988, 686);
        gridLayout = new QGridLayout(DlgPreorder);
        gridLayout->setObjectName("gridLayout");
        tbl = new QTableWidget(DlgPreorder);
        if (tbl->columnCount() < 9)
            tbl->setColumnCount(9);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        QTableWidgetItem *__qtablewidgetitem8 = new QTableWidgetItem();
        tbl->setHorizontalHeaderItem(8, __qtablewidgetitem8);
        tbl->setObjectName("tbl");
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setSelectionMode(QAbstractItemView::SingleSelection);
        tbl->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout->addWidget(tbl, 1, 0, 1, 2);

        widget_3 = new QWidget(DlgPreorder);
        widget_3->setObjectName("widget_3");
        widget_3->setMinimumSize(QSize(0, 0));
        horizontalLayout = new QHBoxLayout(widget_3);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btnNew = new QToolButton(widget_3);
        btnNew->setObjectName("btnNew");
        btnNew->setMinimumSize(QSize(30, 30));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/add.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnNew->setIcon(icon);

        horizontalLayout->addWidget(btnNew);

        btnEdit = new QToolButton(widget_3);
        btnEdit->setObjectName("btnEdit");
        btnEdit->setMinimumSize(QSize(30, 30));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/constructor.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnEdit->setIcon(icon1);

        horizontalLayout->addWidget(btnEdit);

        horizontalSpacer = new QSpacerItem(740, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        chShowAll = new QCheckBox(widget_3);
        chShowAll->setObjectName("chShowAll");

        horizontalLayout->addWidget(chShowAll);

        btnClose = new QToolButton(widget_3);
        btnClose->setObjectName("btnClose");
        btnClose->setMinimumSize(QSize(30, 30));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/logout.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnClose->setIcon(icon2);

        horizontalLayout->addWidget(btnClose);


        gridLayout->addWidget(widget_3, 0, 0, 1, 2);

        winfo = new QWidget(DlgPreorder);
        winfo->setObjectName("winfo");
        winfo->setMaximumSize(QSize(16777215, 16777215));
        gridLayout_3 = new QGridLayout(winfo);
        gridLayout_3->setObjectName("gridLayout_3");
        label_5 = new QLabel(winfo);
        label_5->setObjectName("label_5");

        gridLayout_3->addWidget(label_5, 7, 0, 1, 1);

        leDateFor = new C5DateEdit(winfo);
        leDateFor->setObjectName("leDateFor");

        gridLayout_3->addWidget(leDateFor, 3, 1, 1, 1);

        label_9 = new QLabel(winfo);
        label_9->setObjectName("label_9");

        gridLayout_3->addWidget(label_9, 8, 0, 1, 1);

        label_7 = new QLabel(winfo);
        label_7->setObjectName("label_7");

        gridLayout_3->addWidget(label_7, 1, 0, 1, 1);

        label_3 = new QLabel(winfo);
        label_3->setObjectName("label_3");

        gridLayout_3->addWidget(label_3, 6, 0, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_3, 2, 2, 1, 1);

        widget_2 = new QWidget(winfo);
        widget_2->setObjectName("widget_2");
        widget_2->setMinimumSize(QSize(0, 0));
        gridLayout_2 = new QGridLayout(widget_2);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        btnSave = new QPushButton(widget_2);
        btnSave->setObjectName("btnSave");

        gridLayout_2->addWidget(btnSave, 0, 0, 1, 2);

        btnCreateSale = new QPushButton(widget_2);
        btnCreateSale->setObjectName("btnCreateSale");

        gridLayout_2->addWidget(btnCreateSale, 2, 0, 1, 3);

        btnPrintFiscal = new QPushButton(widget_2);
        btnPrintFiscal->setObjectName("btnPrintFiscal");

        gridLayout_2->addWidget(btnPrintFiscal, 0, 2, 1, 1);


        gridLayout_3->addWidget(widget_2, 10, 0, 1, 2);

        lePrepaidCart = new C5LineEdit(winfo);
        lePrepaidCart->setObjectName("lePrepaidCart");

        gridLayout_3->addWidget(lePrepaidCart, 8, 1, 1, 1);

        cbState = new QComboBox(winfo);
        cbState->addItem(QString());
        cbState->addItem(QString());
        cbState->addItem(QString());
        cbState->setObjectName("cbState");

        gridLayout_3->addWidget(cbState, 2, 1, 1, 1);

        tgoods = new QTableWidget(winfo);
        if (tgoods->columnCount() < 7)
            tgoods->setColumnCount(7);
        QTableWidgetItem *__qtablewidgetitem9 = new QTableWidgetItem();
        tgoods->setHorizontalHeaderItem(0, __qtablewidgetitem9);
        QTableWidgetItem *__qtablewidgetitem10 = new QTableWidgetItem();
        tgoods->setHorizontalHeaderItem(1, __qtablewidgetitem10);
        QTableWidgetItem *__qtablewidgetitem11 = new QTableWidgetItem();
        tgoods->setHorizontalHeaderItem(2, __qtablewidgetitem11);
        QTableWidgetItem *__qtablewidgetitem12 = new QTableWidgetItem();
        tgoods->setHorizontalHeaderItem(3, __qtablewidgetitem12);
        QTableWidgetItem *__qtablewidgetitem13 = new QTableWidgetItem();
        tgoods->setHorizontalHeaderItem(4, __qtablewidgetitem13);
        QTableWidgetItem *__qtablewidgetitem14 = new QTableWidgetItem();
        tgoods->setHorizontalHeaderItem(5, __qtablewidgetitem14);
        QTableWidgetItem *__qtablewidgetitem15 = new QTableWidgetItem();
        tgoods->setHorizontalHeaderItem(6, __qtablewidgetitem15);
        tgoods->setObjectName("tgoods");
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(tgoods->sizePolicy().hasHeightForWidth());
        tgoods->setSizePolicy(sizePolicy);
        tgoods->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tgoods->setSelectionMode(QAbstractItemView::SingleSelection);
        tgoods->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout_3->addWidget(tgoods, 12, 0, 1, 3);

        widget_4 = new QWidget(winfo);
        widget_4->setObjectName("widget_4");
        widget_4->setMinimumSize(QSize(0, 0));
        horizontalLayout_3 = new QHBoxLayout(widget_4);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        btnAddGoods = new QToolButton(widget_4);
        btnAddGoods->setObjectName("btnAddGoods");
        btnAddGoods->setMinimumSize(QSize(30, 30));
        btnAddGoods->setIcon(icon);

        horizontalLayout_3->addWidget(btnAddGoods);

        btnEditGoods = new QToolButton(widget_4);
        btnEditGoods->setObjectName("btnEditGoods");
        btnEditGoods->setMinimumSize(QSize(30, 30));
        btnEditGoods->setIcon(icon1);

        horizontalLayout_3->addWidget(btnEditGoods);

        horizontalSpacer_2 = new QSpacerItem(877, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        gridLayout_3->addWidget(widget_4, 11, 0, 1, 3);

        label_8 = new QLabel(winfo);
        label_8->setObjectName("label_8");

        gridLayout_3->addWidget(label_8, 3, 0, 1, 1);

        label_2 = new QLabel(winfo);
        label_2->setObjectName("label_2");

        gridLayout_3->addWidget(label_2, 4, 0, 1, 1);

        leUUID = new C5LineEdit(winfo);
        leUUID->setObjectName("leUUID");
        leUUID->setReadOnly(true);

        gridLayout_3->addWidget(leUUID, 0, 1, 1, 1);

        lePhone = new C5LineEdit(winfo);
        lePhone->setObjectName("lePhone");

        gridLayout_3->addWidget(lePhone, 6, 1, 1, 1);

        label = new QLabel(winfo);
        label->setObjectName("label");

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        lePrepaidFiscal = new C5LineEdit(winfo);
        lePrepaidFiscal->setObjectName("lePrepaidFiscal");
        lePrepaidFiscal->setReadOnly(true);

        gridLayout_3->addWidget(lePrepaidFiscal, 9, 1, 1, 1);

        leCustomer = new C5LineEdit(winfo);
        leCustomer->setObjectName("leCustomer");

        gridLayout_3->addWidget(leCustomer, 4, 1, 1, 1);

        leDateCreated = new C5DateEdit(winfo);
        leDateCreated->setObjectName("leDateCreated");

        gridLayout_3->addWidget(leDateCreated, 1, 1, 1, 1);

        lePrepaidCash = new C5LineEdit(winfo);
        lePrepaidCash->setObjectName("lePrepaidCash");

        gridLayout_3->addWidget(lePrepaidCash, 7, 1, 1, 1);

        label_10 = new QLabel(winfo);
        label_10->setObjectName("label_10");

        gridLayout_3->addWidget(label_10, 2, 0, 1, 1);

        label_6 = new QLabel(winfo);
        label_6->setObjectName("label_6");

        gridLayout_3->addWidget(label_6, 9, 0, 1, 1);


        gridLayout->addWidget(winfo, 3, 0, 1, 1);


        retranslateUi(DlgPreorder);

        QMetaObject::connectSlotsByName(DlgPreorder);
    } // setupUi

    void retranslateUi(QDialog *DlgPreorder)
    {
        DlgPreorder->setWindowTitle(QCoreApplication::translate("DlgPreorder", "Dialog", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("DlgPreorder", "UUID", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("DlgPreorder", "\325\216\325\253\325\263\325\241\325\257", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("DlgPreorder", "\324\263\326\200\325\241\325\266\326\201\325\264\325\241\325\266 \326\205\326\200", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("DlgPreorder", "\324\277\325\241\325\277\325\241\326\200\325\264\325\241\325\266 \326\205\326\200", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("DlgPreorder", "\325\200\325\241\325\263\325\241\325\255\325\270\326\200\325\244", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = tbl->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QCoreApplication::translate("DlgPreorder", "\325\200\325\245\325\274\325\241\325\255\325\270\325\275", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = tbl->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QCoreApplication::translate("DlgPreorder", "\324\277\325\241\325\266\325\255\325\253\325\257 \325\257\325\241\325\266\325\255\325\241\325\276\325\263\325\241\326\200", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = tbl->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QCoreApplication::translate("DlgPreorder", "\325\224\325\241\326\200\325\277 \325\257\325\241\325\266\325\255\325\241\325\276\325\263\325\241\326\200", nullptr));
        QTableWidgetItem *___qtablewidgetitem8 = tbl->horizontalHeaderItem(8);
        ___qtablewidgetitem8->setText(QCoreApplication::translate("DlgPreorder", "\325\200\324\264\325\204", nullptr));
        btnNew->setText(QCoreApplication::translate("DlgPreorder", "...", nullptr));
        btnEdit->setText(QCoreApplication::translate("DlgPreorder", "...", nullptr));
        chShowAll->setText(QCoreApplication::translate("DlgPreorder", "\325\221\325\270\326\202\326\201\325\241\325\244\326\200\325\245\325\254 \325\242\325\270\325\254\325\270\326\200\325\250", nullptr));
        btnClose->setText(QCoreApplication::translate("DlgPreorder", "...", nullptr));
        label_5->setText(QCoreApplication::translate("DlgPreorder", "\324\277\325\241\325\266\325\255\325\241\325\276\325\263\325\241\326\200 \325\257\325\241\325\266\325\255\325\253\325\257\325\270\325\276", nullptr));
        label_9->setText(QCoreApplication::translate("DlgPreorder", "\324\277\325\241\325\266\325\255\325\241\325\276\325\263\325\241\325\274 \326\204\325\241\326\200\325\277\325\270\325\276", nullptr));
        label_7->setText(QCoreApplication::translate("DlgPreorder", "\324\263\326\200\325\241\325\266\326\201\325\264\325\241\325\266 \326\205\326\200", nullptr));
        label_3->setText(QCoreApplication::translate("DlgPreorder", "\325\200\325\245\325\274\325\241\325\255\325\270\325\275\325\253 \325\260\325\241\325\264\325\241\326\200", nullptr));
        btnSave->setText(QCoreApplication::translate("DlgPreorder", "\325\212\325\241\325\260\325\272\325\241\325\266\325\245\325\254", nullptr));
        btnCreateSale->setText(QCoreApplication::translate("DlgPreorder", "\324\277\325\241\325\277\325\241\326\200\325\245\325\254 \325\276\325\241\325\263\325\241\325\274\326\204", nullptr));
        btnPrintFiscal->setText(QCoreApplication::translate("DlgPreorder", "\325\217\325\272\325\245\325\254 \324\277\325\241\325\266\325\255\325\241\325\276\325\263\325\241\326\200\325\253 \325\200\324\264\325\204", nullptr));
        cbState->setItemText(0, QCoreApplication::translate("DlgPreorder", "\324\270\325\266\325\251\325\241\326\201\325\253\325\257", nullptr));
        cbState->setItemText(1, QCoreApplication::translate("DlgPreorder", "\324\277\325\241\325\277\325\241\326\200\325\276\325\241\325\256", nullptr));
        cbState->setItemText(2, QCoreApplication::translate("DlgPreorder", "\325\211\325\245\325\262\325\241\326\200\325\257\325\276\325\241\325\256", nullptr));

        QTableWidgetItem *___qtablewidgetitem9 = tgoods->horizontalHeaderItem(0);
        ___qtablewidgetitem9->setText(QCoreApplication::translate("DlgPreorder", "UUID", nullptr));
        QTableWidgetItem *___qtablewidgetitem10 = tgoods->horizontalHeaderItem(1);
        ___qtablewidgetitem10->setText(QCoreApplication::translate("DlgPreorder", "Goods id", nullptr));
        QTableWidgetItem *___qtablewidgetitem11 = tgoods->horizontalHeaderItem(2);
        ___qtablewidgetitem11->setText(QCoreApplication::translate("DlgPreorder", "\324\277\325\270\325\244", nullptr));
        QTableWidgetItem *___qtablewidgetitem12 = tgoods->horizontalHeaderItem(3);
        ___qtablewidgetitem12->setText(QCoreApplication::translate("DlgPreorder", "\324\261\325\266\325\276\325\241\325\266\325\270\326\202\325\264", nullptr));
        QTableWidgetItem *___qtablewidgetitem13 = tgoods->horizontalHeaderItem(4);
        ___qtablewidgetitem13->setText(QCoreApplication::translate("DlgPreorder", "\325\224\325\241\325\266\325\241\325\257", nullptr));
        QTableWidgetItem *___qtablewidgetitem14 = tgoods->horizontalHeaderItem(5);
        ___qtablewidgetitem14->setText(QCoreApplication::translate("DlgPreorder", "\324\263\325\253\325\266", nullptr));
        QTableWidgetItem *___qtablewidgetitem15 = tgoods->horizontalHeaderItem(6);
        ___qtablewidgetitem15->setText(QCoreApplication::translate("DlgPreorder", "\324\270\325\266\325\244\325\241\325\264\325\245\325\266\325\250", nullptr));
        btnAddGoods->setText(QCoreApplication::translate("DlgPreorder", "...", nullptr));
        btnEditGoods->setText(QCoreApplication::translate("DlgPreorder", "...", nullptr));
        label_8->setText(QCoreApplication::translate("DlgPreorder", "\324\277\325\241\325\277\325\241\326\200\325\264\325\241\325\266 \326\205\326\200", nullptr));
        label_2->setText(QCoreApplication::translate("DlgPreorder", "\325\200\325\241\325\263\325\241\325\255\325\270\326\200\325\244", nullptr));
        label->setText(QCoreApplication::translate("DlgPreorder", "UUID", nullptr));
        label_10->setText(QCoreApplication::translate("DlgPreorder", "\325\216\325\253\325\263\325\241\325\257", nullptr));
        label_6->setText(QCoreApplication::translate("DlgPreorder", "\325\200\324\264\325\204 \325\257\325\277\326\200\325\270\325\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgPreorder: public Ui_DlgPreorder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGPREORDER_H
