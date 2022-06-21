#ifndef JZSYNCMANAGER_H
#define JZSYNCMANAGER_H

#include <QObject>
#include <QHash>

class JzSyncManager : public QObject
{
    Q_OBJECT
public:
    explicit JzSyncManager(const QHash<QString, QString> &opt);

    void start();

private:
    QHash<QString, QString> fOptions;

private slots:
    void timeout();

signals:

};

#endif // JZSYNCMANAGER_H
