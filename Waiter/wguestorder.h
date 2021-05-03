#ifndef WGUESTORDER_H
#define WGUESTORDER_H

#include <QWidget>

namespace Ui {
class WGuestOrder;
}

class WOrder;

class WGuestOrder : public QWidget
{
    Q_OBJECT

public:
    explicit WGuestOrder(QWidget *parent = nullptr);

    ~WGuestOrder();

    WOrder *worder();

private:
    Ui::WGuestOrder *ui;
};

#endif // WGUESTORDER_H
