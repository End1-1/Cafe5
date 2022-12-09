#ifndef WCUSTOMERDISPLAY_H
#define WCUSTOMERDISPLAY_H

#include <QWidget>

namespace Ui {
class WCustomerDisplay;
}

class WCustomerDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit WCustomerDisplay(QWidget *parent = nullptr);
    ~WCustomerDisplay();
    void clear();
    void addRow(const QString &name, const QString &qty, const QString &price, const QString &total);
    void setTotal(const QString &total);

private:
    Ui::WCustomerDisplay *ui;
};

#endif // WCUSTOMERDISPLAY_H
