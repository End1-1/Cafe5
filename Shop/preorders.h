#ifndef PREORDERS_H
#define PREORDERS_H

#include <QDialog>

namespace Ui {
class Preorders;
}

class Preorders : public QDialog
{
    Q_OBJECT

public:
    explicit Preorders(QWidget *parent = nullptr);

    ~Preorders();

    QByteArray fUUID;

private slots:
    void on_btnView_clicked();

private:
    Ui::Preorders *ui;
};

#endif // PREORDERS_H
