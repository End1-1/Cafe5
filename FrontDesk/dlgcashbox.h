#pragma once

#include <QDialog>

class C5User;

namespace Ui
{
class DlgCashbox;
}

class DlgCashbox : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCashbox(C5User *user, int id, QWidget *parent = nullptr);
    ~DlgCashbox() override;

    int recordId() const { return mId; }

private slots:
    void loadData();
    void trySave();
    void tryDelete();

private:
    void applyRow(const QJsonObject &row);

    Ui::DlgCashbox *ui = nullptr;
    C5User *mUser = nullptr;
    int mId = 0;
};
