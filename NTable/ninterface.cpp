#include "ninterface.h"
#include "nloadingdlg.h"
#include "ndataprovider.h"
#include "c5message.h"

NInterface::NInterface(QWidget *parent)
    : QObject{parent}
{
}

void NInterface::createHttpQuery(const QString &route, const QJsonObject &params, const char *slotResponse,
                                 const QVariant &marks)
{
    auto *np = new NDataProvider(this->parent());
    connect(np, &NDataProvider::started, this, &NInterface::httpQueryStarted);
    connect(np, &NDataProvider::error, this, &NInterface::httpQueryError);
    connect(np, SIGNAL(done(QJsonObject)), this->parent(), slotResponse);
    np->setProperty("marks", marks);
    np->getData(route, params);
}

void NInterface::httpQueryStarted()
{
    fLoadingDlg = new NLoadingDlg(static_cast<QWidget *>(this->parent()));
    fLoadingDlg->open();
}

void NInterface::httpQueryFinished(QObject *sender)
{
    fLoadingDlg->hide();
    fLoadingDlg->deleteLater();
    sender->deleteLater();
}

void NInterface::httpQueryError(const QString &err)
{
    httpQueryFinished(this);
    C5Message::error(err);
}
