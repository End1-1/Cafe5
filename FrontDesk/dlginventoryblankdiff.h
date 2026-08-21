#pragma once

#include <QDialog>
#include <QJsonArray>

namespace Ui {
class DlgInventoryBlankDiff;
}

class DlgInventoryBlankDiff : public QDialog
{
    Q_OBJECT

public:
    explicit DlgInventoryBlankDiff(QWidget *parent = nullptr);
    ~DlgInventoryBlankDiff() override;

    void setDiff(const QJsonArray &removed, const QJsonArray &added);

    bool removeObsolete() const { return mRemoveObsolete; }
    bool addNew() const { return mAddNew; }

    /** Returns true if user chose to apply at least one change. */
    static bool showDiff(const QJsonArray &removed,
                         const QJsonArray &added,
                         bool &doRemove,
                         bool &doAdd,
                         QWidget *parent = nullptr);

private slots:
    void onApply();
    void onRemoveOnly();
    void onAddOnly();

private:
    Ui::DlgInventoryBlankDiff *ui;
    QJsonArray mRemoved;
    QJsonArray mAdded;
    bool mRemoveObsolete = false;
    bool mAddNew = false;
};
