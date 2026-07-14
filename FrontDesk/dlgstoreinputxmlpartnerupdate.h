#pragma once

#include <QDialog>
#include <QJsonObject>

namespace Ui
{
class DlgStoreInputXmlPartnerUpdate;
}

class DlgStoreInputXmlPartnerUpdate : public QDialog
{
    Q_OBJECT

public:
    explicit DlgStoreInputXmlPartnerUpdate(const QJsonObject &partner,
                                           const QString &importTaxName,
                                           const QString &importAddress,
                                           QWidget *parent = nullptr);

    ~DlgStoreInputXmlPartnerUpdate() override;

    QJsonObject updatedPartner() const;

private slots:
    void tryAccept();

private:
    Ui::DlgStoreInputXmlPartnerUpdate *ui;

    QJsonObject mPartner;
};
