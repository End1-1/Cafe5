#pragma once

#include <QDialog>

class C5User;

namespace Ui
{
class DlgFiscalMachine;
}

class DlgFiscalMachine : public QDialog
{
    Q_OBJECT

public:
    explicit DlgFiscalMachine(C5User *user, int id, QWidget *parent = nullptr);
    ~DlgFiscalMachine() override;

    int recordId() const { return mId; }

private slots:
    void loadData();
    void trySave();
    void tryDelete();

private:
    void applyRow(const QJsonObject &row);

    Ui::DlgFiscalMachine *ui = nullptr;
    C5User *mUser = nullptr;
    int mId = 0;
};
