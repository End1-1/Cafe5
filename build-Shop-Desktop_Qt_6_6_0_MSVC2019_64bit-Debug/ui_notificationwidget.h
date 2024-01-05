/********************************************************************************
** Form generated from reading UI file 'notificationwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NOTIFICATIONWIDGET_H
#define UI_NOTIFICATIONWIDGET_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NotificationWidget
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *wtime;
    QHBoxLayout *horizontalLayout;
    QLabel *lbInfo;
    QLabel *lbDataTime;
    QSpacerItem *horizontalSpacer;
    QToolButton *btnClose;
    QWidget *wmsg;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lbMessage;

    void setupUi(QWidget *NotificationWidget)
    {
        if (NotificationWidget->objectName().isEmpty())
            NotificationWidget->setObjectName("NotificationWidget");
        NotificationWidget->setWindowModality(Qt::WindowModal);
        NotificationWidget->resize(414, 155);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(NotificationWidget->sizePolicy().hasHeightForWidth());
        NotificationWidget->setSizePolicy(sizePolicy);
        NotificationWidget->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:1, y1:0, x2:1, y2:1, stop:0 rgba(0, 85, 127, 255), stop:0.462871 rgba(0, 124, 181, 255), stop:1 rgba(0, 85, 127, 255));\n"
"border-radius: 40px;"));
        verticalLayout = new QVBoxLayout(NotificationWidget);
        verticalLayout->setObjectName("verticalLayout");
        wtime = new QWidget(NotificationWidget);
        wtime->setObjectName("wtime");
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(wtime->sizePolicy().hasHeightForWidth());
        wtime->setSizePolicy(sizePolicy1);
        wtime->setStyleSheet(QString::fromUtf8("background:transparent"));
        horizontalLayout = new QHBoxLayout(wtime);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        lbInfo = new QLabel(wtime);
        lbInfo->setObjectName("lbInfo");
        sizePolicy.setHeightForWidth(lbInfo->sizePolicy().hasHeightForWidth());
        lbInfo->setSizePolicy(sizePolicy);
        lbInfo->setMaximumSize(QSize(25, 25));
        lbInfo->setPixmap(QPixmap(QString::fromUtf8(":/info.png")));
        lbInfo->setScaledContents(true);

        horizontalLayout->addWidget(lbInfo);

        lbDataTime = new QLabel(wtime);
        lbDataTime->setObjectName("lbDataTime");
        QFont font;
        font.setBold(true);
        lbDataTime->setFont(font);
        lbDataTime->setStyleSheet(QString::fromUtf8("background:transparent;\n"
"color:white;"));

        horizontalLayout->addWidget(lbDataTime);

        horizontalSpacer = new QSpacerItem(215, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnClose = new QToolButton(wtime);
        btnClose->setObjectName("btnClose");
        sizePolicy.setHeightForWidth(btnClose->sizePolicy().hasHeightForWidth());
        btnClose->setSizePolicy(sizePolicy);
        btnClose->setFocusPolicy(Qt::StrongFocus);
        btnClose->setStyleSheet(QString::fromUtf8("background-color: rgba(255, 255, 255, 0);"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/cancel.png"), QSize(), QIcon::Normal, QIcon::Off);
        btnClose->setIcon(icon);
        btnClose->setIconSize(QSize(32, 32));
        btnClose->setAutoRaise(true);

        horizontalLayout->addWidget(btnClose);


        verticalLayout->addWidget(wtime);

        wmsg = new QWidget(NotificationWidget);
        wmsg->setObjectName("wmsg");
        wmsg->setStyleSheet(QString::fromUtf8("background:transparent"));
        horizontalLayout_2 = new QHBoxLayout(wmsg);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        lbMessage = new QLabel(wmsg);
        lbMessage->setObjectName("lbMessage");
        QFont font1;
        font1.setPointSize(12);
        font1.setBold(true);
        lbMessage->setFont(font1);
        lbMessage->setStyleSheet(QString::fromUtf8("background:transparent;\n"
"color:rgb(255, 170, 0);"));
        lbMessage->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        lbMessage->setWordWrap(true);

        horizontalLayout_2->addWidget(lbMessage);


        verticalLayout->addWidget(wmsg);


        retranslateUi(NotificationWidget);

        QMetaObject::connectSlotsByName(NotificationWidget);
    } // setupUi

    void retranslateUi(QWidget *NotificationWidget)
    {
        NotificationWidget->setWindowTitle(QCoreApplication::translate("NotificationWidget", "Message", nullptr));
        lbInfo->setText(QString());
        lbDataTime->setText(QCoreApplication::translate("NotificationWidget", "27.07.2019 22:16", nullptr));
        btnClose->setText(QString());
        lbMessage->setText(QCoreApplication::translate("NotificationWidget", "The message!", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NotificationWidget: public Ui_NotificationWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NOTIFICATIONWIDGET_H
