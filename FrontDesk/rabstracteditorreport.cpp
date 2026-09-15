#include "rabstracteditorreport.h"
#include <QAbstractButton>
#include <QAbstractTableModel>
#include <QApplication>
#include <QButtonGroup>
#include <QColor>
#include <QDateTime>
#include <QDesktopServices>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFileDialog>
#include <QFont>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QListWidget>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QSet>
#include <QSizePolicy>
#include <QSortFilterProxyModel>
#include <QTimer>
#include <QToolButton>
#include <QUrl>
#include <QVBoxLayout>
#include <QtGlobal>
#include "c5config.h"
#include "c5editor.h"
#include "c5mainwindow.h"
#include "c5message.h"
#include "c5salaryeditor.h"
#include "c5salarypaymenteditor.h"
#include "c5storeinput.h"
#include "c5storeinventory.h"
#include "c5storeoutput.h"
#include "dlginventoryblankeditor.h"
#include "c5storemovement.h"
#include "c5storecomplectation.h"
#include "c5user.h"
#include "c5utils.h"
#include "ce5goods.h"
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

RAbstractEditorReport::RAbstractEditorReport(const QString &title, QIcon icon, const QString &editorName, QWidget *parent)
    : C5Widget(parent),
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

        for(int c = 0; c < ui->tblTotal->columnCount(); ++c) {
            ui->tblTotal->setString(0, c, QString());
        }

        for(auto it = values.constBegin(); it != values.constEnd(); ++it) {
            int col = it.key();
            double v = it.value();
            ui->tblTotal->setString(0, col, float_str(v, 2));
        }

        for(auto it = mFooterValues.constBegin(); it != mFooterValues.constEnd(); ++it) {
            bool ok = false;
            const int col = it.key().toInt(&ok);
            if(!ok || col < 0 || col >= ui->tblTotal->columnCount()) {
                continue;
            }
            ui->tblTotal->setString(0, col, it.value().toString());
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
    mViewModeGroup = new QButtonGroup(this);
    mViewModeGroup->setExclusive(true);
    connect(mViewModeGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked), this, [this](QAbstractButton *btn) {
        if(!btn) {
            return;
        }
        const int value = btn->property("value").toInt();
        const QJsonObject current = filterObject(mViewModeFilterName);
        if(!current.isEmpty() && current.value(mViewModeFilterName).toInt() == value) {
            return;
        }
        setViewModeFilterValue(value);
        __c5config.setRegValue("filter_values_" + mEditorName, QJsonDocument(mFilterValues).toJson(QJsonDocument::Compact));
        getData();
    });
    connect(ui->btnViewModeLeft, &QToolButton::clicked, this, [this] { scrollViewModeBy(-120); });
    connect(ui->btnViewModeRight, &QToolButton::clicked, this, [this] { scrollViewModeBy(120); });
    connect(ui->scrollViewMode->horizontalScrollBar(), &QScrollBar::rangeChanged, this, [this](int, int) {
        syncViewModeScrollRange();
    });
    connect(ui->scrollViewMode->horizontalScrollBar(), &QScrollBar::valueChanged, this, [this](int) {
        syncViewModeScrollRange();
    });
    ui->wViewMode->installEventFilter(this);
    ui->scrollViewMode->installEventFilter(this);
    ui->scrollViewMode->viewport()->installEventFilter(this);
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
        if(mEditorName == QLatin1String("form_stock")) {
            fToolBar->addAction(QIcon(QStringLiteral(":/print.png")), tr("Print blank"), this, [this] {
                printStockInventoryBlank();
            });
        }
    }

    return fToolBar;
}

void RAbstractEditorReport::reloadReport()
{
    mFirstLoad = false;
    getData();
}

void RAbstractEditorReport::setDeferredLoad(bool deferred)
{
    mDeferredLoad = deferred;
}

void RAbstractEditorReport::showEvent(QShowEvent *e)
{
    C5Widget::showEvent(e);

    if(mFirstLoad && !mDeferredLoad) {
        mFirstLoad = false;
        getData();
    }
}

void RAbstractEditorReport::resizeEvent(QResizeEvent *e)
{
    C5Widget::resizeEvent(e);
    updateViewModeScrollButtons();
}

bool RAbstractEditorReport::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Resize
        && (watched == ui->wViewMode
            || watched == ui->scrollViewMode
            || watched == ui->scrollViewMode->viewport())) {
        updateViewModeScrollButtons();
    }
    return C5Widget::eventFilter(watched, event);
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
            QDate date = QDate::fromString(mModel->data(mModel->index(srcIndex.row(), 0), Qt::DisplayRole).toString(), FORMAT_DATE_TO_STR);
            if (type == 1) {
                auto *sdoc = new C5SalaryEditor();
                sdoc->open(date);
                __mainWindow->addWidget(sdoc);
            } else if (type == 2) {
                auto *pdoc = new C5SalaryPaymentEditor();
                pdoc->open(date);
                __mainWindow->addWidget(pdoc);
            }
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
                               const QJsonObject docJo = jo.value("doc").toObject();
                               StoreInputDocument sid = JsonParser<StoreInputDocument>::fromJson(docJo);
                               switch (sid.type) {
                               case DOC_TYPE_STORE_MOVE: {
                                   auto *sw = new C5StoreMovement(mUser, tr("Store movement"), QIcon());
                                   __mainWindow->addWidget(sw);
                                   sw->setDocument(sid);
                                   return;
                               }
                               case DOC_TYPE_STORE_COMPLECTATION: {
                                   auto *sw = new C5StoreComplectation(mUser, tr("Store complectation"), QIcon());
                                   __mainWindow->addWidget(sw);
                                   sw->setDocument(docJo);
                                   return;
                               }
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
                               default:
                                   C5Message::error(tr("Unsupported document type"));
                                   return;
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

    if (mEditorName == "form_stock") {
        const int goodsId = mModel->data(mModel->index(srcIndex.row(), 0), Qt::DisplayRole).toInt();
        if (goodsId <= 0) {
            return;
        }
        CE5Goods *ep = new CE5Goods();
        C5Editor *e = C5Editor::createEditor(mUser, ep, goodsId);
        QList<QMap<QString, QVariant>> data;
        e->getResult(data);
        delete e;
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
        mProxyModel->columnLast.clear();
        mFooterValues = QJsonObject();
        QJsonArray jsums = jdoc["sum"].toArray();

        for(int i = 0; i < jsums.size(); i++) {
            mProxyModel->columnSums[jsums.at(i).toInt()] = 0;
        }

        QJsonArray jsumLast = jdoc.value(QStringLiteral("sum_last")).toArray();
        for(int i = 0; i < jsumLast.size(); i++) {
            const int col = jsumLast.at(i).toInt();
            mProxyModel->columnLast.append(col);
            mProxyModel->columnSums[col] = 0;
        }

        mFooterValues = jdoc.value(QStringLiteral("footer_values")).toObject();

        mProxyModel->numericCols = mProxyModel->columnSums.keys();
        mModel->setJson(jdoc);
        mProxyModel->clearAllColumnValueFilters();
        ui->tbl->resizeColumnsToContents();
        mReportDefaultHiddenColumns.clear();
        for(const QJsonValue &hv : jdoc.value(QStringLiteral("hidden_columns")).toArray()) {
            mReportDefaultHiddenColumns.insert(hv.toInt());
        }
        applyColumnVisibility();

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
        setupViewModeBar();
    });
}

void RAbstractEditorReport::applyColumnVisibility()
{
    const QString key = QStringLiteral("report_columns_visible_%1").arg(mEditorName);
    const QString raw = __c5config.getRegValue(key, "").toString();
    const QJsonObject jvis = QJsonDocument::fromJson(raw.toUtf8()).object();
    const bool registrySet = !jvis.isEmpty();

    for(int i = 0; i < mProxyModel->columnCount(); ++i) {
        const QString ckey = QString::number(i);
        bool visible;
        if(registrySet) {
            visible = jvis.contains(ckey) ? jvis.value(ckey).toBool() : true;
        } else {
            visible = !mReportDefaultHiddenColumns.contains(i);
        }
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
    const bool registrySet = !jvis.isEmpty();
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
        const bool visible = registrySet
            ? (jvis.contains(ckey) ? jvis.value(ckey).toBool() : true)
            : !mReportDefaultHiddenColumns.contains(c);
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
    applyColumnVisibility();
}

QJsonObject RAbstractEditorReport::filterObject(const QString &name) const
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

int RAbstractEditorReport::reportColumnFromEnd(int offsetFromEnd) const
{
    if(!mModel || offsetFromEnd >= 0) {
        return -1;
    }
    const int colCount = mModel->columnCount({});
    return colCount + offsetFromEnd;
}

void RAbstractEditorReport::applyFilter()
{
    RFilterDialog df(mUser);
    df.buildWidget(mEditorName, mFilterWidget);

    if(df.exec() == QDialog::Accepted) {
        const QJsonObject preservedViewMode = mViewModeFilterName.isEmpty()
            ? QJsonObject()
            : filterObject(mViewModeFilterName);
        mFilterValues = df.filterValues();
        if(!preservedViewMode.isEmpty()) {
            mFilterValues.append(preservedViewMode);
        }
        getData();
        __c5config.setRegValue("filter_values_" + mEditorName, QJsonDocument(mFilterValues).toJson(QJsonDocument::Compact));
    }
}

void RAbstractEditorReport::clearViewModeButtons()
{
    if(mViewModeGroup) {
        const QList<QAbstractButton *> buttons = mViewModeGroup->buttons();
        for(QAbstractButton *btn : buttons) {
            mViewModeGroup->removeButton(btn);
        }
    }
    auto *layout = ui->horizontalLayoutViewModeButtons;
    while(QLayoutItem *item = layout->takeAt(0)) {
        if(QWidget *w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }
}

void RAbstractEditorReport::setViewModeFilterValue(int value)
{
    if(mViewModeFilterName.isEmpty()) {
        return;
    }
    for(int i = 0; i < mFilterValues.size(); ++i) {
        QJsonObject obj = mFilterValues.at(i).toObject();
        if(obj.contains(mViewModeFilterName)) {
            obj.insert(mViewModeFilterName, value);
            mFilterValues.replace(i, obj);
            return;
        }
    }
    QJsonObject obj;
    obj.insert(mViewModeFilterName, value);
    mFilterValues.append(obj);
}

void RAbstractEditorReport::setupViewModeBar()
{
    QJsonObject viewModeConfig;
    for(int i = 0; i < mFilterWidget.size(); ++i) {
        const QJsonObject jo = mFilterWidget.at(i).toObject();
        if(jo.value(QStringLiteral("type")).toString() == QStringLiteral("viewmode")) {
            viewModeConfig = jo;
            break;
        }
    }

    if(viewModeConfig.isEmpty()) {
        mViewModeFilterName.clear();
        clearViewModeButtons();
        ui->wViewMode->hide();
        return;
    }

    mViewModeFilterName = viewModeConfig.value(QStringLiteral("name")).toString();
    const QJsonArray values = viewModeConfig.value(QStringLiteral("values")).toArray();
    if(mViewModeFilterName.isEmpty() || values.isEmpty()) {
        mViewModeFilterName.clear();
        clearViewModeButtons();
        ui->wViewMode->hide();
        return;
    }
    const int defaultValue = viewModeConfig.value(QStringLiteral("default")).toInt(1);
    const QJsonObject currentObj = filterObject(mViewModeFilterName);
    int currentValue = currentObj.isEmpty() ? defaultValue : currentObj.value(mViewModeFilterName).toInt(defaultValue);
    if(currentObj.isEmpty()) {
        setViewModeFilterValue(currentValue);
        __c5config.setRegValue("filter_values_" + mEditorName, QJsonDocument(mFilterValues).toJson(QJsonDocument::Compact));
    }

    clearViewModeButtons();
    auto *layout = ui->horizontalLayoutViewModeButtons;
    layout->setSizeConstraint(QLayout::SetFixedSize);
    static const char *kViewModeBtnStyle =
        "QToolButton {"
        "  border: 1px solid #c5cdd8;"
        "  background-color: #f7f9fc;"
        "  color: #1e2a3a;"
        "  padding: 2px 10px;"
        "  margin: 0px;"
        "}"
        "QToolButton:hover:!checked {"
        "  background-color: rgba(99, 132, 232, 0.12);"
        "  border: 1px solid rgba(99, 132, 232, 0.45);"
        "}"
        "QToolButton:checked {"
        "  background-color: #6384e8;"
        "  color: #ffffff;"
        "  border: 1px solid #4a6ad4;"
        "}";
    for(const QJsonValue &jv : values) {
        const QJsonObject item = jv.toObject();
        auto *btn = new QToolButton(ui->wViewModeButtons);
        const QString label = item.value(QStringLiteral("label")).toString();
        btn->setText(label);
        btn->setCheckable(true);
        btn->setAutoRaise(false);
        btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
        btn->setProperty("value", item.value(QStringLiteral("value")).toInt());
        btn->setFixedHeight(28);
        btn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        btn->setStyleSheet(QLatin1String(kViewModeBtnStyle));
        // Width for bold text so checked state never clips (stylesheet may still bold via theme).
        QFont f = btn->font();
        f.setBold(true);
        const int textW = QFontMetrics(f).horizontalAdvance(label);
        btn->setMinimumWidth(textW + 24);
        layout->addWidget(btn);
        mViewModeGroup->addButton(btn);
        if(btn->property("value").toInt() == currentValue) {
            btn->setChecked(true);
        }
    }
    ui->wViewMode->show();
    ui->wViewMode->setFixedHeight(28);
    ui->scrollViewMode->setFixedHeight(28);
    ui->wViewModeButtons->setFixedHeight(28);
    ui->btnViewModeLeft->setFixedSize(28, 28);
    ui->btnViewModeRight->setFixedSize(28, 28);
    ui->horizontalLayoutViewMode->setAlignment(ui->btnViewModeLeft, Qt::AlignVCenter);
    ui->horizontalLayoutViewMode->setAlignment(ui->scrollViewMode, Qt::AlignVCenter);
    ui->horizontalLayoutViewMode->setAlignment(ui->btnViewModeRight, Qt::AlignVCenter);
    ui->wViewModeButtons->adjustSize();
    updateViewModeScrollButtons();
    QTimer::singleShot(0, this, [this] { updateViewModeScrollButtons(); });
}

void RAbstractEditorReport::updateViewModeScrollButtons()
{
    if(mUpdatingViewModeScroll || !ui->wViewMode->isVisible()) {
        return;
    }
    mUpdatingViewModeScroll = true;
    // Keep scroll buttons always in the layout so restore/resize cannot
    // lose the right button to a chicken-and-egg visibility/viewport race.
    ui->btnViewModeLeft->show();
    ui->btnViewModeRight->show();
    syncViewModeScrollRange();
    mUpdatingViewModeScroll = false;
}

void RAbstractEditorReport::syncViewModeScrollRange()
{
    if(!ui->wViewMode->isVisible()) {
        return;
    }

    const int contentW = qMax(ui->wViewModeButtons->width(), ui->wViewModeButtons->sizeHint().width());
    const int viewportW = qMax(1, ui->scrollViewMode->viewport()->width());
    const int maxPos = qMax(0, contentW - viewportW);

    QScrollBar *bar = ui->scrollViewMode->horizontalScrollBar();
    // Force range: after restore Qt often keeps maximum=0 until user interacts.
    bar->setRange(0, maxPos);
    bar->setPageStep(viewportW);
    bar->setSingleStep(40);

    const bool canScroll = maxPos > 0;
    ui->btnViewModeLeft->setEnabled(canScroll && bar->value() > 0);
    ui->btnViewModeRight->setEnabled(canScroll && bar->value() < maxPos);
}

void RAbstractEditorReport::scrollViewModeBy(int delta)
{
    syncViewModeScrollRange();
    QScrollBar *bar = ui->scrollViewMode->horizontalScrollBar();
    bar->setValue(bar->value() + delta);
    syncViewModeScrollRange();
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
        const int choice = C5Message::question(tr("Select document type"),
                                               tr("Accrual"),
                                               tr("Cancel"),
                                               tr("Payment"));
        if (choice == QDialog::Accepted) {
            auto *sdoc = new C5SalaryEditor();
            sdoc->open(QDate::currentDate());
            __mainWindow->addWidget(sdoc);
        } else if (choice == 2) {
            auto *pdoc = new C5SalaryPaymentEditor();
            pdoc->open(QDate::currentDate());
            __mainWindow->addWidget(pdoc);
        }
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

void RAbstractEditorReport::printStockInventoryBlank()
{
    if(mEditorName != QLatin1String("form_stock") || !mProxyModel) {
        return;
    }
    if(mProxyModel->rowCount() == 0) {
        C5Message::info(tr("Empty report!"));
        return;
    }

    // Stock report columns: Id, Storage, Group, Name, Barcode, Qty, Unit, ...
    constexpr int colId = 0;
    constexpr int colStore = 1;
    constexpr int colGroup = 2;
    constexpr int colName = 3;
    constexpr int colSku = 4;
    constexpr int colQty = 5;
    constexpr int colUnit = 6;

    QMap<QString, QJsonArray> itemsByStore;
    for(int r = 0; r < mProxyModel->rowCount(); ++r) {
        const QString name = mProxyModel->index(r, colName).data(Qt::DisplayRole).toString().trimmed();
        if(name.isEmpty()) {
            continue;
        }
        const QString storeName = mProxyModel->index(r, colStore).data(Qt::DisplayRole).toString().trimmed();
        const int goodsId = mProxyModel->index(r, colId).data(Qt::DisplayRole).toInt();
        QJsonObject item;
        item.insert(QStringLiteral("goods_id"), goodsId);
        item.insert(QStringLiteral("code"), goodsId > 0 ? QString::number(goodsId) : QString());
        item.insert(QStringLiteral("name"), name);
        item.insert(QStringLiteral("unit"), mProxyModel->index(r, colUnit).data(Qt::DisplayRole).toString());
        item.insert(QStringLiteral("sku"), mProxyModel->index(r, colSku).data(Qt::DisplayRole).toString());
        item.insert(QStringLiteral("group_name"), mProxyModel->index(r, colGroup).data(Qt::DisplayRole).toString());
        const QString qty = mProxyModel->index(r, colQty).data(Qt::DisplayRole).toString();
        item.insert(QStringLiteral("qty"), qty);
        item.insert(QStringLiteral("qty_sys"), qty);
        itemsByStore[storeName.isEmpty() ? tr("Storage") : storeName].append(item);
    }

    if(itemsByStore.isEmpty()) {
        C5Message::info(tr("Empty report!"));
        return;
    }

    QJsonArray stores;
    for(auto it = itemsByStore.constBegin(); it != itemsByStore.constEnd(); ++it) {
        QJsonObject store;
        store.insert(QStringLiteral("store_id"), 0);
        store.insert(QStringLiteral("store_name"), it.key());
        store.insert(QStringLiteral("items"), it.value());
        stores.append(store);
    }

    QString title = tr("Inventory blank");
    if(itemsByStore.size() == 1) {
        title += QStringLiteral(" — ") + itemsByStore.constBegin().key();
    }

    QJsonObject blank;
    blank.insert(QStringLiteral("title"), title);
    blank.insert(QStringLiteral("created"),
                 QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss")));
    blank.insert(QStringLiteral("stores"), stores);

    DlgInventoryBlankEditor::printBlank(blank, this);
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
