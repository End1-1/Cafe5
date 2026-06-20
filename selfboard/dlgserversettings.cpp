#include "dlgserversettings.h"
#include "ui_dlgserversettings.h"

#include "appsettings.h"
#include "serverconfig.h"

#include <QCheckBox>
#include <QPointer>

DlgServerSettings::DlgServerSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgServerSettings)
{
    ui->setupUi(this);
    setModal(true);
    loadFields();

    connect(ui->cbPaymentTestStub, &QCheckBox::toggled, this, [](bool enabled) {
        AppSettings::setPaymentTestStub(enabled);
        AppSettings::saveToSettings();
    });
}

DlgServerSettings::~DlgServerSettings()
{
    delete ui;
}

void DlgServerSettings::loadFields()
{
    ServerConfig::loadFromSettings();
    AppSettings::loadFromSettings();
    ui->cbPaymentTestStub->blockSignals(true);
    ui->cbPaymentTestStub->setChecked(AppSettings::paymentTestStub());
    ui->cbPaymentTestStub->blockSignals(false);
    ui->sbTableTakeAway->setValue(AppSettings::tableTakeAway());
    ui->sbTableDineIn->setValue(AppSettings::tableDineIn());
    ui->sbCashboxId->setValue(AppSettings::cashboxId());
    ui->sbServiceFactor->setValue(AppSettings::serviceFactor());
    ui->leServerHost->setText(ServerConfig::host());
    ui->cbUseHttps->setChecked(ServerConfig::useHttps());
    ui->leUsername->setText(ServerConfig::username());
    ui->lePassword->setText(ServerConfig::password());
}

void DlgServerSettings::applyFields()
{
    AppSettings::setPaymentTestStub(ui->cbPaymentTestStub->isChecked());
    AppSettings::setTableTakeAway(ui->sbTableTakeAway->value());
    AppSettings::setTableDineIn(ui->sbTableDineIn->value());
    AppSettings::setCashboxId(ui->sbCashboxId->value());
    AppSettings::setServiceFactor(ui->sbServiceFactor->value());
    AppSettings::saveToSettings();
    ServerConfig::setHost(ui->leServerHost->text());
    ServerConfig::setUseHttps(ui->cbUseHttps->isChecked());
    ServerConfig::setUsername(ui->leUsername->text());
    ServerConfig::setPassword(ui->lePassword->text());
    ServerConfig::saveToSettings();
}

void DlgServerSettings::on_btnCancel_clicked()
{
    reject();
}

void DlgServerSettings::on_btnSave_clicked()
{
    applyFields();
    ui->lblStatus->clear();
    ui->btnSave->setEnabled(false);
    ui->btnCancel->setEnabled(false);
    ui->lblStatus->setStyleSheet(QStringLiteral("color: #333;"));
    ui->lblStatus->setText(tr("Connecting..."));

    QPointer<DlgServerSettings> self(this);
    ServerConfig::login(this, [self](bool ok, const QString &error) {
        if (!self) {
            return;
        }
        self->ui->btnSave->setEnabled(true);
        self->ui->btnCancel->setEnabled(true);
        if (!ok) {
            self->m_sessionOk = false;
            self->ui->lblStatus->setStyleSheet(QStringLiteral("color: #c0392b;"));
            self->ui->lblStatus->setText(error);
            return;
        }
        self->m_sessionOk = true;
        self->accept();
    });
}
