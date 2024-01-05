/********************************************************************************
** Form generated from reading UI file 'c5printpreview.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C5PRINTPREVIEW_H
#define UI_C5PRINTPREVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <c5graphicsview.h>

QT_BEGIN_NAMESPACE

class Ui_C5PrintPreview
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *btnFirst;
    QPushButton *btnBack;
    QLabel *lbPage;
    QPushButton *btnNext;
    QPushButton *btnLast;
    QPushButton *btnZoomOut;
    QPushButton *btnZoopIn;
    QComboBox *cbZoom;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QComboBox *cbPrintSelection;
    QLineEdit *lePages;
    QComboBox *cbPrinters;
    QPushButton *btnPrint;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout_2;
    QScrollArea *saMain;
    QWidget *scrollAreaWidgetContents;
    QHBoxLayout *horizontalLayout_3;
    C5GraphicsView *gv;

    void setupUi(QDialog *C5PrintPreview)
    {
        if (C5PrintPreview->objectName().isEmpty())
            C5PrintPreview->setObjectName("C5PrintPreview");
        C5PrintPreview->resize(1271, 714);
        verticalLayout = new QVBoxLayout(C5PrintPreview);
        verticalLayout->setObjectName("verticalLayout");
        widget = new QWidget(C5PrintPreview);
        widget->setObjectName("widget");
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        btnFirst = new QPushButton(widget);
        btnFirst->setObjectName("btnFirst");
        btnFirst->setMinimumSize(QSize(30, 30));
        btnFirst->setMaximumSize(QSize(30, 30));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/double-left.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnFirst->setIcon(icon);
        btnFirst->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(btnFirst);

        btnBack = new QPushButton(widget);
        btnBack->setObjectName("btnBack");
        btnBack->setMinimumSize(QSize(30, 30));
        btnBack->setMaximumSize(QSize(30, 30));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/left-arrow.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnBack->setIcon(icon1);
        btnBack->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(btnBack);

        lbPage = new QLabel(widget);
        lbPage->setObjectName("lbPage");

        horizontalLayout->addWidget(lbPage);

        btnNext = new QPushButton(widget);
        btnNext->setObjectName("btnNext");
        btnNext->setMinimumSize(QSize(30, 30));
        btnNext->setMaximumSize(QSize(30, 30));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/right-arrow.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnNext->setIcon(icon2);
        btnNext->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(btnNext);

        btnLast = new QPushButton(widget);
        btnLast->setObjectName("btnLast");
        btnLast->setMinimumSize(QSize(30, 30));
        btnLast->setMaximumSize(QSize(30, 30));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/double-right.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnLast->setIcon(icon3);
        btnLast->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(btnLast);

        btnZoomOut = new QPushButton(widget);
        btnZoomOut->setObjectName("btnZoomOut");
        btnZoomOut->setMinimumSize(QSize(30, 30));
        btnZoomOut->setMaximumSize(QSize(30, 30));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/zoom-out.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnZoomOut->setIcon(icon4);
        btnZoomOut->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(btnZoomOut);

        btnZoopIn = new QPushButton(widget);
        btnZoopIn->setObjectName("btnZoopIn");
        btnZoopIn->setMinimumSize(QSize(30, 30));
        btnZoopIn->setMaximumSize(QSize(30, 30));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/zoom-in.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnZoopIn->setIcon(icon5);
        btnZoopIn->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(btnZoopIn);

        cbZoom = new QComboBox(widget);
        cbZoom->addItem(QString());
        cbZoom->addItem(QString());
        cbZoom->addItem(QString());
        cbZoom->addItem(QString());
        cbZoom->addItem(QString());
        cbZoom->addItem(QString());
        cbZoom->addItem(QString());
        cbZoom->addItem(QString());
        cbZoom->setObjectName("cbZoom");

        horizontalLayout->addWidget(cbZoom);

        horizontalSpacer = new QSpacerItem(417, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label = new QLabel(widget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        cbPrintSelection = new QComboBox(widget);
        cbPrintSelection->addItem(QString());
        cbPrintSelection->addItem(QString());
        cbPrintSelection->addItem(QString());
        cbPrintSelection->setObjectName("cbPrintSelection");

        horizontalLayout->addWidget(cbPrintSelection);

        lePages = new QLineEdit(widget);
        lePages->setObjectName("lePages");
        lePages->setEnabled(false);
        lePages->setMinimumSize(QSize(100, 0));

        horizontalLayout->addWidget(lePages);

        cbPrinters = new QComboBox(widget);
        cbPrinters->setObjectName("cbPrinters");
        cbPrinters->setMinimumSize(QSize(200, 0));

        horizontalLayout->addWidget(cbPrinters);

        btnPrint = new QPushButton(widget);
        btnPrint->setObjectName("btnPrint");
        btnPrint->setMinimumSize(QSize(30, 30));
        btnPrint->setMaximumSize(QSize(30, 30));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/print.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnPrint->setIcon(icon6);
        btnPrint->setIconSize(QSize(24, 24));

        horizontalLayout->addWidget(btnPrint);


        verticalLayout->addWidget(widget);

        widget_2 = new QWidget(C5PrintPreview);
        widget_2->setObjectName("widget_2");
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy1);
        horizontalLayout_2 = new QHBoxLayout(widget_2);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 2);
        saMain = new QScrollArea(widget_2);
        saMain->setObjectName("saMain");
        saMain->setFrameShape(QFrame::Box);
        saMain->setWidgetResizable(true);
        saMain->setAlignment(Qt::AlignCenter);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 1245, 649));
        horizontalLayout_3 = new QHBoxLayout(scrollAreaWidgetContents);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(25, 25, 9, 0);
        gv = new C5GraphicsView(scrollAreaWidgetContents);
        gv->setObjectName("gv");
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(gv->sizePolicy().hasHeightForWidth());
        gv->setSizePolicy(sizePolicy2);
        gv->setFrameShape(QFrame::NoFrame);
        gv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        gv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        gv->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        gv->setTransformationAnchor(QGraphicsView::NoAnchor);
        gv->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

        horizontalLayout_3->addWidget(gv);

        saMain->setWidget(scrollAreaWidgetContents);

        horizontalLayout_2->addWidget(saMain);


        verticalLayout->addWidget(widget_2);


        retranslateUi(C5PrintPreview);

        cbZoom->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(C5PrintPreview);
    } // setupUi

    void retranslateUi(QDialog *C5PrintPreview)
    {
        C5PrintPreview->setWindowTitle(QCoreApplication::translate("C5PrintPreview", "Print preview", nullptr));
        btnFirst->setText(QString());
        btnBack->setText(QString());
        lbPage->setText(QCoreApplication::translate("C5PrintPreview", "Page 1 of 1", nullptr));
        btnNext->setText(QString());
        btnLast->setText(QString());
        btnZoomOut->setText(QString());
        btnZoopIn->setText(QString());
        cbZoom->setItemText(0, QCoreApplication::translate("C5PrintPreview", "200%", nullptr));
        cbZoom->setItemText(1, QCoreApplication::translate("C5PrintPreview", "175%", nullptr));
        cbZoom->setItemText(2, QCoreApplication::translate("C5PrintPreview", "150%", nullptr));
        cbZoom->setItemText(3, QCoreApplication::translate("C5PrintPreview", "125%", nullptr));
        cbZoom->setItemText(4, QCoreApplication::translate("C5PrintPreview", "100%", nullptr));
        cbZoom->setItemText(5, QCoreApplication::translate("C5PrintPreview", "75%", nullptr));
        cbZoom->setItemText(6, QCoreApplication::translate("C5PrintPreview", "50%", nullptr));
        cbZoom->setItemText(7, QCoreApplication::translate("C5PrintPreview", "25%", nullptr));

        label->setText(QCoreApplication::translate("C5PrintPreview", "Print", nullptr));
        cbPrintSelection->setItemText(0, QCoreApplication::translate("C5PrintPreview", "All pages", nullptr));
        cbPrintSelection->setItemText(1, QCoreApplication::translate("C5PrintPreview", "Current page", nullptr));
        cbPrintSelection->setItemText(2, QCoreApplication::translate("C5PrintPreview", "Custom", nullptr));

        btnPrint->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class C5PrintPreview: public Ui_C5PrintPreview {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5PRINTPREVIEW_H
