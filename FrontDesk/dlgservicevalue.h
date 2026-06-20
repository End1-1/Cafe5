#pragma once

#include <QDialog>

class C5User;

namespace Ui
{
class DlgServiceValue;
}

class DlgServiceValue : public QDialog
{
    Q_OBJECT

public:
    explicit DlgServiceValue(C5User *user, int id, QWidget *parent = nullptr);
    ~DlgServiceValue() override;

    int recordId() const { return mId; }

private slots:
    void loadData();
    void trySave();
    void tryDelete();

private:
    Ui::DlgServiceValue *ui = nullptr;
    C5User *mUser = nullptr;
    int mId = 0;
};
