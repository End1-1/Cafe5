#ifndef TEST_H
#define TEST_H

#include "threadworker.h"
#include <QSslSocket>
#include <QFile>
#include <QApplication>

template<typename T>
class Test : public ThreadWorker
{
public:
    Test(const QString &serverIP, int port, int count, QObject *parent, const char *data_slot, const char *finished_slot, const char *error_slot) :
        fServerIP(serverIP),
        fPort(port),
        fCount(count),
        fParent(parent),
        fSlotData(data_slot),
        fSlotFinished(finished_slot),
        fSlotError(error_slot) {
        if (fSslCertificate.isNull()) {
            QString certFileName = qApp->applicationDirPath() + "/" + "cert.pem";
            QFile file(certFileName);
            if (!file.open(QIODevice::ReadOnly)) {
                //ui->peLog->appendPlainText("Could not load certificate: " + qApp->applicationDirPath() + "/" + "cert.pem");
            }
            fSslCertificate = QSslCertificate(file.readAll());
        }
    }

protected slots:
    virtual void run() {
        for (int i = 0; i < fCount; i++) {
            T *t  = new T(fServerIP, fPort, fSslCertificate, i);
            t->connect(t, SIGNAL(finished()), fParent, fSlotFinished);
            t->connect(t, SIGNAL(threadError(int,QString)), fParent, fSlotError);
            t->connect(t, SIGNAL(data(int,QVariant)), fParent, fSlotData);
            t->start();
        }
    }

private:
    QString fServerIP;
    int fPort;
    int fCount;
    QObject *fParent;
    const char *fSlotFinished;
    const char *fSlotError;
    const char *fSlotData;
};

extern QSslCertificate fSslCertificate;

#endif // TEST_H
