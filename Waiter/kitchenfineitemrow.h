#pragma once

#include <QWidget>

class QLabel;

class KitchenFineItemRow : public QWidget
{
    Q_OBJECT

public:
    explicit KitchenFineItemRow(double qty, const QString &name, const QString &comment, QWidget *parent = nullptr);

    QString lineId() const { return mLineId; }

    void setLineId(const QString &id) { mLineId = id; }

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QString mLineId;
    QLabel *mLbQty = nullptr;
    QLabel *mLbName = nullptr;
    QLabel *mLbComment = nullptr;
};
