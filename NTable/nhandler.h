#ifndef NHANDLER_H
#define NHANDLER_H

#include <QObject>
#include <QJsonArray>

class NFilterDlg;
class QTableView;

class NHandler : public QObject
{
    Q_OBJECT
public:
    explicit NHandler(QObject *parent = nullptr);
    void configure(NFilterDlg *filter, const QVariantList &handlers, QTableView *tv);
    void handle(const QJsonArray &ja);
    void toolWidget(QWidget *w);

private:
    NFilterDlg *mFilterDlg;
    QVariantList mHandlers;
    QTableView *mTableView;

signals:

};

#endif // NHANDLER_H
