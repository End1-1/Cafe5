#include "wplast30sessions.h"
#include "ui_wplast30sessions.h"
#include "ninterface.h"
#include "c5user.h"
#include <QJsonArray>
#include <QListWidgetItem>

WPLast30Sessions::WPLast30Sessions(C5User *user, QWidget *parent)
    : WPWidget(user, parent), ui(new Ui::WPLast30Sessions)
{
    ui->setupUi(this);
}

WPLast30Sessions::~WPLast30Sessions() { delete ui; }

void WPLast30Sessions::setup()
{
    NInterface::query1("/engine/v2/waiter/reports/last-30-sessions", mUser->mSessionKey, this, {{}},
    [this](const QJsonObject & jdoc) {
        for(auto const &v : jdoc["values"].toArray()) {
            const QJsonObject &jo = v.toObject();
            auto *item = new QListWidgetItem(ui->lst);
            item->setText(jo["text"].toString());
            item->setData(Qt::UserRole, jo["value"].toInt());
            item->setSizeHint(QSize(50, 50));
            ui->lst->addItem(item);
        }
    });
}

void WPLast30Sessions::on_lst_itemClicked(QListWidgetItem *item)
{
    mParams["session_id"] = item->data(Qt::UserRole).toInt();
}
