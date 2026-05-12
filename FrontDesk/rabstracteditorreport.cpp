#include "rabstracteditorreport.h"
#include <QAbstractTableModel>
#include <QApplication>
#include <QColor>
#include <QDesktopServices>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QListWidget>
#include <QPushButton>
#include <QScrollBar>
#include <QSet>
#include <QSortFilterProxyModel>
#include <QUrl>
#include <QVBoxLayout>
#include "c5config.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5salaryeditor.h"
#include "c5storeinput.h"
#include "c5storeinventory.h"
#include "c5storeoutput.h"
#include "c5user.h"
#include "c5utils.h"
#include "dict_workstation.h"
#include "rabstracteditordialog.h"
#include "rfilterdialog.h"
#include "rfilterproxymodel.h"
#include "struct_doc_store_input.h"
#include "struct_waiter_order.h"
#include "ui_rabstracteditorreport.h"
#include "worderinspector.h"
#include <algorithm>
#include <xlsxdocument.h>

class RAbstractEditorTableModel: public QAbstractTableModel
{
public:
    QJsonArray mRows;
    QStringList mHeaders;

    int rowCount(const QModelIndex&) const override
    {
        return mRows.size();
    }

    int columnCount(const QModelIndex&) const override
    {
        return mHeaders.size();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if(!index.isValid() || role != Qt::DisplayRole)
            return {};

        const QJsonArray row = mRows[index.row()].toArray();

        return row.at(index.column()).toVariant();
    }

    QVariant headerData(int section, Qt::Orientation o, int role) const override
    {
        if(role != Qt::DisplayRole)
            return {};

        return (o == Qt::Horizontal) ? QVariant(mHeaders[section]) : QVariant(section + 1);
    }

    void setJson(const QJsonObject &jdoc)
    {
        beginResetModel();
        mRows = jdoc["rows"].toArray();
        mHeaders.clear();

        for(auto h : jdoc["headers"].toArray()) {
            mHeaders.append(h.toString());
        }

        endResetModel();
    }
};

RAbstractEditorReport::RAbstractEditorReport(const QString &title, QIcon icon, const QString &editorName)
    : C5Widget(),
      ui(new Ui::RAbstractEditorReport)
{
    ui->setupUi(this);
    fLabel = title;
    fIcon = icon;
    mEditorName = editorName;
    mModel = new RAbstractEditorTableModel();
    mProxyModel = new RFilterProxyModel();
    mProxyModel->setSourceModel(mModel);
    ui->tbl->setModel(mProxyModel);
    connect(mProxyModel, &RFilterProxyModel::sumsChanged, this, [this](const QHash<int, double>& values) {
        ui->tblTotal->setRowCount(1);
        ui->tblTotal->setColumnCount(mProxyModel->columnCount());
        ui->tblTotal->setVerticalHeaderLabels({ QString::number(mProxyModel->rowCount()) });

        for(auto it = values.constBegin(); it != values.constEnd(); ++it) {
            int col = it.key();
            double v = it.value();
            ui->tblTotal->setString(0, col, float_str(v, 2));
        }
    });
    //connect(ui->tbl->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewHeaderContextMenuRequested(QPoint)));
    connect(ui->tbl->horizontalHeader(), &QHeaderView::sectionClicked, this, [this](int index) {
        mProxyModel->sort(index, mProxyModel->sortOrder() == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder);
    });
    connect(ui->tbl->horizontalHeader(), &QHeaderView::sectionResized, this, [this](int index, int oldSize, int newSize) {
        ui->tblTotal->setColumnWidth(index, newSize);
    });
    {
        QHeaderView *header = ui->tbl->horizontalHeader();
        header->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(header, &QHeaderView::customContextMenuRequested, this, [this](const QPoint &pos) {
            const int col = ui->tbl->horizontalHeader()->logicalIndexAt(pos);
            if(col < 0) {
                return;
            }
            showColumnValueFilterDialog(col);
        });
    }
    // connect(ui->tbl->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(selectionChanged(QItemSelection, QItemSelection)));
    // connect(ui->tbl, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(tableViewContextMenuRequested(QPoint)));
    ui->tbl->horizontalScrollBar()->setSingleStep(1);
    ui->tblTotal->horizontalScrollBar()->setSingleStep(1);
    connect(ui->tblTotal->horizontalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
        ui->tbl->horizontalScrollBar()->setValue(value);
    });
    mFilterValues =  QJsonDocument::fromJson(__c5config.getRegValue("filter_values_" + mEditorName, "").toString().toUtf8()).array();
}

RAbstractEditorReport::~RAbstractEditorReport()
{
    delete ui;
}

QToolBar* RAbstractEditorReport::toolBar()
{
    if(!fToolBar) {
        C5Widget::toolBar();
        auto *action = fToolBar->addAction(QIcon(":/new-file.png"), tr("New"), this, [this]() { newData(); });
        action->setProperty("name", "new");
        action = fToolBar->addAction(QIcon(":/reload.png"), tr("Reload"), this, [this] {
            getData();
        });
        action->setProperty("name", "reload");
        action = fToolBar->addAction(QIcon(":/cancel.png"), tr("Delete"), this, [this] { removeAction(); });
        action->setProperty("name", "delete");
        action = fToolBar->addAction(QIcon(":/filter.png"), tr("Parameters"), this, [this] {
            applyFilter();
        });
        action->setProperty("name", "filter");
        action = fToolBar->addAction(QIcon(":/columns.png"), tr("Columns"), this, [this] { showColumnVisibilityDialog(); });
        action->setProperty("name", "columns");
        fToolBar->addAction(QIcon(":/excel.png"), tr("Export\nto Excel"), this, [this] {
            exportToExcel();
        });
    }

    return fToolBar;
}

void RAbstractEditorReport::showEvent(QShowEvent *e)
{
    C5Widget::showEvent(e);

    if(mFirstLoad) {
        mFirstLoad = false;
        getData();
    }
}

void RAbstractEditorReport::on_tbl_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid()) {
        return;
    }

    const QModelIndex srcIndex = mProxyModel->mapToSource(index);
    if(!srcIndex.isValid()) {
        return;
    }

    RAbstractEditorDialog *dialog = nullptr;

    if(mEditorName == "Workstations") {
        int type = mModel->data(mModel->index(srcIndex.row(), 1), Qt::DisplayRole).toInt();

        switch(type) {
        case WORKSTATION_WAITER:
            dialog = createEditorDialog("Workstations");
            break;

        default:
            Q_ASSERT_X(false, "check editor type", QString("Invalid type of editor %1").arg(type).toLatin1());
            break;
        }
    }

    if (mEditorName == "form_salary") {
        auto obj = filterObject("viewmode");
        if (!obj.isEmpty()) {
            int reportType = obj.value("viewmode").toInt();
            if (reportType < 2) {
                return;
            }

            int type = 0;
            if (reportType == 2) {
                if (index.column() == 1) {
                    type = 1;
                }
                if (index.column() == 2) {
                    type = 2;
                }
            }
            if (reportType == 3) {
                if (index.column() == 2) {
                    type = 1;
                }
                if (index.column() == 3) {
                    type = 2;
                }
            }
            if (type == 0) {
                return;
            }
            auto *sdoc = new C5SalaryEditor();
            QDate date = QDate::fromString(mModel->data(mModel->index(srcIndex.row(), 0), Qt::DisplayRole).toString(), FORMAT_DATE_TO_STR);
            sdoc->open(date, type);
            __mainWindow->addWidget(sdoc);
        }
        return;
    }

    if(mEditorName == "CashSessions") {
        dialog = createEditorDialog("CashSessions");
    }

    if (mEditorName == "form_store_documents" || mEditorName == "form_store_moves") {
        NInterface::query1("/engine/v2/common/store-move/open",
                           mUser->mSessionKey,
                           this,
                           {{"id", mModel->data(mModel->index(srcIndex.row(), 0), Qt::DisplayRole).toString()}},
                           [this](const QJsonObject &jo) {
                               StoreInputDocument sid = JsonParser<StoreInputDocument>::fromJson(jo.value("doc").toObject());
                               switch (sid.type) {
                               case DOC_TYPE_STORE_INPUT: {
                                   auto *sw = new C5StoreInput(mUser, tr("Store input"), QIcon());
                                   __mainWindow->addWidget(sw);
                                   sw->setDocument(sid);
                                   return;
                               }
                               case DOC_TYPE_STORE_OUTPUT: {
                                   auto *sw = new C5StoreOutput(mUser, tr("Store output"), QIcon());
                                   __mainWindow->addWidget(sw);
                                   sw->setDocument(sid);
                                   return;
                               }
                               }
                           });
        return;
    }

    if (mEditorName == "form_cashsessions") {
        QString id = mModel->data(mModel->index(srcIndex.row(), 0), Qt::DisplayRole).toString();
        NInterface::query1("/engine/v2/waiter/order/query-order", mUser->mSessionKey, this, {{"id", id}}, [this](const QJsonObject &jo) {
            WaiterOrder order = JsonParser<WaiterOrder>::fromJson(jo.value("order").toObject());

            auto *sw = new WOrderInspector(mUser, tr("Order"), QIcon());

            __mainWindow->addWidget(sw);
            sw->setOrder(order);
        });
        return;
    }

    if (mEditorName == "form_inventory_documents") {
        NInterface::query1("/engine/v2/common/stock/open-inventory",
                           mUser->mSessionKey,
                           this,
                           {{"id", mModel->data(mModel->index(srcIndex.row(), 0), Qt::DisplayRole).toString()}},
                           [this](const QJsonObject &jo) {
                               StoreInventoryDocument sid = JsonParser<StoreInventoryDocument>::fromJson(jo.value("data").toObject());

                               auto *sw = new C5StoreInventory(mUser, tr("Store input"), QIcon());
                               __mainWindow->addWidget(sw);
                               sw->setDocument(sid);
                           });
        return;
    }

    if (dialog) {
        QJsonValue id = mModel->data(mModel->index(srcIndex.row(), 0), Qt::DisplayRole).toJsonValue();
        dialog->setId(id);

        if(dialog->exec() == QDialog::Accepted) {
        }

        dialog->deleteLater();
    }
}

void RAbstractEditorReport::getData()
{
    NInterface::query1("/engine/v2/officen/editors/get-all", mUser->mSessionKey, this, {
        {"editor", mEditorName},
        {"filter", mFilterValues}
    },
    [this](const QJsonObject & jdoc) {
        mFilterWidget = jdoc["filter"].toArray();
        mProxyModel->columnSums.clear();
        QJsonArray jsums = jdoc["sum"].toArray();

        for(int i = 0; i < jsums.size(); i++) {
            mProxyModel->columnSums[jsums.at(i).toInt()] = 0;
        }

        mProxyModel->numericCols = mProxyModel->columnSums.keys();
        mModel->setJson(jdoc);
        mProxyModel->clearAllColumnValueFilters();
        ui->tbl->resizeColumnsToContents();
        applySavedColumnVisibility();

        QJsonArray colWidths = jdoc.value("col_widths").toArray();

        for (const QJsonValue &jcValue : colWidths) {
            if (jcValue.isObject()) {
                QJsonObject jc = jcValue.toObject();

                int column = jc.value("col").toInt();
                int width = jc.value("width").toInt(100);

                ui->tbl->setColumnWidth(column, width);
            }
        }

        QJsonObject jtoolBar = jdoc["toolbar"].toObject();

        for(auto *a : fToolBar->actions()) {
            if(a->property("name").isValid()) {
                if (a->property("name").toString() == "columns") {
                    continue;
                }
                a->setVisible(jtoolBar[a->property("name").toString()].toBool());
            }
        }

        ui->tblTotal->setColumnCount(mProxyModel->columnCount());

        for(int i = 0; i < mProxyModel->columnCount(); i++) {
            ui->tblTotal->setColumnWidth(i, ui->tbl->columnWidth(i));
        }

        mProxyModel->recalcSums();
        emit mProxyModel->sumsChanged(mProxyModel->columnSums);
    });
}

void RAbstractEditorReport::applySavedColumnVisibility()
{
    const QString key = QStringLiteral("report_columns_visible_%1").arg(mEditorName);
    const QString raw = __c5config.getRegValue(key, "").toString();
    const QJsonObject jvis = QJsonDocument::fromJson(raw.toUtf8()).object();

    for(int i = 0; i < mProxyModel->columnCount(); ++i) {
        const QString ckey = QString::number(i);
        const bool visible = jvis.contains(ckey) ? jvis.value(ckey).toBool() : true;
        ui->tbl->setColumnHidden(i, !visible);
    }
}

void RAbstractEditorReport::showColumnVisibilityDialog()
{
    if(!mProxyModel || mProxyModel->columnCount() == 0) {
        return;
    }

    const QString key = QStringLiteral("report_columns_visible_%1").arg(mEditorName);
    const QString raw = __c5config.getRegValue(key, "").toString();
    QJsonObject jvis = QJsonDocument::fromJson(raw.toUtf8()).object();
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Columns"));
    auto *v = new QVBoxLayout(&dlg);
    auto *list = new QListWidget;
    list->setMinimumHeight(280);

    for(int c = 0; c < mProxyModel->columnCount(); ++c) {
        const QString header = mProxyModel->headerData(c, Qt::Horizontal, Qt::DisplayRole).toString();
        auto *it = new QListWidgetItem(header);
        it->setData(Qt::UserRole, c);
        it->setFlags(it->flags() | Qt::ItemIsUserCheckable);
        const QString ckey = QString::number(c);
        const bool visible = jvis.contains(ckey) ? jvis.value(ckey).toBool() : !ui->tbl->isColumnHidden(c);
        it->setCheckState(visible ? Qt::Checked : Qt::Unchecked);
        list->addItem(it);
    }
    v->addWidget(list);

    auto *btnRow = new QHBoxLayout;
    auto *btnAll = new QPushButton(tr("All"));
    auto *btnNone = new QPushButton(tr("None"));
    auto *btnReset = new QPushButton(tr("Reset"));
    btnRow->addWidget(btnAll);
    btnRow->addWidget(btnNone);
    btnRow->addWidget(btnReset);
    btnRow->addStretch(1);
    v->addLayout(btnRow);

    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    v->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    connect(btnAll, &QPushButton::clicked, list, [list] {
        for(int i = 0; i < list->count(); ++i) {
            list->item(i)->setCheckState(Qt::Checked);
        }
    });
    connect(btnNone, &QPushButton::clicked, list, [list] {
        for(int i = 0; i < list->count(); ++i) {
            list->item(i)->setCheckState(Qt::Unchecked);
        }
    });
    connect(btnReset, &QPushButton::clicked, list, [list] {
        for(int i = 0; i < list->count(); ++i) {
            list->item(i)->setCheckState(Qt::Checked);
        }
    });

    dlg.resize(460, 440);
    if(dlg.exec() != QDialog::Accepted) {
        return;
    }

    jvis = QJsonObject();
    for(int i = 0; i < list->count(); ++i) {
        const QListWidgetItem *it = list->item(i);
        const int c = it->data(Qt::UserRole).toInt();
        const bool visible = it->checkState() == Qt::Checked;
        jvis.insert(QString::number(c), visible);
    }
    __c5config.setRegValue(key, QJsonDocument(jvis).toJson(QJsonDocument::Compact));
    applySavedColumnVisibility();
}

QJsonObject RAbstractEditorReport::filterObject(const QString &name)
{
    for (int i = 0; i < mFilterValues.size(); i++) {
        auto obj = mFilterValues.at(i).toObject();
        if (obj.contains(name)) {
            return obj;
        }
    }
    return {};
}

QModelIndex RAbstractEditorReport::reportMapViewIndexToSource(const QModelIndex &viewIndex) const
{
    if(!viewIndex.isValid() || !mProxyModel) {
        return {};
    }
    return mProxyModel->mapToSource(viewIndex);
}

QVariant RAbstractEditorReport::reportSourceCellData(int sourceRow, int column, int role) const
{
    if(!mModel || sourceRow < 0 || column < 0) {
        return {};
    }
    return mModel->data(mModel->index(sourceRow, column), role);
}

void RAbstractEditorReport::applyFilter()
{
    RFilterDialog df(mUser);
    df.buildWidget(mEditorName, mFilterWidget);

    if(df.exec() == QDialog::Accepted) {
        mFilterValues = df.filterValues();
        getData();
        __c5config.setRegValue("filter_values_" + mEditorName, QJsonDocument(mFilterValues).toJson(QJsonDocument::Compact));
    }
}

void RAbstractEditorReport::removeAction()
{
    QModelIndex index = ui->tbl->currentIndex();
    if (!index.isValid()) {
        return;
    }
    const QModelIndex srcIndex = mProxyModel->mapToSource(index);
    if (!srcIndex.isValid()) {
        return;
    }
    if (C5Message::question(tr("Do you want to delete the selected document?")) != QDialog::Accepted) {
        return;
    }
    if (mEditorName == "form_store_documents" || mEditorName == "form_store_moves") {
        NInterface::query1("/engine/v2/common/store-move/remove",
                           mUser->mSessionKey,
                           this,
                           {{"id", mModel->data(mModel->index(srcIndex.row(), 0), Qt::DisplayRole).toString()}},
                           [this, srcIndex](const QJsonObject &jo) { mModel->removeRow(srcIndex.row()); });
        return;
    }
}

RAbstractEditorDialog* RAbstractEditorReport::createEditorDialog(const QString &editorName)
{
    auto *d = new RAbstractEditorDialog(mUser, editorName);
    return d;
}

void RAbstractEditorReport::on_leFilter_textChanged(const QString &arg1)
{
    mProxyModel->setGlobalFilter(arg1);
}

void RAbstractEditorReport::newData()
{
    if (mEditorName == "form_salary") {
        auto *w = new C5SalaryEditor();
        __mainWindow->addWidget(w);
    }
}

void RAbstractEditorReport::showColumnValueFilterDialog(int col)
{
    if(!mModel || col < 0 || col >= mModel->columnCount(QModelIndex())) {
        return;
    }
    if(ui->tbl->isColumnHidden(col)) {
        return;
    }
    if(mModel->rowCount(QModelIndex()) == 0) {
        C5Message::info(tr("No data to filter."));
        return;
    }
    QSet<QString> uniques;
    for(int r = 0; r < mModel->rowCount(QModelIndex()); ++r) {
        uniques.insert(mModel->data(mModel->index(r, col), Qt::DisplayRole).toString());
    }
    QStringList sorted = uniques.values();
    std::sort(sorted.begin(), sorted.end(), [](const QString &a, const QString &b) {
        return QString::localeAwareCompare(a, b) < 0;
    });
    const bool hadFilter = mProxyModel->hasColumnValueFilter(col);
    const QSet<QString> prevAllowed = mProxyModel->columnAllowedValues(col);
    QDialog dlg(this);
    dlg.setWindowTitle(tr("Filter: %1")
                           .arg(mModel->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString()));
    auto *v = new QVBoxLayout(&dlg);
    auto *list = new QListWidget;
    list->setMinimumHeight(280);
    for(const QString &val : sorted) {
        auto *it = new QListWidgetItem;
        it->setData(Qt::UserRole, val);
        it->setText(val.isEmpty() ? tr("(empty)") : val);
        it->setFlags(it->flags() | Qt::ItemIsUserCheckable);
        if(!hadFilter) {
            it->setCheckState(Qt::Checked);
        } else {
            it->setCheckState(prevAllowed.contains(val) ? Qt::Checked : Qt::Unchecked);
        }
        list->addItem(it);
    }
    v->addWidget(list);
    auto *btnRow = new QHBoxLayout;
    auto *btnAll = new QPushButton(tr("All"));
    auto *btnNone = new QPushButton(tr("None"));
    auto *btnReset = new QPushButton(tr("Reset"));
    btnRow->addWidget(btnAll);
    btnRow->addWidget(btnNone);
    btnRow->addWidget(btnReset);
    btnRow->addStretch(1);
    v->addLayout(btnRow);
    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    v->addWidget(box);
    connect(box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    connect(btnAll, &QPushButton::clicked, list, [list] {
        for(int i = 0; i < list->count(); ++i) {
            list->item(i)->setCheckState(Qt::Checked);
        }
    });
    connect(btnNone, &QPushButton::clicked, list, [list] {
        for(int i = 0; i < list->count(); ++i) {
            list->item(i)->setCheckState(Qt::Unchecked);
        }
    });
    connect(btnReset, &QPushButton::clicked, this, [this, col, list] {
        mProxyModel->clearColumnValueFilter(col);
        for(int i = 0; i < list->count(); ++i) {
            list->item(i)->setCheckState(Qt::Checked);
        }
    });
    dlg.resize(500, 420);
    if(dlg.exec() != QDialog::Accepted) {
        return;
    }
    QSet<QString> chosen;
    for(int i = 0; i < list->count(); ++i) {
        if(list->item(i)->checkState() == Qt::Checked) {
            chosen.insert(list->item(i)->data(Qt::UserRole).toString());
        }
    }
    if(chosen.isEmpty()) {
        mProxyModel->setColumnValueAllowList(col, QSet<QString>{ });
        return;
    }
    if(chosen.size() == uniques.size()) {
        mProxyModel->clearColumnValueFilter(col);
        return;
    }
    mProxyModel->setColumnValueAllowList(col, chosen);
}

void RAbstractEditorReport::exportToExcel()
{
    if(!mProxyModel || mProxyModel->columnCount() == 0) {
        C5Message::info(tr("Empty report!"));
        return;
    }

    QList<int> cols;

    for(int c = 0; c < mProxyModel->columnCount(); c++) {
        if(!ui->tbl->isColumnHidden(c)) {
            cols.append(c);
        }
    }

    if(cols.isEmpty()) {
        C5Message::info(tr("Empty report!"));
        return;
    }

    if(mProxyModel->rowCount() == 0) {
        C5Message::info(tr("Empty report!"));
        return;
    }

    QXlsx::Document d;
    d.addSheet("Sheet1");
    QXlsx::Format hf;
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    hf.setFont(headerFont);
    hf.setBorderStyle(QXlsx::Format::BorderThin);
    hf.setPatternBackgroundColor(QColor(200, 200, 250));
    int excelCol = 1;

    for(int c : cols) {
        d.write(1, excelCol, mProxyModel->headerData(c, Qt::Horizontal, Qt::DisplayRole), hf);
        d.setColumnWidth(excelCol, qMax(8, ui->tbl->columnWidth(c) / 7));
        excelCol++;
    }

    QXlsx::Format bf;
    bf.setBorderStyle(QXlsx::Format::BorderThin);
    bf.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    QXlsx::Format bfn;
    bfn.setBorderStyle(QXlsx::Format::BorderThin);
    bfn.setHorizontalAlignment(QXlsx::Format::AlignRight);
    bfn.setNumberFormat("0.00");
    int currentExcelRow = 2;

    for(int r = 0; r < mProxyModel->rowCount(); r++) {
        excelCol = 1;

        for(int c : cols) {
            const QString textVal = mProxyModel->data(mProxyModel->index(r, c), Qt::DisplayRole).toString();

            if(mProxyModel->numericCols.contains(c)) {
                d.write(currentExcelRow, excelCol, str_money_mysql_format(textVal), bfn);
            } else {
                d.write(currentExcelRow, excelCol, textVal, bf);
            }

            excelCol++;
        }

        currentExcelRow++;
    }

    if(ui->tblTotal->isVisible() && ui->tblTotal->rowCount() > 0) {
        QXlsx::Format tfNum = hf;
        tfNum.setHorizontalAlignment(QXlsx::Format::AlignRight);
        tfNum.setNumberFormat("0.00");
        excelCol = 1;

        for(int c : cols) {
            if(mProxyModel->columnSums.contains(c)) {
                d.write(currentExcelRow, excelCol, mProxyModel->columnSums.value(c), tfNum);
            } else {
                QString cell = ui->tblTotal->getString(0, c);
                d.write(currentExcelRow, excelCol, cell, hf);
            }

            excelCol++;
        }
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Excel"), QString(), tr("Excel (*.xlsx)"));

    if(filename.isEmpty()) {
        return;
    }

    if(!filename.endsWith(".xlsx", Qt::CaseInsensitive)) {
        filename += ".xlsx";
    }

    if(!d.saveAs(filename)) {
        C5Message::error(tr("Could not save file"));
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
}
