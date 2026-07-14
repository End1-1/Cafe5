#include "dlgstoreinputxmlgoodsmatch.h"
#include "c5codenameselector.h"
#include "c5codenameselectorfunctions.h"
#include "c5message.h"
#include "ui_dlgstoreinputxmlgoodsmatch.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTableWidgetItem>

DlgStoreInputXmlGoodsMatch::DlgStoreInputXmlGoodsMatch(const QVector<StoreInputXmlGoodLine> &lines, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DlgStoreInputXmlGoodsMatch)
    , mSourceLines(lines)
{
    ui->setupUi(this);
    ui->tblGoods->setColumnCount(6);
    ui->tblGoods->setHorizontalHeaderLabels(
        {tr("Import name"), tr("Qty"), tr("Price"), tr("Unit"), tr("Goods in database"), tr("Update name")});
    ui->tblGoods->horizontalHeader()->setStretchLastSection(false);
    ui->tblGoods->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tblGoods->setColumnWidth(1, 70);
    ui->tblGoods->setColumnWidth(2, 80);
    ui->tblGoods->setColumnWidth(3, 70);
    ui->tblGoods->setColumnWidth(4, 280);
    ui->tblGoods->setColumnWidth(5, 90);
    ui->tblGoods->setRowCount(mSourceLines.size());

    for (int row = 0; row < mSourceLines.size(); ++row) {
        const StoreInputXmlGoodLine &line = mSourceLines.at(row);
        auto *nameItem = new QTableWidgetItem(line.description);
        nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
        ui->tblGoods->setItem(row, 0, nameItem);
        ui->tblGoods->setItem(row, 1, new QTableWidgetItem(QString::number(line.qty, 'f', 4)));
        ui->tblGoods->setItem(row, 2, new QTableWidgetItem(QString::number(line.pricePerUnit, 'f', 2)));
        ui->tblGoods->setItem(row, 3, new QTableWidgetItem(line.unit));

        auto *selector = new C5CodeNameSelector(ui->tblGoods);
        selector->setSelectorName(tr("Goods"));
        selector->selectorCallback = goodsItemSelector;
        selector->setMinimumHeight(28);
        ui->tblGoods->setCellWidget(row, 4, selector);
        mSelectors.append(selector);

        auto *updateCheck = new QCheckBox(ui->tblGoods);
        updateCheck->setText(tr("Rename"));
        auto *checkWrap = new QWidget(ui->tblGoods);
        auto *checkLayout = new QHBoxLayout(checkWrap);
        checkLayout->setContentsMargins(6, 0, 6, 0);
        checkLayout->addWidget(updateCheck);
        ui->tblGoods->setCellWidget(row, 5, checkWrap);
        mUpdateChecks.append(updateCheck);
    }

    resize(980, qMin(680, 140 + mSourceLines.size() * 34));
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DlgStoreInputXmlGoodsMatch::tryAccept);
}

DlgStoreInputXmlGoodsMatch::~DlgStoreInputXmlGoodsMatch()
{
    delete ui;
}

QVector<StoreInputXmlGoodsMappingRow> DlgStoreInputXmlGoodsMatch::rows() const
{
    QVector<StoreInputXmlGoodsMappingRow> result;
    result.reserve(mSourceLines.size());
    for (int row = 0; row < mSourceLines.size(); ++row) {
        StoreInputXmlGoodsMappingRow mapped;
        mapped.source = mSourceLines.at(row);
        mapped.goodsId = mSelectors.at(row)->value();
        mapped.goodsName = mSelectors.at(row)->name();
        mapped.updateName = mUpdateChecks.at(row)->isChecked();
        result.append(mapped);
    }
    return result;
}

void DlgStoreInputXmlGoodsMatch::tryAccept()
{
    for (int row = 0; row < mSelectors.size(); ++row) {
        if (mSelectors.at(row)->value() <= 0) {
            C5Message::error(tr("Match all goods before import. Row: %1").arg(row + 1));
            return;
        }
    }
    accept();
}
