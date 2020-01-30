#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QDialog>

namespace Ui {
class DbConnection;
}

class DbConnection : public QDialog
{
    Q_OBJECT

public:
    explicit DbConnection(QWidget *parent = nullptr);
    ~DbConnection();

private:
    Ui::DbConnection *ui;
};

#endif // DBCONNECTION_H
