/********************************************************************************
** Form generated from reading UI file 'c5mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C5MAINWINDOW_H
#define UI_C5MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_C5MainWindow
{
public:
    QAction *actionLogin;
    QAction *actionLogout;
    QAction *action;
    QAction *actionConnection;
    QAction *actionAbout;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuAbout;
    QMenu *menuDirectory_of_menu;
    QMenu *menuDirectory_of_storage;
    QMenu *menuConfiguration_of_hal;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *C5MainWindow)
    {
        if (C5MainWindow->objectName().isEmpty())
            C5MainWindow->setObjectName(QStringLiteral("C5MainWindow"));
        C5MainWindow->resize(1018, 690);
        actionLogin = new QAction(C5MainWindow);
        actionLogin->setObjectName(QStringLiteral("actionLogin"));
        actionLogout = new QAction(C5MainWindow);
        actionLogout->setObjectName(QStringLiteral("actionLogout"));
        action = new QAction(C5MainWindow);
        action->setObjectName(QStringLiteral("action"));
        actionConnection = new QAction(C5MainWindow);
        actionConnection->setObjectName(QStringLiteral("actionConnection"));
        actionAbout = new QAction(C5MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        centralWidget = new QWidget(C5MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        tabWidget->addTab(tab, QString());

        horizontalLayout->addWidget(tabWidget);

        C5MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(C5MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1018, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuAbout = new QMenu(menuBar);
        menuAbout->setObjectName(QStringLiteral("menuAbout"));
        menuDirectory_of_menu = new QMenu(menuBar);
        menuDirectory_of_menu->setObjectName(QStringLiteral("menuDirectory_of_menu"));
        menuDirectory_of_storage = new QMenu(menuBar);
        menuDirectory_of_storage->setObjectName(QStringLiteral("menuDirectory_of_storage"));
        menuConfiguration_of_hal = new QMenu(menuBar);
        menuConfiguration_of_hal->setObjectName(QStringLiteral("menuConfiguration_of_hal"));
        C5MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(C5MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        C5MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(C5MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        C5MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuDirectory_of_menu->menuAction());
        menuBar->addAction(menuDirectory_of_storage->menuAction());
        menuBar->addAction(menuConfiguration_of_hal->menuAction());
        menuBar->addAction(menuAbout->menuAction());
        menuFile->addAction(actionLogin);
        menuFile->addAction(actionLogout);
        menuFile->addSeparator();
        menuFile->addAction(actionConnection);
        menuFile->addAction(action);
        menuAbout->addAction(actionAbout);

        retranslateUi(C5MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(C5MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *C5MainWindow)
    {
        C5MainWindow->setWindowTitle(QApplication::translate("C5MainWindow", "C5MainWindow", nullptr));
        actionLogin->setText(QApplication::translate("C5MainWindow", "Login", nullptr));
        actionLogout->setText(QApplication::translate("C5MainWindow", "Logout", nullptr));
        action->setText(QApplication::translate("C5MainWindow", "Quit", nullptr));
        actionConnection->setText(QApplication::translate("C5MainWindow", "Connection", nullptr));
        actionAbout->setText(QApplication::translate("C5MainWindow", "About", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("C5MainWindow", "Tab 1", nullptr));
        menuFile->setTitle(QApplication::translate("C5MainWindow", "File", nullptr));
        menuEdit->setTitle(QApplication::translate("C5MainWindow", "Edit", nullptr));
        menuAbout->setTitle(QApplication::translate("C5MainWindow", "Help", nullptr));
        menuDirectory_of_menu->setTitle(QApplication::translate("C5MainWindow", "Configuration of menu", nullptr));
        menuDirectory_of_storage->setTitle(QApplication::translate("C5MainWindow", "Configuration of storage", nullptr));
        menuConfiguration_of_hal->setTitle(QApplication::translate("C5MainWindow", "Configuration of hall", nullptr));
    } // retranslateUi

};

namespace Ui {
    class C5MainWindow: public Ui_C5MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C5MAINWINDOW_H
