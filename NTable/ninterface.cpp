#include "ninterface.h"
#include "nloadingdlg.h"
#include "ndataprovider.h"
#ifdef SELFBOARD
#include <QMessageBox>
#else
#include "c5message.h"
#endif
#include <QPointer>
#include <QTimer>
#include <QWidget>

namespace {

void ninterfaceShowError(const QString &msg)
{
#ifdef SELFBOARD
    QMessageBox::critical(nullptr, QString(), msg);
#else
    C5Message::error(msg);
#endif
}

/** Owned by the request context, so it disappears together with the window that asked. */
class NJobPoller : public QObject
{
public:
    NJobPoller(QObject *context, const QString &bearer, const QString &title, int intervalMs,
               std::function<void(const QJsonObject&)> callback,
               std::function<bool (const QJsonObject&)> errCallback)
        : QObject(context),
          mContext(context),
          mBearer(bearer),
          mCallback(std::move(callback)),
          mErrCallback(std::move(errCallback))
    {
        mTimer.setSingleShot(true);
        mTimer.setInterval(intervalMs);
        connect(&mTimer, &QTimer::timeout, this, &NJobPoller::poll);
        if (!NInterface::isProgressSuppressed()) {
            QWidget *parentWidget = qobject_cast<QWidget *>(context);
            mDlg = new NLoadingDlg(title, parentWidget);
            if (parentWidget) {
                mDlg->move(parentWidget->window()->geometry().center() - mDlg->rect().center());
            }
            mDlg->open();
        }
    }

    void start(const QString &route, const QJsonObject &params)
    {
        NInterface::query(route, mBearer, mContext, params,
                          [this](const QJsonObject &jo) {
                              mJobId = jo.value(QStringLiteral("job_id")).toString();
                              if (mJobId.isEmpty()) {
                                  fail(NInterface::tr("Server did not start the job"));
                                  return;
                              }
                              mTimer.start();
                          },
                          [this](const QJsonObject &jerr) {
                              fail(jerr.value(QStringLiteral("errorMessage")).toString());
                              return true;
                          },
                          false);
    }

private:
    static constexpr int kMaxPollFailures = 5;

    void poll()
    {
        NInterface::query(QStringLiteral("/engine/v2/common/jobs/status"), mBearer, mContext,
                          {{"id", mJobId}},
                          [this](const QJsonObject &jo) {
                              mPollFailures = 0;
                              if (!jo.value(QStringLiteral("done")).toBool()) {
                                  mTimer.start();
                                  return;
                              }
                              const QString err = jo.value(QStringLiteral("error")).toString();
                              if (jo.contains(QStringLiteral("error"))) {
                                  fail(err.isEmpty() ? NInterface::tr("Job failed") : err);
                                  return;
                              }
                              closeDialog();
                              mCallback(jo.value(QStringLiteral("result")).toObject());
                              deleteLater();
                          },
                          [this](const QJsonObject &jerr) {
                              // The job keeps running on the server; a dropped poll is not a failure yet.
                              if (++mPollFailures < kMaxPollFailures) {
                                  mTimer.start();
                              } else {
                                  fail(jerr.value(QStringLiteral("errorMessage")).toString());
                              }
                              return true;
                          },
                          false);
    }

    void fail(const QString &msg)
    {
        closeDialog();
        const bool handled = mErrCallback && mErrCallback({{"status", 1}, {"errorMessage", msg}});
        if (!handled && !msg.isEmpty()) {
            ninterfaceShowError(msg);
        }
        deleteLater();
    }

    void closeDialog()
    {
        if (mDlg) {
            mDlg->hide();
            mDlg->deleteLater();
        }
    }

    QObject *mContext;
    QString mBearer;
    QString mJobId;
    QTimer mTimer;
    int mPollFailures = 0;
    QPointer<NLoadingDlg> mDlg;
    std::function<void(const QJsonObject&)> mCallback;
    std::function<bool (const QJsonObject&)> mErrCallback;
};

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

void NInterface::forceCloseCurrentLoading()
{
    // sCurrentLoading is already a QPointer — only hide, do not delete or wrap again.
    if (sCurrentLoading) {
        sCurrentLoading->hide();
    }
    sCurrentLoading.clear();
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
        if (sCurrentLoading == fLoadingDlg) {
            sCurrentLoading.clear();
        }
        fLoadingDlg->deleteLater();
        fLoadingDlg = nullptr;
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
    // Context must be `this`, not parent(): authorize often uses NInterface() without parent,
    // and connecting done/error to nullptr leaves the loading dialog spinning forever.
    if (parent() && parent()->metaObject()->indexOfSlot("updateRequired(QString,QString,QString)") >= 0) {
        connect(np,
                SIGNAL(updateRequired(QString, QString, QString)),
                parent(),
                SLOT(updateRequired(QString, QString, QString)),
                Qt::QueuedConnection);
    }
    auto finish = [this]() {
        if (fProgress || fLoadingDlg) {
            if (fLoadingDlg) {
                if (sCurrentLoading == fLoadingDlg) {
                    sCurrentLoading.clear();
                }
                fLoadingDlg->hide();
            }
        }
    };
    connect(np, &NDataProvider::updateRequired, this, [this, np, errCallback, finish](const QString &a, const QString &b, const QString &c) {
        finish();
        const bool hasUpdateSlot =
            parent() && parent()->metaObject()->indexOfSlot("updateRequired(QString,QString,QString)") >= 0;
        if (!hasUpdateSlot) {
            const QString msg = a.isEmpty()
                ? tr("Application update required")
                : a;
            if (errCallback) {
                errCallback({{"status", 1}, {"errorMessage", msg}, {"appName", b}, {"newVersion", c}});
            } else if (fProgress) {
                ninterfaceShowError(msg);
            }
        }
        np->deleteLater();
        deleteLater();
    });
    connect(np, &NDataProvider::error, this, [this, np, errCallback, finish](const QString &msg) {
        finish();
        if(fProgress) {
            ninterfaceShowError(msg);
        }

        errCallback({{"status", 1}, {"errorMessage", msg}});
        np->deleteLater();
    });
    connect(np, &NDataProvider::done, this, [this, np, callback, finish](const QJsonObject &data) {
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
    if (parent() && parent()->metaObject()->indexOfSlot("updateRequired(QString,QString,QString)") >= 0) {
        connect(np, SIGNAL(updateRequired(QString, QString, QString)), parent(), SLOT(updateRequired(QString, QString, QString)));
    }
    connect(np, &NDataProvider::updateRequired, this, [this, np, errCallback](const QString &a, const QString &b, const QString &c) {
        if (fProgress || fLoadingDlg) {
            if (fLoadingDlg) {
                fLoadingDlg->hide();
            }
        }
        const bool hasUpdateSlot =
            parent() && parent()->metaObject()->indexOfSlot("updateRequired(QString,QString,QString)") >= 0;
        if (!hasUpdateSlot) {
            const QString msg = a.isEmpty()
                ? tr("Application update required")
                : a;
            if (!errCallback || !errCallback({{"status", 1}, {"errorMessage", msg}, {"appName", b}, {"newVersion", c}})) {
                if (fProgress) {
                    ninterfaceShowError(msg);
                }
            }
        }
        np->deleteLater();
    });
    connect(np, &NDataProvider::error, this, [this, np, errCallback](const QString & msg) {
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
    connect(np, &NDataProvider::done, this, [this, np, callback](const QJsonObject & data) {
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
        [iface, np, finish, errCallback, context](const QString &a, const QString &b, const QString &c) {
            if (!iface) {
                return;
            }
            finish();
            // C5Widget/C5Dialog already show the updater UI via their slot.
            // SelfBoard and other hosts without that slot must not hang forever.
            const bool hasUpdateSlot =
                context && context->metaObject()->indexOfSlot("updateRequired(QString,QString,QString)") >= 0;
            if (!hasUpdateSlot) {
                const QString msg = a.isEmpty()
                    ? QObject::tr("Application update required")
                    : a;
                if (errCallback) {
                    errCallback({{"status", 1}, {"errorMessage", msg}, {"appName", b}, {"newVersion", c}});
                } else if (iface->fProgress) {
                    ninterfaceShowError(msg);
                }
            }
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
    // Only wire updateRequired UI slot when the context actually has it (C5Widget etc.).
    if (context->metaObject()->indexOfSlot("updateRequired(QString,QString,QString)") >= 0) {
        connect(np, SIGNAL(updateRequired(QString, QString, QString)), context, SLOT(updateRequired(QString, QString, QString)));
    }
    np->getData(route, params);
}

void NInterface::query1(const QString &route, const QString &bearer, QObject *context, const QJsonObject &params,
                        std::function<void (const QJsonObject&)> callback, int timeout)
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
        timeout,
        false);
}

void NInterface::queryJob(const QString &route, const QString &bearer, QObject *context, const QJsonObject &params,
                          std::function<void (const QJsonObject&)> callback,
                          std::function<bool (const QJsonObject&)> errCallback,
                          int pollIntervalMs)
{
    Q_ASSERT(context);
    if (!context) {
        qWarning() << "NInterface::queryJob called with null context";
        return;
    }
    QString title = sPendingLoadingTitle;
    sPendingLoadingTitle.clear();
    if (title.isEmpty()) {
        title = tr("Query");
    }
    auto *poller = new NJobPoller(context, bearer, title, pollIntervalMs, std::move(callback), std::move(errCallback));
    poller->start(route, params);
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

    if (sender) {
        sender->deleteLater();
    }
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
