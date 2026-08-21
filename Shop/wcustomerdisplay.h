#ifndef WCUSTOMERDISPLAY_H
#define WCUSTOMERDISPLAY_H

#include <QWidget>

class QCloseEvent;

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
    void addRow(const QString &name, const QString &qty, const QString &price, const QString &total, const QString &discount);
    void setTotal(const QString &total);
    void placeOnSecondaryScreen();

signals:
    void displayClosed();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::WCustomerDisplay *ui;
};

#endif // WCUSTOMERDISPLAY_H
