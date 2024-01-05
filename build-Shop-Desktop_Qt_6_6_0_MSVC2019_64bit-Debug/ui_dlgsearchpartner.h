/********************************************************************************
** Form generated from reading UI file 'dlgsearchpartner.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGSEARCHPARTNER_H
#define UI_DLGSEARCHPARTNER_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DlgSearchPartner
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btnAddPartner;
    QLineEdit *lineEdit;
    QWidget *waddpartner;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *leTaxcode;
    QLabel *label_2;
    QLineEdit *leTaxName;
    QLabel *label_3;
    QLineEdit *leContact;
    QLabel *label_4;
    QLineEdit *lePhone;
    QPushButton *btnSave;
    QPushButton *btnCancel;
    QTableWidget *tbl;

    void setupUi(QDialog *DlgSearchPartner)
    {
        if (DlgSearchPartner->objectName().isEmpty())
            DlgSearchPartner->setObjectName("DlgSearchPartner");
        DlgSearchPartner->resize(916, 539);
        verticalLayout = new QVBoxLayout(DlgSearchPartner);
        verticalLayout->setObjectName("verticalLayout");
        widget_2 = new QWidget(DlgSearchPartner);
        widget_2->setObjectName("widget_2");
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        btnAddPartner = new QPushButton(widget_2);
        btnAddPartner->setObjectName("btnAddPartner");
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btnAddPartner->sizePolicy().hasHeightForWidth());
        btnAddPartner->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/add.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnAddPartner->setIcon(icon);

        horizontalLayout_2->addWidget(btnAddPartner);

        lineEdit = new QLineEdit(widget_2);
        lineEdit->setObjectName("lineEdit");

        horizontalLayout_2->addWidget(lineEdit);


        verticalLayout->addWidget(widget_2);

        waddpartner = new QWidget(DlgSearchPartner);
        waddpartner->setObjectName("waddpartner");
        horizontalLayout = new QHBoxLayout(waddpartner);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(waddpartner);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        leTaxcode = new QLineEdit(waddpartner);
        leTaxcode->setObjectName("leTaxcode");

        horizontalLayout->addWidget(leTaxcode);

        label_2 = new QLabel(waddpartner);
        label_2->setObjectName("label_2");

        horizontalLayout->addWidget(label_2);

        leTaxName = new QLineEdit(waddpartner);
        leTaxName->setObjectName("leTaxName");

        horizontalLayout->addWidget(leTaxName);

        label_3 = new QLabel(waddpartner);
        label_3->setObjectName("label_3");

        horizontalLayout->addWidget(label_3);

        leContact = new QLineEdit(waddpartner);
        leContact->setObjectName("leContact");

        horizontalLayout->addWidget(leContact);

        label_4 = new QLabel(waddpartner);
        label_4->setObjectName("label_4");

        horizontalLayout->addWidget(label_4);

        lePhone = new QLineEdit(waddpartner);
        lePhone->setObjectName("lePhone");

        horizontalLayout->addWidget(lePhone);

        btnSave = new QPushButton(waddpartner);
        btnSave->setObjectName("btnSave");
        sizePolicy.setHeightForWidth(btnSave->sizePolicy().hasHeightForWidth());
        btnSave->setSizePolicy(sizePolicy);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/save.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnSave->setIcon(icon1);

        horizontalLayout->addWidget(btnSave);

        btnCancel = new QPushButton(waddpartner);
        btnCancel->setObjectName("btnCancel");
        sizePolicy.setHeightForWidth(btnCancel->sizePolicy().hasHeightForWidth());
        btnCancel->setSizePolicy(sizePolicy);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnCancel->setIcon(icon2);

        horizontalLayout->addWidget(btnCancel);


        verticalLayout->addWidget(waddpartner);

        tbl = new QTableWidget(DlgSearchPartner);
        if (tbl->columnCount() < 5)
            tbl->setColumnCount(5);
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
        tbl->setObjectName("tbl");
        tbl->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tbl->setSelectionMode(QAbstractItemView::SingleSelection);
        tbl->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout->addWidget(tbl);

        QWidget::setTabOrder(lineEdit, leTaxcode);
        QWidget::setTabOrder(leTaxcode, leTaxName);
        QWidget::setTabOrder(leTaxName, leContact);
        QWidget::setTabOrder(leContact, lePhone);
        QWidget::setTabOrder(lePhone, btnSave);
        QWidget::setTabOrder(btnSave, tbl);
        QWidget::setTabOrder(tbl, btnAddPartner);

        retranslateUi(DlgSearchPartner);

        QMetaObject::connectSlotsByName(DlgSearchPartner);
    } // setupUi

    void retranslateUi(QDialog *DlgSearchPartner)
    {
        DlgSearchPartner->setWindowTitle(QCoreApplication::translate("DlgSearchPartner", "Dialog", nullptr));
        btnAddPartner->setText(QString());
        label->setText(QCoreApplication::translate("DlgSearchPartner", "Taxpayer code", nullptr));
        label_2->setText(QCoreApplication::translate("DlgSearchPartner", "Tax name", nullptr));
        label_3->setText(QCoreApplication::translate("DlgSearchPartner", "Contact", nullptr));
        label_4->setText(QCoreApplication::translate("DlgSearchPartner", "Phone", nullptr));
        btnSave->setText(QString());
        btnCancel->setText(QString());
        QTableWidgetItem *___qtablewidgetitem = tbl->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("DlgSearchPartner", "Id", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tbl->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("DlgSearchPartner", "Taxpayer code", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = tbl->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("DlgSearchPartner", "New Column", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = tbl->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QCoreApplication::translate("DlgSearchPartner", "Taxpayer name", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = tbl->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QCoreApplication::translate("DlgSearchPartner", "Phone", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DlgSearchPartner: public Ui_DlgSearchPartner {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGSEARCHPARTNER_H
