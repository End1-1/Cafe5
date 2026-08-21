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
    ui->tblGoods->setColumnCount(7);
    ui->tblGoods->setHorizontalHeaderLabels({tr("Import name"),
                                             tr("Qty"),
                                             tr("Price"),
                                             tr("Unit"),
                                             tr("Goods in database"),
                                             tr("Create"),
                                             tr("Update name")});
    ui->tblGoods->horizontalHeader()->setStretchLastSection(false);
    ui->tblGoods->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tblGoods->setColumnWidth(1, 70);
    ui->tblGoods->setColumnWidth(2, 80);
    ui->tblGoods->setColumnWidth(3, 70);
    ui->tblGoods->setColumnWidth(4, 260);
    ui->tblGoods->setColumnWidth(5, 80);
    ui->tblGoods->setColumnWidth(6, 90);
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

        auto *createCheck = new QCheckBox(ui->tblGoods);
        createCheck->setText(tr("Create"));
        createCheck->setChecked(true);
        auto *createWrap = new QWidget(ui->tblGoods);
        auto *createLayout = new QHBoxLayout(createWrap);
        createLayout->setContentsMargins(6, 0, 6, 0);
        createLayout->addWidget(createCheck);
        ui->tblGoods->setCellWidget(row, 5, createWrap);
        mCreateChecks.append(createCheck);

        auto *updateCheck = new QCheckBox(ui->tblGoods);
        updateCheck->setText(tr("Rename"));
        updateCheck->setEnabled(false);
        auto *checkWrap = new QWidget(ui->tblGoods);
        auto *checkLayout = new QHBoxLayout(checkWrap);
        checkLayout->setContentsMargins(6, 0, 6, 0);
        checkLayout->addWidget(updateCheck);
        ui->tblGoods->setCellWidget(row, 6, checkWrap);
        mUpdateChecks.append(updateCheck);

        connect(createCheck, &QCheckBox::toggled, this, [selector, updateCheck](bool checked) {
            if (checked) {
                selector->setCodeAndName(0, QString());
                updateCheck->setChecked(false);
                updateCheck->setEnabled(false);
            } else {
                updateCheck->setEnabled(true);
            }
        });
    }

    resize(1080, qMin(680, 140 + mSourceLines.size() * 34));
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
        mapped.createNew = mCreateChecks.at(row)->isChecked();
        mapped.goodsId = mapped.createNew ? 0 : mSelectors.at(row)->value();
        mapped.goodsName = mapped.createNew ? mapped.source.description : mSelectors.at(row)->name();
        mapped.updateName = !mapped.createNew && mUpdateChecks.at(row)->isChecked();
        result.append(mapped);
    }
    return result;
}

void DlgStoreInputXmlGoodsMatch::tryAccept()
{
    for (int row = 0; row < mSelectors.size(); ++row) {
        const bool createNew = mCreateChecks.at(row)->isChecked();
        if (!createNew && mSelectors.at(row)->value() <= 0) {
            C5Message::error(tr("Match or create all goods before import. Row: %1").arg(row + 1));
            return;
        }
    }
    accept();
}
