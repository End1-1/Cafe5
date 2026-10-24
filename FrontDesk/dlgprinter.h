#pragma once

#include <QDialog>
#include <QJsonObject>

class C5User;

namespace Ui
{
class DlgPrinter;
}

class DlgPrinter : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPrinter(C5User *user, int id, QWidget *parent = nullptr);
    ~DlgPrinter() override;

    int recordId() const { return mId; }

private slots:
    void loadData();
    void trySave();
    void tryDelete();

private:
    void applyRow(const QJsonObject &row);

    Ui::DlgPrinter *ui = nullptr;
    C5User *mUser = nullptr;
    int mId = 0;
};
