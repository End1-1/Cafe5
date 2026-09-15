#include "rworkstationconfigwidget.h"
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include "c5message.h"
#include "c5user.h"
#include "dict_workstation.h"
#include "ninterface.h"
#include "rworkstationconfigcommon.h"
#include "rworkstationconfigshop.h"
#include "rworkstationconfigwaiter.h"

RWorkstationConfigWidget::RWorkstationConfigWidget(QWidget *parent)
    : RAbstractSpecialWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    mLbTitle = new QLabel(this);
    mLbTitle->setWordWrap(true);
    layout->addWidget(mLbTitle);
    setLayout(layout);
}

void RWorkstationConfigWidget::addFormWidget(QWidget *widget)
{
    if(auto *box = qobject_cast<QVBoxLayout *>(layout())) {
        box->addWidget(widget, 1);
        return;
    }

    if(layout()) {
        layout()->addWidget(widget);
    }
}

void RWorkstationConfigWidget::setHeaderInfo(const QJsonObject &jdoc)
{
    mId = jdoc.value(QStringLiteral("f_id")).toInt();
    mType = jdoc.value(QStringLiteral("f_type")).toInt();
    const QString title = jdoc.value(QStringLiteral("title")).toString();
    const QString account = jdoc.value(QStringLiteral("f_station_account")).toString();
    mLbTitle->setText(QStringLiteral("%1\n%2: %3")
                          .arg(title, tr("Station account"), account));
}

void RWorkstationConfigWidget::setup(const QJsonObject &jdoc)
{
    RAbstractSpecialWidget::setup(jdoc);
    setHeaderInfo(jdoc);
    applyLookups(jdoc);
    applyConfig(jdoc.value(QStringLiteral("config")).toObject());
}

void RWorkstationConfigWidget::applyLookups(const QJsonObject &jdoc)
{
    Q_UNUSED(jdoc);
}

void RWorkstationConfigWidget::fillFiscalMachineCombo(QComboBox *combo, const QJsonArray &machines) const
{
    if (!combo) {
        return;
    }

    const int previousId = combo->currentData().toInt();
    combo->clear();
    combo->addItem(tr("Not selected"), 0);

    for (const QJsonValue &value : machines) {
        const QJsonObject row = value.toObject();
        const int id = row.value(QStringLiteral("f_id")).toInt();
        const QString name = row.value(QStringLiteral("f_name")).toString().trimmed();
        const QString ip = row.value(QStringLiteral("f_ip")).toString().trimmed();
        const int port = row.value(QStringLiteral("f_port")).toInt();
        QString label = name.isEmpty()
                            ? QStringLiteral("%1").arg(id)
                            : QStringLiteral("%1 — %2").arg(id).arg(name);
        if (!ip.isEmpty()) {
            label += QStringLiteral(" (%1:%2)").arg(ip).arg(port);
        }
        combo->addItem(label, id);
    }

    if (previousId > 0 && combo->findData(previousId) < 0) {
        combo->addItem(tr("Unknown (%1)").arg(previousId), previousId);
    }
}

void RWorkstationConfigWidget::selectFiscalMachineCombo(QComboBox *combo, int id) const
{
    if (!combo) {
        return;
    }

    int index = combo->findData(id);
    if (index < 0 && id > 0) {
        combo->addItem(tr("Unknown (%1)").arg(id), id);
        index = combo->findData(id);
    }
    combo->setCurrentIndex(index >= 0 ? index : 0);
}

int RWorkstationConfigWidget::selectedFiscalMachineId(const QComboBox *combo) const
{
    return combo ? combo->currentData().toInt() : 0;
}

void RWorkstationConfigWidget::save()
{
    if(mId <= 0) {
        return;
    }

    NInterface::query(QStringLiteral("/engine/v2/officen/editors/save"),
                      mUser->mSessionKey,
                      this,
                      {
                          {QStringLiteral("editor"), editorKey()},
                          {QStringLiteral("id"), mId},
                          {QStringLiteral("config"), collectConfig()},
                      },
                      [this](const QJsonObject &jo) {
                          applyConfig(jo.value(QStringLiteral("config")).toObject());
                          C5Message::info(tr("Saved"));
                          emit goAccept();
                      },
                      [](const QJsonObject &) { return false; },
                      true);
}

RWorkstationConfigWidget *RWorkstationConfigWidget::createForType(int type, QWidget *parent, C5User *user)
{
    RWorkstationConfigWidget *w = nullptr;
    switch (type) {
    case WORKSTATION_WAITER:
        w = new RWorkstationConfigWaiter(parent);
        break;
    case WORKSTATION_SHOP:
        w = new RWorkstationConfigShop(parent);
        break;
    case WORKSTATION_COMMON:
        w = new RWorkstationConfigCommon(parent);
        break;
    default:
        return nullptr;
    }
    w->mUser = user;
    return w;
}
