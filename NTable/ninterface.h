#ifndef NINTERFACE_H
#define NINTERFACE_H

#include <QObject>
#include <QJsonObject>

class NLoadingDlg;

class NInterface : public QObject
{
    Q_OBJECT
public:
    explicit NInterface(QWidget *parent = nullptr);

    void createHttpQuery(const QString &route, const QJsonObject &params, const char *slotResponse,
                         const QVariant &marks = QVariant());

public slots:
    void httpQueryStarted();

    void httpQueryFinished(QObject *sender);

    void httpQueryError(const QString &err);

private:
    NLoadingDlg *fLoadingDlg;

signals:
};

#endif // NINTERFACE_H
