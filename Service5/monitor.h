#ifndef MONITOR_H
#define MONITOR_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Monitor; }
QT_END_NAMESPACE

class Monitor : public QDialog
{
    Q_OBJECT

public:
    Monitor(QWidget *parent = nullptr);
    ~Monitor();

private:
    Ui::Monitor *ui;

private slots:
    void receiveData(int code, const QString &session, const QString &data1, const QVariant &data2);
};
#endif // MONITOR_H
