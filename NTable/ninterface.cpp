#include "ninterface.h"
#include "nloadingdlg.h"
#include "ndataprovider.h"
#include "c5message.h"

NInterface::NInterface(QWidget *parent)
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
                                 const QVariant &marks, bool progress)
{
    fProgress = progress;
    auto *np = new NDataProvider();
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

void NInterface::httpQueryStarted()
{
    if (!fLoadingDlg) {
        fLoadingDlg = new NLoadingDlg(static_cast<QWidget *>(this->parent()));
    }
    fLoadingDlg->resetSeconds();
    if (fProgress) {
        fLoadingDlg->open();
    }
}

void NInterface::httpQueryFinished(QObject *sender)
{
    fLoadingDlg->hide();
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
