#include "rworkstationconfigwaiter.h"
#include "ui_rworkstationconfigwaiter.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>

namespace {

const QStringList kSetupButtonKeys = {
    QStringLiteral("dlgface_disable_btnguests"),
    QStringLiteral("dlgface_disable_btninprogress"),
    QStringLiteral("dlgface_disable_btnmyorders"),
    QStringLiteral("dlgface_disable_btnnormalview"),
    QStringLiteral("dlgface_disable_preorders"),
    QStringLiteral("dglorder_disable_btntransfertable"),
    QStringLiteral("dlgorder_disable_btnactivate_preorder"),
    QStringLiteral("dlgorder_disable_btnanyqty"),
    QStringLiteral("dlgorder_disable_btnbackgroup"),
    QStringLiteral("dlgorder_disable_btnchangemenu"),
    QStringLiteral("dlgorder_disable_btnchangestaff"),
    QStringLiteral("dlgorder_disable_btndeliveryamount"),
    QStringLiteral("dlgorder_disable_btndiscount"),
    QStringLiteral("dlgorder_disable_btngroupselect"),
    QStringLiteral("dlgorder_disable_btnpackage"),
    QStringLiteral("dlgorder_disable_btnpart1"),
    QStringLiteral("dlgorder_disable_btnpart2"),
    QStringLiteral("dlgorder_disable_btnpart3"),
    QStringLiteral("dlgorder_disable_btnpartfavorite"),
    QStringLiteral("dlgorder_disable_btnpreorder_datetime"),
    QStringLiteral("dlgorder_disable_btnprepaid"),
    QStringLiteral("dlgorder_disable_btnreceiptlanguage"),
    QStringLiteral("dlgorder_disable_btnshowcustomerdisplay"),
    QStringLiteral("dlgorder_disable_btnshowhideremoved"),
    QStringLiteral("dlgorder_disable_btnservice"),
    QStringLiteral("dlgorder_disable_btnsit"),
    QStringLiteral("dlgorder_disable_btnstoplistmode"),
    QStringLiteral("dlgorder_disable_btntransferdishes"),
    QStringLiteral("payment_button_1"),
    QStringLiteral("payment_button_2"),
    QStringLiteral("payment_button_3"),
    QStringLiteral("payment_button_4"),
    QStringLiteral("payment_button_5"),
    QStringLiteral("payment_button_6"),
    QStringLiteral("payment_button_7"),
    QStringLiteral("payment_button_8"),
    QStringLiteral("payment_button_9"),
};

} // namespace

RWorkstationConfigWaiter::RWorkstationConfigWaiter(QWidget *parent)
    : RWorkstationConfigWidget(parent),
      ui(new Ui::RWorkstationConfigWaiter)
{
    mForm = new QWidget(this);
    ui->setupUi(mForm);
    buildSetupButtons();
    addFormWidget(mForm);
    setMinimumWidth(720);
    setMinimumHeight(560);
}

RWorkstationConfigWaiter::~RWorkstationConfigWaiter()
{
    delete ui;
}

void RWorkstationConfigWaiter::buildSetupButtons()
{
    auto *scroll = new QScrollArea(ui->gbSetupButtons);
    scroll->setWidgetResizable(true);
    auto *inner = new QWidget(scroll);
    auto *grid = new QGridLayout(inner);
    int row = 0;
    int col = 0;

    for(const QString &key : kSetupButtonKeys) {
        auto *ch = new QCheckBox(key, inner);
        ch->setToolTip(tr("Disable button when checked"));
        mSetupButtons.insert(key, ch);
        grid->addWidget(ch, row, col);
        col++;
        if(col >= 2) {
            col = 0;
            row++;
        }
    }

    scroll->setWidget(inner);
    auto *boxLayout = new QVBoxLayout(ui->gbSetupButtons);
    boxLayout->setContentsMargins(4, 4, 4, 4);
    boxLayout->addWidget(scroll);
}

void RWorkstationConfigWaiter::applyConfig(const QJsonObject &config)
{
    ui->leCashboxId->setText(QString::number(config.value(QStringLiteral("f_cashbox_id")).toInt()));
    ui->leHallId->setText(QString::number(config.value(QStringLiteral("f_default_hall_id")).toInt()));
    ui->leFiscalId->setText(QString::number(config.value(QStringLiteral("f_fiscal_machine_id")).toInt()));
    ui->leStoreId->setText(QString::number(config.value(QStringLiteral("f_default_store_id")).toInt()));
    ui->leReceiptPhone->setText(config.value(QStringLiteral("receipt_phone")).toString());
    ui->lePrecheckPrinter->setText(config.value(QStringLiteral("precheck_printer")).toString());
    ui->pteReceiptPolicy->setPlainText(config.value(QStringLiteral("receipt_policy")).toString());
    ui->chReceiptNoTable->setChecked(config.value(QStringLiteral("receipt_no_table")).toBool(false));
    ui->chReceiptNoServiceHint->setChecked(config.value(QStringLiteral("receipt_no_service_hint")).toBool(false));
    ui->chReceiptNoDiscountHint->setChecked(config.value(QStringLiteral("receipt_no_discount_hint")).toBool(false));
    ui->leSearchMenuH->setText(QString::number(config.value(QStringLiteral("dlgsearchmenu_hsection_size")).toInt(240)));
    ui->leSearchMenuV->setText(QString::number(config.value(QStringLiteral("dlgsearchmenu_vsection_size")).toInt(100)));
    ui->chCostDependOnServiceAndDiscount->setChecked(config.value(u"cost_depend_on_service_and_discount").toBool(false));
    ui->chDoNotPrintCustomerOnReceipt->setChecked(config.value(u"do_not_print_customer_on_receipt").toBool(false));
    ui->leCostumerNotification->setChecked(config.value(QStringLiteral("customer_notification")).toBool(false));
    ui->leRecentDishesMinutes->setText(QString::number(config.value(QStringLiteral("recent_dishes_minutes")).toInt(40)));

    const QJsonObject buttons = config.value(QStringLiteral("setup_buttons")).toObject();
    for(auto it = mSetupButtons.begin(); it != mSetupButtons.end(); ++it) {
        it.value()->setChecked(buttons.value(it.key()).toBool());
    }
}

QJsonObject RWorkstationConfigWaiter::collectConfig() const
{
    QJsonObject jo;
    jo.insert(u"cost_depend_on_service_and_discount", ui->chCostDependOnServiceAndDiscount->isChecked());
    jo.insert(u"do_not_print_customer_on_receipt", ui->chDoNotPrintCustomerOnReceipt->isChecked());
    jo.insert(QStringLiteral("customer_notification"), ui->leCostumerNotification->isChecked());
    jo.insert(QStringLiteral("f_cashbox_id"), ui->leCashboxId->text().trimmed().toInt());
    jo.insert(QStringLiteral("f_default_hall_id"), ui->leHallId->text().trimmed().toInt());
    jo.insert(QStringLiteral("f_fiscal_machine_id"), ui->leFiscalId->text().trimmed().toInt());
    jo.insert(QStringLiteral("f_default_store_id"), ui->leStoreId->text().trimmed().toInt());
    jo.insert(QStringLiteral("receipt_phone"), ui->leReceiptPhone->text().trimmed());
    jo.insert(QStringLiteral("precheck_printer"), ui->lePrecheckPrinter->text().trimmed());
    jo.insert(QStringLiteral("receipt_policy"), ui->pteReceiptPolicy->toPlainText());
    jo.insert(QStringLiteral("receipt_no_table"), ui->chReceiptNoTable->isChecked());
    jo.insert(QStringLiteral("receipt_no_service_hint"), ui->chReceiptNoServiceHint->isChecked());
    jo.insert(QStringLiteral("receipt_no_discount_hint"), ui->chReceiptNoDiscountHint->isChecked());
    jo.insert(QStringLiteral("dlgsearchmenu_hsection_size"), ui->leSearchMenuH->text().trimmed().toInt());
    jo.insert(QStringLiteral("dlgsearchmenu_vsection_size"), ui->leSearchMenuV->text().trimmed().toInt());
    int recentMinutes = ui->leRecentDishesMinutes->text().trimmed().toInt();
    if (recentMinutes <= 0) {
        recentMinutes = 40;
    }
    jo.insert(QStringLiteral("recent_dishes_minutes"), recentMinutes);

    QJsonObject buttons;
    for(auto it = mSetupButtons.begin(); it != mSetupButtons.end(); ++it) {
        buttons.insert(it.key(), it.value()->isChecked());
    }
    jo.insert(QStringLiteral("setup_buttons"), buttons);
    return jo;
}
