#include "rabstracteditorreport.h"
#include "ui_rabstracteditorreport.h"
#include "c5user.h"
#include "dict_workstation.h"
#include "rabstracteditordialog.h"
#include "rfilterdialog.h"
#include "rfilterproxymodel.h"
#include "c5config.h"
#include "c5utils.h"
#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QDialog>
#include <QScrollBar>
#include <QJsonDocument>
#include <QJsonArray>

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
        action = fToolBar->addAction(QIcon(":/cancel.png"), tr("Delete"), this, [] {

        });
        action->setProperty("name", "delete");
        action = fToolBar->addAction(QIcon(":/filter.png"), tr("Parameters"), this, [this] {
            applyFilter();
        });
        action->setProperty("name", "filter");
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

    if(dialog) {
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

        for(auto column : jdoc["hidden_columns"].toArray()) {
            ui->tbl->setColumnHidden(column.toInt(), true);
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

RAbstractEditorDialog* RAbstractEditorReport::createEditorDialog(const QString &editorName)
{
    auto *d = new RAbstractEditorDialog(mUser, editorName);
    return d;
}

void RAbstractEditorReport::on_leFilter_textChanged(const QString &arg1)
{
    mProxyModel->setGlobalFilter(arg1);
}
