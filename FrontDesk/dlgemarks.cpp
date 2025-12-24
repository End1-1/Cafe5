#include "dlgemarks.h"
#include "ui_dlgemarks.h"
#include "ninterface.h"

DlgEmarks::DlgEmarks(C5User *user, const QStringList &emarks)
    : C5Dialog(user)
    , ui(new Ui::DlgEmarks)
{
    ui->setupUi(this);

    for(auto const &s : emarks) {
        QListWidgetItem *item = new QListWidgetItem(ui->lst);
        item->setText(s);
        ui->lst->addItem(s);
    }
}

DlgEmarks::~DlgEmarks()
{
    delete ui;
}

void DlgEmarks::checkQrResponse(const QJsonObject &jdoc)
{
    fHttp->httpQueryFinished(sender());
}

void DlgEmarks::on_leCode_returnPressed()
{
    QJsonObject jo;
    fHttp->createHttpQuery("/engine/picasso.store/", jo, SLOT(checkQrResponse(QJsonObject)));
}
