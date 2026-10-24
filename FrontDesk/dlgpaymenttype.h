#pragma once

#include <QDialog>

class C5User;
class QJsonObject;

namespace Ui
{
class DlgPaymentType;
}

class DlgPaymentType : public QDialog
{
    Q_OBJECT

public:
    explicit DlgPaymentType(C5User *user, int id, QWidget *parent = nullptr);
    ~DlgPaymentType() override;

private slots:
    void loadData();
    void trySave();

private:
    void applyRow(const QJsonObject &row);

    Ui::DlgPaymentType *ui = nullptr;
    C5User *mUser = nullptr;
    int mId = 0;
};
