#include "ninterface.h"
#include "nloadingdlg.h"
#include "ndataprovider.h"
#ifdef SELFBOARD
#include <QMessageBox>
#else
#include "c5message.h"
#endif
#include <QPointer>

namespace {

void ninterfaceShowError(const QString &msg)
{
#ifdef SELFBOARD
    QMessageBox::critical(nullptr, QString(), msg);
#else
    C5Message::error(msg);
#endif
}

} // namespace

int NInterface::sSuppressProgress = 0;
QPointer<NLoadingDlg> NInterface::sCurrentLoading;
QString NInterface::sPendingLoadingTitle;

void NInterface::pushSuppressProgress()
{
    ++sSuppressProgress;
}

void NInterface::popSuppressProgress()
{
    if (sSuppressProgress > 0) {
        --sSuppressProgress;
    }
}

bool NInterface::isProgressSuppressed()
{
    return sSuppressProgress > 0;
}

void NInterface::setLoadingTitle(const QString &title)
{
    fLoadingTitle = title;
    if (fLoadingDlg) {
        fLoadingDlg->setTitle(title);
    }
}

void NInterface::prepareLoadingTitle(const QString &title)
{
    sPendingLoadingTitle = title;
}

NLoadingDlg *NInterface::currentLoadingDialog()
{
    return sCurrentLoading.data();
}

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
    if (isProgressSuppressed()) {
        progress = false;
    }
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
    if (isProgressSuppressed()) {
        progress = false;
    }
    fProgress = progress;
    auto *np = new NDataProvider();
    np->changeTimeout(timeout);
    connect(np, &NDataProvider::started, this, &NInterface::httpQueryStarted);
    connect(np,
            SIGNAL(updateRequired(QString, QString, QString)),
            this->parent(),
            SLOT(updateRequired(QString, QString, QString)),
            Qt::QueuedConnection);
    auto finish = [this]() {
        if (fProgress || fLoadingDlg) {
            if (fLoadingDlg) {
                fLoadingDlg->hide();
            }
        }
    };
    connect(np, &NDataProvider::updateRequired, this, [this, np, finish](const QString &a, const QString &b, const QString &c) {
        finish();
        np->deleteLater();
        deleteLater();
    });
    connect(np, &NDataProvider::error, this->parent(), [this, np, errCallback, finish](const QString &msg) {
        finish();
        if(fProgress) {
            ninterfaceShowError(msg);
        }

        errCallback({{"status", 1}, {"errorMessage", msg}});
        np->deleteLater();
    });
    connect(np, &NDataProvider::done, this->parent(), [this, np, callback, finish](const QJsonObject &data) {
        finish();
        callback(data);
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
    if (isProgressSuppressed()) {
        progress = false;
    }
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
                ninterfaceShowError(msg);
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

    if (isProgressSuppressed()) {
        progress = false;
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
                if (NInterface::sCurrentLoading == iface->fLoadingDlg) {
                    NInterface::sCurrentLoading.clear();
                }
                iface->fLoadingDlg->hide();
            }
        }
    };
    connect(
        np,
        &NDataProvider::updateRequired,
        i,
        [iface, np, finish](const QString &a, const QString &b, const QString &c) {
            if (!iface) {
                return;
            }
            finish();
            np->deleteLater();
            iface->deleteLater();
        },
        Qt::QueuedConnection);
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
            ninterfaceShowError(msg);
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
    query(
        route,
        bearer,
        context,
        params,
        callback,
        [](const QJsonObject &jerr) {
            Q_UNUSED(jerr);
            return false;
        },
        true,
        60000,
        false);
}

void NInterface::httpQueryStarted()
{
    if (isProgressSuppressed()) {
        return;
    }
    if(!fLoadingDlg) {
        if(fProgress) {
            QWidget *parentWidget = qobject_cast<QWidget *>(parent());
            QString title = fLoadingTitle;
            if (title.isEmpty()) {
                title = sPendingLoadingTitle;
            }
            if (title.isEmpty()) {
                title = tr("Query");
            }
            sPendingLoadingTitle.clear();
            fLoadingDlg = new NLoadingDlg(title, parentWidget);
        }
    } else if (!fLoadingTitle.isEmpty()) {
        fLoadingDlg->setTitle(fLoadingTitle);
    } else if (!sPendingLoadingTitle.isEmpty()) {
        fLoadingDlg->setTitle(sPendingLoadingTitle);
        sPendingLoadingTitle.clear();
    }

    if(fProgress) {
        sCurrentLoading = fLoadingDlg;
        if (fLoadingDlg->parentWidget()) {
            fLoadingDlg->move(fLoadingDlg->parentWidget()->window()->geometry().center() - fLoadingDlg->rect().center());
        }

        fLoadingDlg->open();
        fLoadingDlg->raise();
        fLoadingDlg->activateWindow();
    }
}

void NInterface::httpQueryStartedWithShowDialog()
{
    if (isProgressSuppressed()) {
        return;
    }
    if(!fLoadingDlg) {
        if(fProgress) {
            QString title = fLoadingTitle;
            if (title.isEmpty()) {
                title = sPendingLoadingTitle;
            }
            if (title.isEmpty()) {
                title = tr("Query");
            }
            sPendingLoadingTitle.clear();
            fLoadingDlg = new NLoadingDlg(title, static_cast<QWidget*>(this->parent()));
        }
    } else if (!fLoadingTitle.isEmpty()) {
        fLoadingDlg->setTitle(fLoadingTitle);
    } else if (!sPendingLoadingTitle.isEmpty()) {
        fLoadingDlg->setTitle(sPendingLoadingTitle);
        sPendingLoadingTitle.clear();
    }

    if(fProgress) {
        sCurrentLoading = fLoadingDlg;
        fLoadingDlg->resetSeconds();
        fLoadingDlg->open();
        fLoadingDlg->raise();
    }
}

void NInterface::httpQueryFinished(QObject *sender)
{
    if(fLoadingDlg) {
        if (sCurrentLoading == fLoadingDlg) {
            sCurrentLoading.clear();
        }
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
        ninterfaceShowError(e);
    }
}
