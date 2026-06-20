#include "c5cashtransfer.h"

#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QVBoxLayout>
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "c5user.h"
#include "c5utils.h"
#include "ninterface.h"
#include "struct_cashbox.h"

C5CashTransfer::C5CashTransfer(C5User *user, QWidget *parent)
    : C5Dialog(user, parent)
{
    setWindowTitle(tr("Cash movement"));

    auto *root = new QVBoxLayout(this);

    auto *grid = new QGridLayout();
    grid->setColumnStretch(1, 1);

    grid->addWidget(new QLabel(tr("Source cashbox")), 0, 0);
    mLeSource = new QLineEdit();
    mLeSource->setReadOnly(true);
    grid->addWidget(mLeSource, 0, 1);
    auto *btnSource = new QToolButton();
    btnSource->setText(QStringLiteral("..."));
    grid->addWidget(btnSource, 0, 2);

    grid->addWidget(new QLabel(tr("Destination cashbox")), 1, 0);
    mLeDest = new QLineEdit();
    mLeDest->setReadOnly(true);
    grid->addWidget(mLeDest, 1, 1);
    auto *btnDest = new QToolButton();
    btnDest->setText(QStringLiteral("..."));
    grid->addWidget(btnDest, 1, 2);

    grid->addWidget(new QLabel(tr("Comment")), 2, 0);
    mLeComment = new QLineEdit();
    grid->addWidget(mLeComment, 2, 1, 1, 2);

    root->addLayout(grid);

    auto *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    root->addWidget(line);

    mFundsHint = new QLabel(tr("Select source cashbox to load available funds"));
    mFundsHint->setWordWrap(true);
    root->addWidget(mFundsHint);

    mFundsGrid = new QGridLayout();
    mFundsGrid->setColumnStretch(1, 1);
    root->addLayout(mFundsGrid);

    root->addStretch(1);

    auto *btnRow = new QHBoxLayout();
    btnRow->addStretch(1);
    auto *btnSave = new QPushButton(tr("Save"));
    auto *btnCancel = new QPushButton(tr("Cancel"));
    btnRow->addWidget(btnSave);
    btnRow->addWidget(btnCancel);
    root->addLayout(btnRow);

    connect(btnSource, &QToolButton::clicked, this, &C5CashTransfer::on_btnSource_clicked);
    connect(btnDest, &QToolButton::clicked, this, &C5CashTransfer::on_btnDest_clicked);
    connect(btnSave, &QPushButton::clicked, this, &C5CashTransfer::on_btnSave_clicked);
    connect(btnCancel, &QPushButton::clicked, this, &C5CashTransfer::on_btnCancel_clicked);

    resize(520, 360);
}

C5CashTransfer::~C5CashTransfer()
{
}

void C5CashTransfer::setSourceCashbox(int cashboxId, const QString &name)
{
    mSourceId = cashboxId;
    mLeSource->setText(name);
    loadSourceFunds();
}

void C5CashTransfer::setCurrencyId(int currencyId)
{
    mCurrencyId = currencyId > 0 ? currencyId : 1;
    if(mSourceId > 0) {
        loadSourceFunds();
    }
}

void C5CashTransfer::on_btnSource_clicked()
{
    const auto r = selectItem<StructCashbox>(true, false, mapPointToGlobal(mLeSource));
    if(r.isEmpty()) {
        return;
    }
    const auto &cb = r.first();
    mSourceId = cb.id;
    mLeSource->setText(cb.name);
    loadSourceFunds();
}

void C5CashTransfer::on_btnDest_clicked()
{
    const auto r = selectItem<StructCashbox>(true, false, mapPointToGlobal(mLeDest));
    if(r.isEmpty()) {
        return;
    }
    const auto &cb = r.first();
    mDestId = cb.id;
    mLeDest->setText(cb.name);
}

void C5CashTransfer::clearFundRows()
{
    mRows.clear();

    while(QLayoutItem *item = mFundsGrid->takeAt(0)) {
        if(QWidget *w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }
}

void C5CashTransfer::rebuildFundRows(const QJsonArray &funds)
{
    clearFundRows();

    if(funds.isEmpty()) {
        mFundsHint->setText(tr("No available funds in this cashbox"));
        return;
    }

    mFundsHint->setText(tr("Enter amount to move or click Available to fill"));

    auto *hName = new QLabel(tr("Funds"));
    auto *hAmount = new QLabel(tr("Amount"));
    auto *hAvailable = new QLabel(tr("Available"));
    QFont bold = hName->font();
    bold.setBold(true);
    hName->setFont(bold);
    hAmount->setFont(bold);
    hAvailable->setFont(bold);
    mFundsGrid->addWidget(hName, 0, 0);
    mFundsGrid->addWidget(hAmount, 0, 1);
    mFundsGrid->addWidget(hAvailable, 0, 2);

    int row = 1;
    for(const QJsonValue &fv : funds) {
        const QJsonObject fo = fv.toObject();
        FundRow fr;
        fr.paymentTypeId = fo.value(QStringLiteral("f_payment_type_id")).toInt();
        fr.available = fo.value(QStringLiteral("f_available")).toDouble();

        mFundsGrid->addWidget(new QLabel(fo.value(QStringLiteral("f_name")).toString()), row, 0);

        fr.amount = new QDoubleSpinBox();
        fr.amount->setDecimals(2);
        fr.amount->setMinimum(0.0);
        fr.amount->setMaximum(fr.available);
        fr.amount->setValue(0.0);
        mFundsGrid->addWidget(fr.amount, row, 1);

        auto *lbAvailable = new QLabel(
            QStringLiteral("<a href=\"fill\" style=\"color:#4895d0;\">%1</a>").arg(float_str(fr.available, 2)));
        lbAvailable->setTextFormat(Qt::RichText);
        lbAvailable->setTextInteractionFlags(Qt::TextBrowserInteraction);
        lbAvailable->setCursor(Qt::PointingHandCursor);
        lbAvailable->setToolTip(tr("Click to fill amount"));
        connect(lbAvailable, &QLabel::linkActivated, fr.amount, [fr](const QString &) {
            fr.amount->setValue(fr.available);
            fr.amount->setFocus();
        });
        mFundsGrid->addWidget(lbAvailable, row, 2);

        mRows.append(fr);
        ++row;
    }
}

void C5CashTransfer::loadSourceFunds()
{
    if(mSourceId <= 0) {
        return;
    }

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/get-cashbox-funds"),
                       mUser->mSessionKey,
                       this,
                       {
                           {QStringLiteral("cashbox_id"), mSourceId},
                           {QStringLiteral("currency_id"), mCurrencyId},
                       },
                       [this](const QJsonObject &jo) {
                           rebuildFundRows(jo.value(QStringLiteral("funds")).toArray());
                       });
}

void C5CashTransfer::on_btnSave_clicked()
{
    if(mSourceId <= 0) {
        C5Message::error(tr("Source cashbox is required"));
        return;
    }
    if(mDestId <= 0) {
        C5Message::error(tr("Destination cashbox is required"));
        return;
    }
    if(mSourceId == mDestId) {
        C5Message::error(tr("Source and destination must differ"));
        return;
    }

    QJsonArray items;
    for(const FundRow &fr : mRows) {
        const double amount = fr.amount->value();
        if(amount <= 0.009) {
            continue;
        }
        if(amount - fr.available > 0.009) {
            C5Message::error(tr("Amount exceeds available funds"));
            return;
        }
        items.append(QJsonObject{
            {"f_payment_type_id", fr.paymentTypeId},
            {"amount", amount},
        });
    }

    if(items.isEmpty()) {
        C5Message::error(tr("Amount must be greater than zero"));
        return;
    }

    QJsonObject params{
        {QStringLiteral("source_cashbox_id"), mSourceId},
        {QStringLiteral("dest_cashbox_id"), mDestId},
        {QStringLiteral("currency_id"), mCurrencyId},
        {QStringLiteral("comment"), mLeComment->text().trimmed()},
        {QStringLiteral("items"), items},
    };

    NInterface::query1(QStringLiteral("/engine/v2/waiter/cashbox/transfer-funds"),
                       mUser->mSessionKey,
                       this,
                       params,
                       [this](const QJsonObject &) {
                           accept();
                       });
}

void C5CashTransfer::on_btnCancel_clicked()
{
    reject();
}
