#pragma once

#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>

class C5User;

namespace Ui
{
class DlgPrintAlias;
}

class DlgPrintAlias : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPrintAlias(C5User *user, int id, QWidget *parent = nullptr);
    ~DlgPrintAlias() override;

    int recordId() const { return mId; }

private slots:
    void loadData();
    void trySave();
    void tryDelete();

private:
    void applyRow(const QJsonObject &row);
    void fillPrinters(const QJsonArray &printers);

    Ui::DlgPrintAlias *ui = nullptr;
    C5User *mUser = nullptr;
    int mId = 0;
};
