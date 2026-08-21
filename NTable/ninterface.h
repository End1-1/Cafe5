#ifndef NINTERFACE_H
#define NINTERFACE_H

#include <QObject>
#include <QJsonObject>
#include <QPointer>
#include <QString>

class NLoadingDlg;

class NInterface : public QObject
{
    Q_OBJECT
public:
    explicit NInterface(QObject *parent = nullptr);

    ~NInterface();

    char* fErrorSlot;

    QObject* fErrorObject;

    void createHttpQuery(const QString &route, const QJsonObject &params, const char* slotResponse,
                         const QVariant &marks = QVariant(), bool progress = true, int timeout = 60000);

    void createHttpQueryLambda(const QString &route, const QJsonObject &params,
                               std::function<void(const QJsonObject&)> callback,
                               std::function<void (const QJsonObject&)> errCallback,
                               const QVariant &marks = QVariant(), bool progress = true, int timeout = 60000);

    void createHttpQueryLambda2(const QString &route, const QJsonObject &params,
                                std::function<void(const QJsonObject&)> callback,
                                std::function<bool (const QJsonObject&)> errCallback,
                                const QVariant &marks = QVariant(), bool progress = true, int timeout = 60000);

    static void query(const QString &route, const QString &bearer, QObject *context, const QJsonObject &params,
                      std::function<void(const QJsonObject&)> callback,
                      std::function<bool (const QJsonObject&)> errCallback,
                      bool progress = true, int timeout = 60000, bool destroyLoadingAtEnd = true);

    static void query1(const QString &route, const QString &bearer, QObject *context, const QJsonObject &params,
                       std::function<void(const QJsonObject&)> callback);

    /** Nested: while active, loading dialogs from query()/createHttpQuery* are not shown. */
    static void pushSuppressProgress();
    static void popSuppressProgress();
    static bool isProgressSuppressed();

    /** Title applied to the next / current NLoadingDlg (web request progress). */
    void setLoadingTitle(const QString &title);
    /** Title for the next loading dialog created by query()/createHttpQuery* (any NInterface). */
    static void prepareLoadingTitle(const QString &title);
    static NLoadingDlg *currentLoadingDialog();

public slots:
    void httpQueryStarted();

    void httpQueryStartedWithShowDialog();

    void httpQueryFinished(QObject *sender);

    void httpQueryError(const QString &err);

private:
    NLoadingDlg* fLoadingDlg = nullptr;

    bool fProgress;

    QString fLoadingTitle;

    static int sSuppressProgress;

    static QPointer<NLoadingDlg> sCurrentLoading;

    static QString sPendingLoadingTitle;

signals:
};

/** RAII: suppress NInterface loading dialogs for the current scope. */
class NInterfaceProgressSuppressor
{
public:
    NInterfaceProgressSuppressor() { NInterface::pushSuppressProgress(); }
    ~NInterfaceProgressSuppressor() { NInterface::popSuppressProgress(); }
    NInterfaceProgressSuppressor(const NInterfaceProgressSuppressor &) = delete;
    NInterfaceProgressSuppressor &operator=(const NInterfaceProgressSuppressor &) = delete;
};

#endif // NINTERFACE_H
