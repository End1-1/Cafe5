#ifndef NINTERFACE_H
#define NINTERFACE_H

#include <QObject>
#include <QJsonObject>

class NLoadingDlg;

class NInterface : public QObject
{
    Q_OBJECT
public:
    explicit NInterface(QObject *parent = nullptr);

    ~NInterface();

    char *fErrorSlot;

    QObject *fErrorObject;

    void createHttpQuery(const QString &route, const QJsonObject &params, const char *slotResponse,
                         const QVariant &marks = QVariant(), bool progress = true, int timeout = 60000);

    void createHttpQueryLambda(const QString &route, const QJsonObject &params,
                               std::function<void(const QJsonObject &)> callback, std::function<void (const QJsonObject &)> errCallback,
                               const QVariant &marks = QVariant(), bool progress = true, int timeout = 60000);

public slots:
    void httpQueryStarted();

    void httpQueryFinished(QObject *sender);

    void httpQueryError(const QString &err);

private:
    NLoadingDlg *fLoadingDlg;

    bool fProgress;

signals:
};

#endif // NINTERFACE_H
