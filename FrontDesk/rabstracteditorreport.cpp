#include "rabstracteditorreport.h"
#include <QAbstractTableModel>
#include <QApplication>
#include <QColor>
#include <QDesktopServices>
#include <QDialog>
#include <QFileDialog>
#include <QFont>
#include <QJsonArray>
#include <QJsonDocument>
#include <QScrollBar>
#include <QSortFilterProxyModel>
#include <QUrl>
#include <xlsxdocument.h>
#include "c5config.h"
#include "c5mainwindow.h"
#include "c5message.h"
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
#include "ui_rabstracteditorreport.h"

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
        auto *action = fToolBar->addAction(QIcon(":/new-file.png"), tr("New"), this, []() {
        });
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

    RAbstractEditorDialog *dialog = nullptr;

    if(mEditorName == "Workstations") {
        int type = mModel->data(mModel->index(index.row(), 1), Qt::DisplayRole).toInt();

        switch(type) {
        case WORKSTATION_WAITER:
            dialog = createEditorDialog("Workstations");
            break;

        default:
            Q_ASSERT_X(false, "check editor type", QString("Invalid type of editor %1").arg(type).toLatin1());
            break;
        }
    }

    if(mEditorName == "CashSessions") {
        dialog = createEditorDialog("CashSessions");
    }

    if (mEditorName == "form_store_documents" || mEditorName == "form_store_moves") {
        NInterface::query1("/engine/v2/common/store-move/open",
                           mUser->mSessionKey,
                           this,
                           {{"id", mModel->data(mModel->index(index.row(), 0), Qt::DisplayRole).toString()}},
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

    if (mEditorName == "form_inventory_documents") {
        NInterface::query1("/engine/v2/common/stock/open-inventory",
                           mUser->mSessionKey,
                           this,
                           {{"id", mModel->data(mModel->index(index.row(), 0), Qt::DisplayRole).toString()}},
                           [this](const QJsonObject &jo) {
                               StoreInventoryDocument sid = JsonParser<StoreInventoryDocument>::fromJson(jo.value("data").toObject());

                               auto *sw = new C5StoreInventory(mUser, tr("Store input"), QIcon());
                               __mainWindow->addWidget(sw);
                               sw->setDocument(sid);
                           });
        return;
    }

    if (dialog) {
        QJsonValue id = mModel->data(mModel->index(index.row(), 0), Qt::DisplayRole).toJsonValue();
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
        ui->tbl->resizeColumnsToContents();

        for (int i = 0; i < mProxyModel->columnCount(); i++) {
            ui->tbl->setColumnHidden(i, false);
        }
        for(auto column : jdoc["hidden_columns"].toArray()) {
            ui->tbl->setColumnHidden(column.toInt(), true);
        }

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
    if (C5Message::question(tr("Do you want to delete the selected document?")) != QDialog::Accepted) {
        return;
    }
    if (mEditorName == "form_store_documents" || mEditorName == "form_store_moves") {
        NInterface::query1("/engine/v2/common/store-move/remove",
                           mUser->mSessionKey,
                           this,
                           {{"id", mModel->data(mModel->index(index.row(), 0), Qt::DisplayRole).toString()}},
                           [this, index](const QJsonObject &jo) { mModel->removeRow(index.row()); });
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
                d.write(currentExcelRow, excelCol, str_float(textVal), bfn);
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
