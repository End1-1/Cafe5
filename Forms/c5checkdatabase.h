#ifndef C5CHECKDATABASE_H
#define C5CHECKDATABASE_H

#include "c5dialog.h"

namespace Ui {
class C5CheckDatabase;
}

class C5CheckDatabase : public C5Dialog
{
    Q_OBJECT

public:
    explicit C5CheckDatabase(C5User *user);

    ~C5CheckDatabase();

private slots:
    void on_btnStart_clicked();

    void on_btnStoreUtlis_clicked();

private:
    Ui::C5CheckDatabase *ui;
};

#endif // C5CHECKDATABASE_H
