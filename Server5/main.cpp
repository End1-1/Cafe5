#include "c5server5.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTranslator t;
    t.load(":/Server5.qm");
    a.installTranslator( &t);
    C5Server5 w;
    w.show();
    w.hide();
    return a.exec();
}
