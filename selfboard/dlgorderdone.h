#ifndef DLGORDERDONE_H
#define DLGORDERDONE_H

#include <QWidget>

class QLabel;
class QPaintEvent;
class QPushButton;

class DlgOrderDone : public QWidget
{
    Q_OBJECT

public:
    explicit DlgOrderDone(const QString &orderNumber, QWidget *parent = nullptr);

signals:
    void acknowledged();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onAcknowledge();

private:
    QLabel *m_lblTitle = nullptr;
    QLabel *m_lblOrderNumber = nullptr;
    QLabel *m_lblHint = nullptr;
    QPushButton *m_btnOk = nullptr;
};

#endif // DLGORDERDONE_H
