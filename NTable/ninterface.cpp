#include "ninterface.h"
#include "nloadingdlg.h"
#include "ndataprovider.h"
#include "c5message.h"

NInterface::NInterface(QObject *parent)
    : QObject{parent},
      fErrorSlot(nullptr),
      fErrorObject(nullptr),
      fLoadingDlg(nullptr)
{
}

NInterface::~NInterface()
{
    if (fLoadingDlg) {
        fLoadingDlg->deleteLater();
    }
}

void NInterface::createHttpQuery(const QString &route, const QJsonObject &params, const char *slotResponse,
                                 const QVariant &marks, bool progress, int timeout)
{
    fProgress = progress;
    auto *np = new NDataProvider();
    np->changeTimeout(timeout);
    connect(np, &NDataProvider::started, this, &NInterface::httpQueryStarted);
    if (fErrorSlot == nullptr) {
        connect(np, &NDataProvider::error, this, &NInterface::httpQueryError);
    } else {
        connect(np, SIGNAL(error(QString)), fErrorObject, fErrorSlot);
    }
    connect(np, SIGNAL(done(QJsonObject)), this->parent(), slotResponse);
    qDebug() << marks;
    np->setProperty("marks", marks);
    np->getData(route, params);
}

void NInterface::createHttpQueryLambda(const QString &route, const QJsonObject &params,
                                       std::function<void (const QJsonObject &)> callback,
                                       std::function<void (const QJsonObject &)> errCallback,
                                       const QVariant &marks, bool progress, int timeout)
{
    fProgress = progress;
    auto *np = new NDataProvider();
    np->changeTimeout(timeout);
    connect(np, &NDataProvider::started, this, &NInterface::httpQueryStarted);
    connect(np, &NDataProvider::error, this->parent(), [this, np, errCallback](const QString &msg) {
        if (fProgress && fLoadingDlg) {
            fLoadingDlg->hide();
        }
        if (fProgress) {
            C5Message::error(msg);
        }
        errCallback({{"status", 1}, {"errorMessage", msg}});
        np->deleteLater();
    });
    connect(np, &NDataProvider::done, this->parent(), [this, np, callback](const QJsonObject &data) {
        callback(data);
        if (fProgress && fLoadingDlg) {
            fLoadingDlg->hide();
        }
        np->deleteLater();
    });
    qDebug() << marks;
    np->setProperty("marks", marks);
    np->getData(route, params);
}

void NInterface::httpQueryStarted()
{
    if (!fLoadingDlg) {
        if (fProgress) {
            fLoadingDlg = new NLoadingDlg(static_cast<QWidget *>(this->parent()));
        }
    }
    if (fProgress) {
        fLoadingDlg->resetSeconds();
        fLoadingDlg->open();
    }
}

void NInterface::httpQueryFinished(QObject *sender)
{
    if (fLoadingDlg) {
        fLoadingDlg->hide();
    }
    sender->deleteLater();
}

void NInterface::httpQueryError(const QString &err)
{
    httpQueryFinished(sender());
    QString e = err;
    if (err.contains("Unauthorized")) {
        e = tr("Unauthorized");
    }
    if (fProgress) {
        C5Message::error(e);
    }
}
