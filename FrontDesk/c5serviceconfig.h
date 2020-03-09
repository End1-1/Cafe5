#ifndef C5SERVICECONFIG_H
#define C5SERVICECONFIG_H

#include <QDialog>

class QSslSocket;

namespace Ui {
class C5ServiceConfig;
}

class C5ServiceConfig : public QDialog
{
    Q_OBJECT

public:
    explicit C5ServiceConfig(const QString &ip, QWidget *parent = nullptr);
    ~C5ServiceConfig();

private slots:
    void on_btnConnect_clicked();
    void on_btnSave_clicked();

private:
    Ui::C5ServiceConfig *ui;
    QSslSocket *fSslSocket;
    void connectToService(QByteArray &data, qint32 &datatype);

};

#endif // C5SERVICECONFIG_H
