#include "dlgloader.h"
#include "dlgconfig.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString app;
    for (int i = 0; i < argc; i++) {
        QString args = argv[i];
        if (args.contains("--app")) {
            QStringList l = args.split("=");
            if (l.size() == 2) {
                app = l.at(1);
            }
        }
    }

    if (app.isEmpty()) {
        QMessageBox::critical(0, "Error", "No application configured");
        return 0;
    }

    if (app == "config") {
        DlgConfig().exec();
        return 0;
    }

    DlgLoader w;
    w.show();
    w.checkForUpdate();
    return a.exec();
}
