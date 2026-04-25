#include "mainwindow.h"

#ifdef FRONTDESK
#include "c5mainwindow.h"
C5MainWindow* __mainWindow;
#else
#include <QDialog>
QDialog *__mainWindow;
#endif
