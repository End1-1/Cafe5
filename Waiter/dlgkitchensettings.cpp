#include "dlgkitchensettings.h"
#include "ui_dlgkitchensettings.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QSpinBox>
#include <QTableWidgetItem>
#include <QWidget>

namespace
{

constexpr int kColShow = 0;

constexpr int kColName = 1;

constexpr int kColWidth = 2;

constexpr int kRowHeight = 44;

QWidget *makeCenteredCheckBox(bool checked, QWidget *parent)
{
    auto *holder = new QWidget(parent);
    auto *lay = new QHBoxLayout(holder);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    auto *cb = new QCheckBox(holder);
    cb->setObjectName(QStringLiteral("kitchen_settings_visible"));
    cb->setChecked(checked);
    lay->addStretch(1);
    lay->addWidget(cb);
    lay->addStretch(1);
    return holder;
}

} // namespace

DlgKitchenSettings::DlgKitchenSettings(C5User *user,
                                       const QVector<KitchenColSetting> &cols,
                                       QWidget *parent)
    : C5Dialog(user, parent)
    , ui(new Ui::DlgKitchenSettings)
    , mCols(cols)
{
    ui->setupUi(this);

    ui->tblCols->setColumnWidth(kColShow, 70);
    ui->tblCols->setColumnWidth(kColName, 280);
    ui->tblCols->setColumnWidth(kColWidth, 160);
    ui->tblCols->horizontalHeader()->setSectionResizeMode(kColName, QHeaderView::Stretch);
    ui->tblCols->horizontalHeader()->setSectionResizeMode(kColShow, QHeaderView::Fixed);
    ui->tblCols->horizontalHeader()->setSectionResizeMode(kColWidth, QHeaderView::Fixed);

    populateRows();
}

DlgKitchenSettings::~DlgKitchenSettings()
{
    delete ui;
}

void DlgKitchenSettings::populateRows()
{
    ui->tblCols->setRowCount(mCols.size());

    for(int i = 0; i < mCols.size(); ++i) {
        const KitchenColSetting &c = mCols.at(i);

        ui->tblCols->setRowHeight(i, kRowHeight);

        QWidget *cbHolder = makeCenteredCheckBox(c.visible, ui->tblCols);
        ui->tblCols->setCellWidget(i, kColShow, cbHolder);

        auto *itName = new QTableWidgetItem(c.label);
        itName->setFlags(itName->flags() & ~Qt::ItemIsEditable);
        itName->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->tblCols->setItem(i, kColName, itName);

        auto *sp = new QSpinBox(ui->tblCols);
        sp->setMinimum(c.minWidth);
        sp->setMaximum(c.maxWidth);
        sp->setSingleStep(5);
        sp->setSuffix(QStringLiteral(" px"));
        sp->setValue(qBound(c.minWidth, c.width, c.maxWidth));
        sp->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tblCols->setCellWidget(i, kColWidth, sp);
    }
}

QCheckBox *DlgKitchenSettings::checkBoxAt(int row) const
{
    QWidget *holder = ui->tblCols->cellWidget(row, kColShow);

    if(!holder) {
        return nullptr;
    }

    return holder->findChild<QCheckBox *>(QStringLiteral("kitchen_settings_visible"));
}

QSpinBox *DlgKitchenSettings::spinBoxAt(int row) const
{
    return qobject_cast<QSpinBox *>(ui->tblCols->cellWidget(row, kColWidth));
}

void DlgKitchenSettings::readControlsIntoModel()
{
    for(int i = 0; i < mCols.size(); ++i) {
        if(QCheckBox *cb = checkBoxAt(i)) {
            mCols[i].visible = cb->isChecked();
        }

        if(QSpinBox *sp = spinBoxAt(i)) {
            mCols[i].width = sp->value();
        }
    }
}

void DlgKitchenSettings::on_btnSave_clicked()
{
    readControlsIntoModel();
    accept();
}

void DlgKitchenSettings::on_btnCancel_clicked()
{
    reject();
}

void DlgKitchenSettings::on_btnReset_clicked()
{
    for(int i = 0; i < mCols.size(); ++i) {
        if(QCheckBox *cb = checkBoxAt(i)) {
            cb->setChecked(mCols.at(i).defaultVisible);
        }

        if(QSpinBox *sp = spinBoxAt(i)) {
            sp->setValue(mCols.at(i).defaultWidth);
        }
    }
}
