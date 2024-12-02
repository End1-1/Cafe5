#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QObject>

class QWebSocketServer;

class ServerThread : public QObject
{
    Q_OBJECT
public:
    ServerThread(const QString &configPath);
    ~ServerThread();

public slots:
    void run();

private:
    QWebSocketServer *fServer;
    const QString fConfigPath;

private slots:
    void onNewConnection();
    void onTextMessage(const QString &msg);

};

#endif // SERVERTHREAD_H
