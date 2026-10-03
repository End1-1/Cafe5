#ifndef DLGUPDATEREQUIRED_H
#define DLGUPDATEREQUIRED_H

#include <QDialog>

namespace Ui {
class DlgUpdateRequired;
}

class DlgUpdateRequired : public QDialog
{
    Q_OBJECT

public:
    explicit DlgUpdateRequired(QWidget *parent = nullptr);
    ~DlgUpdateRequired() override;

    void setUpdateInfo(const QString &message, const QString &oldVersion, const QString &newVersion);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_btnYes_clicked();
    void on_btnNo_clicked();

private:
    Ui::DlgUpdateRequired *ui;
    QPoint m_dragOffset;
    bool m_dragging = false;
};

#endif // DLGUPDATEREQUIRED_H
