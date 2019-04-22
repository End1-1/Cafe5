#ifndef CE5CLIENT_H
#define CE5CLIENT_H

#include "ce5editor.h"

namespace Ui {
class CE5Client;
}

class CE5Client : public CE5Editor
{
    Q_OBJECT

public:
    explicit CE5Client(const QStringList &dbParams, QWidget *parent = nullptr);

    ~CE5Client();

    virtual QString title() {return tr("Client");}

    virtual QString table() {return "b_clients";}

private:
    Ui::CE5Client *ui;
};

#endif // CE5CLIENT_H
