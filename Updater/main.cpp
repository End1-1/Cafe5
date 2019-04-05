#include "maindialog.h"
#include "install.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    /*
     * args
     * -lhttps://192.168.0.1            location local of update
     * -ihttps://www.hotelicca.com, -i  location of internet update, if -i, will use https://www.hotellica.com
     * -s0                              start update without prompt
     * -s1                              start update and show dialog
     */
    QApplication a(argc, argv);
    QStringList args;
    for (int i = 0; i < argc; i++) {
        args << argv[i];
    }

//    if (args.count() == 1) {
//        Install *i = new Install();
//        i->show();
//    }

    MainDialog m;
    m.show();

    return a.exec();
}
