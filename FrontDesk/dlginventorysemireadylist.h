#pragma once

#include "semiready_batch.h"
#include <QDialog>
#include <QVector>

namespace Ui
{
class DlgInventorySemireadyList;
}

class DlgInventorySemireadyList : public QDialog
{
    Q_OBJECT

public:
    explicit DlgInventorySemireadyList(QWidget *parent = nullptr);
    ~DlgInventorySemireadyList() override;

    void setBatches(const QVector<SemireadyBatch> &batches);

signals:
    void removeBatchRequested(const QString &batchId);

private slots:
    void onDeleteClicked();

private:
    Ui::DlgInventorySemireadyList *ui = nullptr;
};
