#ifndef NHANDLER_H
#define NHANDLER_H

#include <QObject>
#include <QJsonArray>

class NFilterDlg;

class NHandler : public QObject
{
    Q_OBJECT
public:
    explicit NHandler(QObject *parent = nullptr);
    void configure(NFilterDlg *filter, const QVariantList &handlers);
    void handle(const QJsonArray &ja);
    void toolWidget(QWidget *w);

private:
    NFilterDlg *mFilterDlg;
    QVariantList mHandlers;

signals:

};

#endif // NHANDLER_H
