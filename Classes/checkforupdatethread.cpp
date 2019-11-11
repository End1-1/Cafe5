#include "checkforupdatethread.h"
#include "c5database.h"
#include "fileversion.h"
#include <QApplication>

CheckForUpdateThread::CheckForUpdateThread(QObject *parent) :
    QThread(parent)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(go()), this, SLOT(startCheck()));
}

CheckForUpdateThread::~CheckForUpdateThread()
{

}

void CheckForUpdateThread::run()
{
    emit go();
    exec();
}

void CheckForUpdateThread::startCheck()
{
//    QString fileVersion = FileVersion::getVersionString(qApp->applicationFilePath());
//    if (fDbParams.count() > 0) {
//        C5Database db(fDbParams);
//        db[":f_app"] = fApplication.toUpper();
//        db.exec("select f_version, f_updatelocalpath from s_app where f_app=:f_app");
//        if (db.nextRow()) {
//            QString dbVersion = db.getString(0);
//            if (dbVersion != fileVersion) {
//                emit checked(true, usLocalnet, db.getString(1));
//            }
//        }
//    } else {

//    }

    emit checked(false, usNone, "");
    quit();
}
