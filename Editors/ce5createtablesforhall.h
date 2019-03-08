#ifndef CE5CREATETABLESFORHALL_H
#define CE5CREATETABLESFORHALL_H

#include "c5dialog.h"

namespace Ui {
class CE5CreateTablesForHall;
}

class CE5CreateTablesForHall : public C5Dialog
{
    Q_OBJECT

public:
    explicit CE5CreateTablesForHall(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5CreateTablesForHall();

    static void createTableForHall(const QStringList &dbParams);

private slots:
    void on_btnCreate_clicked();

    void on_btnNewHall_clicked();

private:
    Ui::CE5CreateTablesForHall *ui;
};

#endif // CE5CREATETABLESFORHALL_H
