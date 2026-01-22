#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QCoreApplication>
#include "updatemanger.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QCommandLineParser parser;
    parser.setApplicationDescription("Picasso Updater");
    parser.addHelpOption();
    QCommandLineOption appOpt("app", "Application module name", "module");
    QCommandLineOption verOpt("version", "Version to update to", "version");
    parser.addOption(appOpt);
    parser.addOption(verOpt);
    parser.process(a);
    QString module  = parser.value(appOpt);
    QString version = parser.value(verOpt);
    QString appExe  = QString("%1.exe").arg(parser.value(appOpt));
    qDebug() << "Updater args:" << module << version << appExe ;

    if(module.isEmpty() || version.isEmpty() || appExe.isEmpty()) {
        QMessageBox::critical(nullptr, "Updater", "Missing required arguments.");
        return 1;
    }

    QWidget w;
    QVBoxLayout l(&w);
    QProgressBar pb;
    pb.setRange(0, 100);
    l.addWidget(&pb);
    w.resize(420, 90);
    w.show();
    const QString appDir = QCoreApplication::applicationDirPath();
    QString archiveUrl = QString("https://update.picasso.am/updates/%1/%2/%1.7z")
                         .arg(module, version);
    UpdateManager um(archiveUrl, appDir, appExe);
    um.setProgressBar(&pb);
    QObject::connect(&um, &UpdateManager::error, [&](const QString & msg) {
        QMessageBox::critical(&w, "Update error", msg);
        a.quit();
    });
    QObject::connect(&um, &UpdateManager::finished, [&]() {
        a.quit();
    });
    um.start();
    return a.exec();
}
