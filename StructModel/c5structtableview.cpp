#include "c5structtableview.h"
#include <QHeaderView>
#include <QJsonDocument>
#include "appwebsocket.h"
#include "c5message.h"
#include "store_doc_status.h"
#include "store_doc_type.h"
#include "struct_cashbox.h"
#include "struct_currency.h"
#include "struct_goods_group.h"
#include "struct_goods_item.h"
#include "struct_partner.h"
#include "struct_payment_type.h"
#include "struct_storage_item.h"
#include "struct_employee.h"
#include "struct_employee_group.h"
#include "struct_goods_type.h"
#include "ui_c5structtableview.h"

C5StructTableView::C5StructTableView(C5User *user)
    : C5Dialog(user), ui(new Ui::C5StructTableView)
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
        if(!jo.contains("result")) {
            if(jo.contains(QStringLiteral("error")) || jo.contains(QStringLiteral("errorMessage"))
               || jo.contains(QStringLiteral("errorCode"))) {
                if(jo.contains(QStringLiteral("requestId")) && !mLastRequestId.isEmpty()
                    && jo.value(QStringLiteral("requestId")).toString() != mLastRequestId) {
                    return;
                }
                const QString err = jo.value(QStringLiteral("errorMessage")).toString();
                C5Message::error(
                    !err.isEmpty() ? err
                    : (jo.value(QStringLiteral("error")).isString() ? jo.value(QStringLiteral("error")).toString()
                        : C5StructTableView::tr("Server error while loading the list.")));
            }
            return;
        }

        QJsonArray arr = jo["result"].toArray();

        if(mSearchEngine == SelectorName<StorageItem>::value) {
            handleSearchResult<StorageItem>(arr, static_cast<C5StructModel<StorageItem>*>(ui->tbl->model()));
        } else if(mSearchEngine == SelectorName<GoodsItem>::value) {
            handleSearchResult<GoodsItem>(arr, static_cast<C5StructModel<GoodsItem>*>(ui->tbl->model()));
        } else if(mSearchEngine == SelectorName<PartnerItem>::value) {
            handleSearchResult<PartnerItem>(arr, static_cast<C5StructModel<PartnerItem>*>(ui->tbl->model()));
        } else if (mSearchEngine == SelectorName<StoreDocStatusItem>::value) {
            handleSearchResult<StoreDocStatusItem>(arr, static_cast<C5StructModel<StoreDocStatusItem> *>(ui->tbl->model()));
        } else if (mSearchEngine == SelectorName<StoreDocTypeItem>::value) {
            handleSearchResult<StoreDocTypeItem>(arr, static_cast<C5StructModel<StoreDocTypeItem> *>(ui->tbl->model()));
        } else if (mSearchEngine == SelectorName<StoreDocTypeItem>::value) {
            handleSearchResult<GoodsGroupItem>(arr, static_cast<C5StructModel<GoodsGroupItem> *>(ui->tbl->model()));
        } else if (mSearchEngine == SelectorName<StructCurrency>::value) {
            handleSearchResult<StructCurrency>(arr, static_cast<C5StructModel<StructCurrency> *>(ui->tbl->model()));
        } else if (mSearchEngine == SelectorName<StructCashbox>::value) {
            handleSearchResult<StructCashbox>(arr, static_cast<C5StructModel<StructCashbox> *>(ui->tbl->model()));
        } else if (mSearchEngine == SelectorName<StructPaymentType>::value) {
            handleSearchResult<StructPaymentType>(arr, static_cast<C5StructModel<StructPaymentType> *>(ui->tbl->model()));
        } else if (mSearchEngine == SelectorName<StructGoodsType>::value) {
            handleSearchResult<StructGoodsType>(arr, static_cast<C5StructModel<StructGoodsType> *>(ui->tbl->model()));
        } else if (mSearchEngine == SelectorName<StructEmployee>::value) {
            handleSearchResult<StructEmployee>(arr, static_cast<C5StructModel<StructEmployee> *>(ui->tbl->model()));
        } else if (mSearchEngine == SelectorName<StructEmployeeGroup>::value) {
            handleSearchResult<StructEmployeeGroup>(arr, static_cast<C5StructModel<StructEmployeeGroup> *>(ui->tbl->model()));
        } else {
            Q_ASSERT_X(false, Q_FUNC_INFO, qPrintable(QString("Unknown search engine: %1").arg(mSearchEngine)));
        }

        ui->tbl->resizeColumnsToContents();
    });
}

C5StructTableView::~C5StructTableView() { delete ui; }

QTableView* C5StructTableView::tableView()
{
    return ui->tbl;
}

QMap<QString, QString> C5StructTableView::selectorTitles()
{
    return {{SelectorName<GoodsItem>::value, QObject::tr("Goods")},
            {SelectorName<GoodsGroupItem>::value, QObject::tr("Group")},
            {SelectorName<StorageItem>::value, QObject::tr("Storage")},
            {SelectorName<PartnerItem>::value, QObject::tr("Partner")},
            {SelectorName<StoreDocTypeItem>::value, QObject::tr("Document type")},
            {SelectorName<StoreDocStatusItem>::value, QObject::tr("Document status")},
            {SelectorName<StructCurrency>::value, QObject::tr("Currency")},
            {SelectorName<StructPaymentType>::value, QObject::tr("Payment type")},
            {SelectorName<StructGoodsType>::value, QObject::tr("Goods type")},
            {SelectorName<StructCashbox>::value, QObject::tr("Cashbox")},
            {SelectorName<StructEmployee>::value, QObject::tr("Employee")},
            {SelectorName<StructEmployeeGroup>::value, QObject::tr("Employee group")}};
}

void C5StructTableView::on_leSearchText_textChanged(const QString &arg1)
{
    if(arg1.isEmpty()) {
        if(!mEmptySearch) {
            return;
        }
    }

    mSearchTimer->start(mSearchDelay);
}

void C5StructTableView::sendSearchRequest()
{
    if(!AppWebSocket::instance) {
        C5Message::error(tr("WebSocket is not initialized. Restart the application."));
        return;
    }
    if(!AppWebSocket::instance->isConnected()) {
        C5Message::error(tr("No connection to the server. Check the network and try again."));
        return;
    }
    QString text = ui->leSearchText->text().trimmed();
    QJsonObject jo;
    jo["command"] = mSearchEngine;
    jo["lower_name"] = text.toLower();
    jo["requestId"] = QUuid::createUuid().toString(QUuid::WithoutBraces);
    mLastRequestId = jo["requestId"].toString();
    qDebug() << "Sending request" << jo;
    if(!AppWebSocket::instance->sendBinaryMessage(QJsonDocument(jo).toJson())) {
        C5Message::error(tr("Failed to send the search request. Check the connection to the server."));
    }
}

void C5StructTableView::on_btnSelect_clicked()
{
    bool isAcceptable = false;

    if(mSearchEngine == SelectorName<StorageItem>::value) {
        if(static_cast<C5StructModel<StorageItem>*>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if(mSearchEngine == SelectorName<GoodsItem>::value) {
        if(static_cast<C5StructModel<GoodsItem>*>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if(mSearchEngine == SelectorName<PartnerItem>::value) {
        if(static_cast<C5StructModel<GoodsItem>*>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if (mSearchEngine == SelectorName<StoreDocStatusItem>::value) {
        if (static_cast<C5StructModel<StoreDocStatusItem> *>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if (mSearchEngine == SelectorName<StoreDocTypeItem>::value) {
        if (static_cast<C5StructModel<StoreDocTypeItem> *>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if (mSearchEngine == SelectorName<StructCurrency>::value) {
        if (static_cast<C5StructModel<StructCurrency> *>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if (mSearchEngine == SelectorName<StructCashbox>::value) {
        if (static_cast<C5StructModel<StructCashbox> *>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if (mSearchEngine == SelectorName<StructPaymentType>::value) {
        if (static_cast<C5StructModel<StructPaymentType> *>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if (mSearchEngine == SelectorName<StructGoodsType>::value) {
        if (static_cast<C5StructModel<StructGoodsType> *>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if (mSearchEngine == SelectorName<StructEmployee>::value) {
        if (static_cast<C5StructModel<StructEmployee> *>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if (mSearchEngine == SelectorName<StructEmployeeGroup>::value) {
        if (static_cast<C5StructModel<StructEmployeeGroup> *>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    } else if (mSearchEngine == SelectorName<GoodsGroupItem>::value) {
        if (static_cast<C5StructModel<GoodsGroupItem> *>(ui->tbl->model())->hasSelectedData()) {
            isAcceptable = true;
        }
    }

    if (isAcceptable) {
        accept();
    }
}

void C5StructTableView::on_tbl_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }

    QModelIndex checkIndex = ui->tbl->model()->index(index.row(), 0);

    if(mSearchEngine == SelectorName<StorageItem>::value) {
        static_cast<C5StructModel<StorageItem>*>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if(mSearchEngine == SelectorName<GoodsItem>::value) {
        static_cast<C5StructModel<GoodsItem>*>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if(mSearchEngine == SelectorName<PartnerItem>::value) {
        static_cast<C5StructModel<PartnerItem>*>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if (mSearchEngine == SelectorName<StoreDocStatusItem>::value) {
        static_cast<C5StructModel<StoreDocStatusItem> *>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if (mSearchEngine == SelectorName<GoodsGroupItem>::value) {
        static_cast<C5StructModel<GoodsGroupItem> *>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if (mSearchEngine == SelectorName<StoreDocTypeItem>::value) {
        static_cast<C5StructModel<StoreDocTypeItem> *>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if (mSearchEngine == SelectorName<StructCurrency>::value) {
        static_cast<C5StructModel<StructCurrency> *>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if (mSearchEngine == SelectorName<StructCashbox>::value) {
        static_cast<C5StructModel<StructCashbox> *>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if (mSearchEngine == SelectorName<StructPaymentType>::value) {
        static_cast<C5StructModel<StructPaymentType> *>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if (mSearchEngine == SelectorName<StructGoodsType>::value) {
        static_cast<C5StructModel<StructGoodsType> *>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if (mSearchEngine == SelectorName<StructEmployee>::value) {
        static_cast<C5StructModel<StructEmployee> *>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else if (mSearchEngine == SelectorName<StructEmployeeGroup>::value) {
        static_cast<C5StructModel<StructEmployeeGroup> *>(ui->tbl->model())->setData(checkIndex, Qt::Checked, Qt::CheckStateRole);
    } else {
        Q_ASSERT_X(false, Q_FUNC_INFO, qPrintable(QString("Unknown search engine: %1").arg(mSearchEngine)));
    }

    on_btnSelect_clicked();
}

void C5StructTableView::on_btnCancel_clicked()
{
    reject();
}
