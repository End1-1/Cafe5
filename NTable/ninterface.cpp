#include "ninterface.h"
#include "nloadingdlg.h"
#include "ndataprovider.h"
#include "c5message.h"
#include <QPointer>

NInterface::NInterface(QObject *parent)
    : QObject{parent},
      fErrorSlot(nullptr),
      fErrorObject(nullptr)
{
}

NInterface::~NInterface()
{
    if(fLoadingDlg) {
        fLoadingDlg->deleteLater();
    }
}

void NInterface::createHttpQuery(const QString &route, const QJsonObject &params, const char* slotResponse,
                                 const QVariant &marks, bool progress, int timeout)
{
    fProgress = progress;
    auto *np = new NDataProvider();
    np->changeTimeout(timeout);
    connect(np, &NDataProvider::started, this, &NInterface::httpQueryStarted);

    if(fErrorSlot == nullptr) {
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
                                       std::function<void (const QJsonObject&)> callback,
                                       std::function<void (const QJsonObject&)> errCallback,
                                       const QVariant &marks, bool progress, int timeout)
{
    fProgress = progress;
    auto *np = new NDataProvider();
    np->changeTimeout(timeout);
    connect(np, &NDataProvider::started, this, &NInterface::httpQueryStarted);
    connect(np, SIGNAL(updateRequired(QString, QString, QString)), this->parent(), SLOT(updateRequired(QString, QString, QString)));
    connect(np, &NDataProvider::error, this->parent(), [this, np, errCallback](const QString & msg) {
        if(fProgress || fLoadingDlg) {
            if(fLoadingDlg) {
                fLoadingDlg->hide();
            }
        }

        if(fProgress) {
            C5Message::error(msg);
        }

        errCallback({{"status", 1}, {"errorMessage", msg}});
        np->deleteLater();
    });
    connect(np, &NDataProvider::done, this->parent(), [this, np, callback](const QJsonObject & data) {
        callback(data);

        if(fProgress || fLoadingDlg) {
            if(fLoadingDlg) {
                fLoadingDlg->hide();
            }
        }

        np->deleteLater();
    });
    np->setProperty("marks", marks);
    np->getData(route, params);
}

void NInterface::createHttpQueryLambda2(const QString &route, const QJsonObject &params,
                                        std::function<void (const QJsonObject&)> callback,
                                        std::function<bool (const QJsonObject&)> errCallback,
                                        const QVariant &marks, bool progress, int timeout)
{
    fProgress = progress;
    auto *np = new NDataProvider();
    np->changeTimeout(timeout);
    connect(np, &NDataProvider::started, this, &NInterface::httpQueryStarted);
    connect(np, SIGNAL(updateRequired(QString, QString, QString)), this->parent(), SLOT(updateRequired(QString, QString, QString)));
    connect(np, &NDataProvider::error, this->parent(), [this, np, errCallback](const QString & msg) {
        if(fProgress || fLoadingDlg) {
            if(fLoadingDlg) {
                fLoadingDlg->hide();
            }
        }

        if(!errCallback({{"status", 1}, {"errorMessage", msg}})) {
            if(fProgress) {
                C5Message::error(msg);
            }
        }
        np->deleteLater();
    });
    connect(np, &NDataProvider::done, this->parent(), [this, np, callback](const QJsonObject & data) {
        callback(data);

        if(fProgress || fLoadingDlg) {
            if(fLoadingDlg) {
                fLoadingDlg->hide();
            }
        }

        np->deleteLater();
    });
    np->setProperty("marks", marks);
    np->getData(route, params);
}

void NInterface::query(const QString &route, const QString &bearer, QObject *context,
                       const QJsonObject &params,
                       std::function<void (const QJsonObject&)> callback,
                       std::function<bool (const QJsonObject&)> errCallback,
                       bool progress, int timeout, bool destroyLoadingAtEnd)
{
    Q_ASSERT(context);

    if(!context) {
        qWarning() << "NInterface::query called with null context";
        return;
    }

    auto *i = new NInterface();
    QPointer<NInterface> iface(i);
    i->fProgress = progress;
    auto *np = new NDataProvider(i);
    np->changeTimeout(timeout);
    np->changeBearer(bearer);
    auto finish = [iface]() {
        if(!iface) {
            return;
        }

        if(iface->fProgress || iface->fLoadingDlg) {
            if(iface->fLoadingDlg) {
                iface->fLoadingDlg->hide();
            }
        }
    };
    connect(np, &NDataProvider::started, i, &NInterface::httpQueryStarted);
    connect(np, &NDataProvider::error, context, [np, errCallback, finish, iface](const QString & msg) {
        if(!iface) {
            return;
        }

        finish();
        bool handled = false;

        if(errCallback) {
            handled = errCallback({{"status", 1}, {"errorMessage", msg}});
        }

        if(!handled && iface->fProgress) {
            C5Message::error(msg);
        }

        np->deleteLater();
        iface->deleteLater();
    });
    connect(np, &NDataProvider::done, context, [ = ](const QJsonObject & data) {
        if(!iface) {
            return;
        }

        if(!destroyLoadingAtEnd) {
            finish();
        }

        callback(data);

        if(destroyLoadingAtEnd) {
            finish();
        }

        np->deleteLater();
        iface->deleteLater();
    });
    connect(np, SIGNAL(updateRequired(QString, QString, QString)), context, SLOT(updateRequired(QString, QString, QString)));
    np->getData(route, params);
}

void NInterface::query1(const QString &route, const QString &bearer, QObject *context, const QJsonObject &params, std::function<void (const QJsonObject&)> callback)
{
    query(route, bearer, context, params, callback,
    [](const QJsonObject & jerr) {
        Q_UNUSED(jerr);
        return false;
    }, true, 5000, false);
}

void NInterface::httpQueryStarted()
{
    if(!fLoadingDlg) {
        if(fProgress) {
            fLoadingDlg = new NLoadingDlg(tr("Query"), static_cast<QWidget*>(this->parent()));
        }
    }

    if(fProgress) {
        fLoadingDlg->resetSeconds();
        // fLoadingDlg->open();
    }
}

void NInterface::httpQueryStartedWithShowDialog()
{
    if(!fLoadingDlg) {
        if(fProgress) {
            fLoadingDlg = new NLoadingDlg(tr("Query"), static_cast<QWidget*>(this->parent()));
        }
    }

    if(fProgress) {
        fLoadingDlg->resetSeconds();
        fLoadingDlg->open();
        fLoadingDlg->raise();
    }
}

void NInterface::httpQueryFinished(QObject *sender)
{
    if(fLoadingDlg) {
        fLoadingDlg->hide();
    }

    sender->deleteLater();
}

void NInterface::httpQueryError(const QString &err)
{
    httpQueryFinished(sender());
    QString e = err;

    if(err.contains("Unauthorized")) {
        e = tr("Unauthorized");
    }

    if(fProgress) {
        C5Message::error(e);
    }
}
