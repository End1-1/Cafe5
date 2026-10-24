#pragma once

#include <QDialog>

class C5User;
class QJsonObject;

namespace Ui
{
class DlgApiKey;
}

class DlgApiKey : public QDialog
{
    Q_OBJECT

public:
    explicit DlgApiKey(C5User *user, const QString &partner, QWidget *parent = nullptr);
    ~DlgApiKey() override;

private slots:
    void loadData();
    void trySave();

private:
    void applyRow(const QJsonObject &row);

    Ui::DlgApiKey *ui = nullptr;
    C5User *mUser = nullptr;
    QString mPartner;
};
