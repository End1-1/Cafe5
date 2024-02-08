#ifndef C5DLGCONNECTIONS_H
#define C5DLGCONNECTIONS_H

#include <QDialog>
#include <QJsonArray>

namespace Ui {
class C5DlgConnections;
}

class C5DlgConnections : public QDialog
{
    Q_OBJECT

public:
    explicit C5DlgConnections(QWidget *parent = nullptr);
    ~C5DlgConnections();

private slots:
    void on_btnPlus_clicked();

    void on_btnEdit_clicked();

    void on_btnDelete_clicked();

private:
    Ui::C5DlgConnections *ui;
    QJsonArray fServers;
    void readServers();
};

#endif // C5DLGCONNECTIONS_H
