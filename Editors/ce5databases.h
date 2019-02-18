#ifndef CE5DATABASES_H
#define CE5DATABASES_H

#include "ce5editor.h"

namespace Ui {
class CE5Databases;
}

class CE5Databases : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Databases(const QStringList &dbParams, QWidget *parent = 0);

    ~CE5Databases();

    virtual QString title() {return tr("Database");}

    virtual QString table() {return "s_db";}

private slots:
    void on_btnCheckConnection_clicked();

private:
    Ui::CE5Databases *ui;
};

#endif // CE5DATABASES_H
