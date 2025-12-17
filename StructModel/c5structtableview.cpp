#include "c5structtableview.h"
#include "ui_c5structtableview.h"
#include "appwebsocket.h"
#include "struct_storage_item.h"
#include "struct_goods_item.h"
#include <QHeaderView>
#include <QJsonDocument>

C5StructTableView::C5StructTableView(QWidget *parent)
    : QDialog(parent), ui(new Ui::C5StructTableView)
{
    ui->setupUi(this);
    ui->tbl->setSortingEnabled(true);
    ui->tbl->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tbl->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbl->horizontalHeader()->setStretchLastSection(true);
    mSearchTimer = new QTimer(this);
    mSearchTimer->setSingleShot(true);
    connect(mSearchTimer, &QTimer::timeout,
            this, &C5StructTableView::sendSearchRequest);
    connect(AppWebSocket::instance, &AppWebSocket::bMessageReceived,
    this, [this](const QJsonObject & jo) {
        if(!jo.contains("result"))
            return;

        QJsonArray arr = jo["result"].toArray();

        if(mSearchEngine == search_storage) {
            handleSearchResult<StorageItem>(arr, static_cast<C5StructModel<StorageItem>*>(ui->tbl->model()));
        } else if(mSearchEngine == search_goods) {
            handleSearchResult<GoodsItem>(arr, static_cast<C5StructModel<GoodsItem>*>(ui->tbl->model()));
        }
    });
}

C5StructTableView::~C5StructTableView() { delete ui; }

QTableView* C5StructTableView::tableView()
{
    return ui->tbl;
}

void C5StructTableView::on_leSearchText_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    mSearchTimer->start(mSearchDelay);
}

void C5StructTableView::sendSearchRequest()
{
    QString text = ui->leSearchText->text().trimmed();
    QJsonObject jo;
    jo["command"] = mSearchEngine;
    jo["lower_name"] = text.toLower();
    jo["requestId"] = QUuid::createUuid().toString(QUuid::WithoutBraces);
    AppWebSocket::instance->sendBinaryMessage(QJsonDocument(jo).toJson());
}

void C5StructTableView::on_btnSelect_clicked()
{
    bool isAcceptable = false;

    if(mSearchEngine == search_storage) {
        if(static_cast<C5StructModel<StorageItem>*>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if(mSearchEngine == search_goods) {
        if(static_cast<C5StructModel<GoodsItem>*>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    }

    if(isAcceptable) {
        accept();
    }
}

void C5StructTableView::on_tbl_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }

    if(mSearchEngine == search_storage) {
        static_cast<C5StructModel<StorageItem>*>(ui->tbl->model())->setData(index, Qt::Checked, Qt::CheckStateRole);
    } else {
        static_cast<C5StructModel<GoodsItem>*>(ui->tbl->model())->setData(index, Qt::Checked, Qt::CheckStateRole);
    }
}
