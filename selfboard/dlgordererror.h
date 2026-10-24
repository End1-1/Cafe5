#ifndef DLGORDERERROR_H
#define DLGORDERERROR_H

#include <QWidget>

namespace Ui {
class DlgOrderError;
}

class DlgOrderError : public QWidget
{
    Q_OBJECT

public:
    explicit DlgOrderError(const QString &message, QWidget *parent = nullptr);
    ~DlgOrderError() override;

signals:
    void acknowledged();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::DlgOrderError *ui;
};

#endif // DLGORDERERROR_H
