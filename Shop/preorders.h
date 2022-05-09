#ifndef PREORDERS_H
#define PREORDERS_H

#include "c5dialog.h"

namespace Ui {
class Preorders;
}

class Preorders : public C5Dialog
{
    Q_OBJECT

public:
    explicit Preorders();

    ~Preorders();

    QByteArray fUUID;

private slots:
    void on_btnView_clicked();

    void on_btnExit_clicked();

private:
    Ui::Preorders *ui;
};

#endif // PREORDERS_H
