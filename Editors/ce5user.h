#ifndef CE5USER_H
#define CE5USER_H

#include "ce5editor.h"

namespace Ui {
class CE5User;
}

class CE5User : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5User(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5User();

    virtual void setId(int id) override;

    virtual QString title() {return tr("User");}

    virtual QString table() {return "s_user";}

    virtual bool checkData(QString &err);

private slots:
    void on_btnNewGroup_clicked();

    void on_btnLoadImage_clicked();

private:
    Ui::CE5User *ui;

};

#endif // CE5USER_H
