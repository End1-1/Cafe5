#include "rworkstationconfigwidget.h"
#include "rworkstationconfigcommon.h"
#include "rworkstationconfigshop.h"
#include "rworkstationconfigwaiter.h"
#include "dict_workstation.h"
#include "c5message.h"
#include "ninterface.h"
#include <QLabel>
#include <QVBoxLayout>

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
    applyConfig(jdoc.value(QStringLiteral("config")).toObject());
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
