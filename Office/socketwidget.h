#ifndef SOCKETWIDGET_H
#define SOCKETWIDGET_H

#include <QWidget>

class SocketWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SocketWidget(QWidget *parent = nullptr);


protected slots:
    virtual void externalDataReceived(quint16 cmd, quint32 messageId, const QByteArray &data) = 0;
    virtual void connectionLost();

signals:
    void dataReady(const QByteArray &);
};

#endif // SOCKETWIDGET_H
