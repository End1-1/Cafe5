/********************************************************************************
** Form generated from reading UI file 'userphoto.ui'
**
** Created by: Qt User Interface Compiler version 6.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERPHOTO_H
#define UI_USERPHOTO_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UserPhoto
{
public:
    QHBoxLayout *horizontalLayout;
    QFrame *frame;
    QGridLayout *gridLayout_2;
    QLabel *lbName;
    QLabel *lbImage;
    QLabel *lbNum;

    void setupUi(QWidget *UserPhoto)
    {
        if (UserPhoto->objectName().isEmpty())
            UserPhoto->setObjectName("UserPhoto");
        UserPhoto->resize(151, 180);
        UserPhoto->setMinimumSize(QSize(151, 180));
        UserPhoto->setMaximumSize(QSize(154, 195));
        horizontalLayout = new QHBoxLayout(UserPhoto);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        frame = new QFrame(UserPhoto);
        frame->setObjectName("frame");
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Sunken);
        gridLayout_2 = new QGridLayout(frame);
        gridLayout_2->setObjectName("gridLayout_2");
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        lbName = new QLabel(frame);
        lbName->setObjectName("lbName");
        lbName->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(lbName, 1, 0, 1, 2);

        lbImage = new QLabel(frame);
        lbImage->setObjectName("lbImage");
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(3);
        sizePolicy.setHeightForWidth(lbImage->sizePolicy().hasHeightForWidth());
        lbImage->setSizePolicy(sizePolicy);
        lbImage->setMinimumSize(QSize(150, 150));
        lbImage->setFrameShape(QFrame::NoFrame);
        lbImage->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(lbImage, 0, 0, 1, 2);

        lbNum = new QLabel(frame);
        lbNum->setObjectName("lbNum");
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setItalic(false);
        lbNum->setFont(font);
        lbNum->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(lbNum, 2, 0, 1, 2);


        horizontalLayout->addWidget(frame);


        retranslateUi(UserPhoto);

        QMetaObject::connectSlotsByName(UserPhoto);
    } // setupUi

    void retranslateUi(QWidget *UserPhoto)
    {
        UserPhoto->setWindowTitle(QCoreApplication::translate("UserPhoto", "Form", nullptr));
        lbName->setText(QCoreApplication::translate("UserPhoto", "Name", nullptr));
        lbImage->setText(QCoreApplication::translate("UserPhoto", "Image", nullptr));
        lbNum->setText(QCoreApplication::translate("UserPhoto", "000111222", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UserPhoto: public Ui_UserPhoto {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERPHOTO_H
