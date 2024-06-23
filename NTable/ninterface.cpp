#include "ninterface.h"
#include "nloadingdlg.h"
#include "ndataprovider.h"
#include "c5message.h"

NInterface::NInterface(QWidget *parent)
    : QObject{parent},
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
    auto *np = new NDataProvider(this->parent());
    connect(np, &NDataProvider::started, this, &NInterface::httpQueryStarted);
    connect(np, &NDataProvider::error, this, &NInterface::httpQueryError);
    connect(np, SIGNAL(done(QJsonObject)), this->parent(), slotResponse);
    np->setProperty("marks", marks);
    np->getData(route, params);
}

void NInterface::httpQueryStarted()
{
    if (!fLoadingDlg) {
        fLoadingDlg = new NLoadingDlg(static_cast<QWidget *>(this->parent()));
    }
    fLoadingDlg->mSecond = 0;
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
    C5Message::error(err);
}
