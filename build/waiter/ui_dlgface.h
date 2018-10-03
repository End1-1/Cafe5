/********************************************************************************
** Form generated from reading UI file 'dlgface.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DLGFACE_H
#define UI_DLGFACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DlgFace
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_3;
    QHBoxLayout *horizontalLayout_3;
    QTableWidget *tableWidget_2;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QTableWidget *tblHall;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QToolButton *btnExit;
    QToolButton *btnConnection;
    QSpacerItem *horizontalSpacer;

    void setupUi(QDialog *DlgFace)
    {
        if (DlgFace->objectName().isEmpty())
            DlgFace->setObjectName(QStringLiteral("DlgFace"));
        DlgFace->resize(1022, 702);
        verticalLayout = new QVBoxLayout(DlgFace);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        widget_3 = new QWidget(DlgFace);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        horizontalLayout_3 = new QHBoxLayout(widget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        tableWidget_2 = new QTableWidget(widget_3);
        tableWidget_2->setObjectName(QStringLiteral("tableWidget_2"));
        tableWidget_2->setMaximumSize(QSize(16777215, 50));

        horizontalLayout_3->addWidget(tableWidget_2);


        verticalLayout->addWidget(widget_3);

        widget_2 = new QWidget(DlgFace);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy);
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        tblHall = new QTableWidget(widget_2);
        tblHall->setObjectName(QStringLiteral("tblHall"));
        tblHall->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblHall->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tblHall->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tblHall->setSelectionMode(QAbstractItemView::NoSelection);
        tblHall->horizontalHeader()->setVisible(false);
        tblHall->verticalHeader()->setVisible(false);

        horizontalLayout_2->addWidget(tblHall);


        verticalLayout->addWidget(widget_2);

        widget = new QWidget(DlgFace);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        btnExit = new QToolButton(widget);
        btnExit->setObjectName(QStringLiteral("btnExit"));
        btnExit->setMinimumSize(QSize(48, 48));
        btnExit->setMaximumSize(QSize(48, 48));
        QIcon icon;
        icon.addFile(QStringLiteral(":/exit.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnExit->setIcon(icon);
        btnExit->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(btnExit);

        btnConnection = new QToolButton(widget);
        btnConnection->setObjectName(QStringLiteral("btnConnection"));
        btnConnection->setMinimumSize(QSize(48, 48));
        btnConnection->setMaximumSize(QSize(48, 48));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/configure.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnConnection->setIcon(icon1);
        btnConnection->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(btnConnection);

        horizontalSpacer = new QSpacerItem(875, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addWidget(widget);


        retranslateUi(DlgFace);

        QMetaObject::connectSlotsByName(DlgFace);
    } // setupUi

    void retranslateUi(QDialog *DlgFace)
    {
        DlgFace->setWindowTitle(QApplication::translate("DlgFace", "DlgFace", nullptr));
        btnExit->setText(QString());
        btnConnection->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class DlgFace: public Ui_DlgFace {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DLGFACE_H
