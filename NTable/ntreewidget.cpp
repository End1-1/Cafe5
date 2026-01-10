#include "ntreewidget.h"
#include "ui_ntreewidget.h"
#include "ntreemodel.h"
#include "ndataprovider.h"
#include "nloadingdlg.h"
#include "c5mainwindow.h"
#include "logwriter.h"
#include "c5filtervalues.h"
#include "c5utils.h"
#include "nfilterdlg.h"
#include "nhandler.h"
#include "c5message.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScrollBar>
#include <QHeaderView>
#include <QMenu>
#include <QFileDialog>
#include <QDesktopServices>
#include <QXlsx/header/xlsxdocument.h>

#define xls_page_size_a4 9
#define xls_page_orientation_landscape "landscape"
#define xls_page_orientation_portrait "portrait"

QString NTreeWidget::mHost;

NTreeWidget::NTreeWidget(C5User *user, const QString &route, QWidget *parent) :
    C5Widget(parent),
    ui(new Ui::NTreeWidget),
    mRoute(route),
    fFilter(nullptr),
    mUser(user),
    mToolWidget(nullptr)
{
    ui->setupUi(this);
    ui->mTreeView->setModel(new NTreeModel());
    ui->mTreeView->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->mTreeView->header(), &QHeaderView::customContextMenuRequested, this,
            &NTreeWidget::tableViewHeaderContextMenuRequested);
    connect(ui->mTreeView->header(), &QHeaderView::sectionClicked, this, &NTreeWidget::tableViewHeaderClicked);
    connect(ui->mTreeView->header(), &QHeaderView::sectionResized, this,
            &NTreeWidget::tableViewHeaderResized);
    connect(ui->mTreeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this,
            SLOT(selectionChanged(QItemSelection, QItemSelection)));
    connect(ui->mTreeView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(tblValueChanged(int)));
    connect(ui->tblTotal->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(tblValueChanged2(int)));
    ui->widget->setVisible(false);
    mHandler = new NHandler(mUser, this);
    ui->mTreeView->setRootIsDecorated(true);
    ui->mTreeView->setIndentation(20);
    ui->mTreeView->setStyleSheet(
        "QTreeView::item {"
        "border-right: 1px solid #ccc;"
        "border-bottom: 1px solid #ccc;"
        "color: black;"
        "}"
        "QTreeView::item:selected {"
        "background: #c8defc;"
        "color: black;"
        "}"
    );
}

NTreeWidget::~NTreeWidget()
{
    delete ui;
}

void NTreeWidget::query()
{
    auto *nd = new NDataProvider(this);
    connect(nd, &NDataProvider::started, this, &NTreeWidget::queryStarted);
    connect(nd, &NDataProvider::error, this, &NTreeWidget::queryError);
    connect(nd, &NDataProvider::done, this, &NTreeWidget::queryFinished);
    QJsonObject jf = fFilter ? fFilter->filter() : QJsonObject();

    if(!fInitParams.isEmpty()) {
        for(const QString &k : fInitParams.keys()) {
            jf[k] = fInitParams[k];
        }
    }

    nd->changeTimeout(180000);
    nd->getData(mRoute, jf);
}

QToolBar* NTreeWidget::toolBar()
{
    if(!fToolBar) {
        QList<ToolBarButtons> btn;
        btn
                << ToolBarButtons::tbFilter
                << ToolBarButtons::tbRefresh
                << ToolBarButtons::tbClearFilter
                << ToolBarButtons::tbExcel
                << ToolBarButtons::tbPrint;
        createStandartToolbar(btn);
    }

    return fToolBar;
}

void NTreeWidget::hotKey(const QString &key)
{
    if(key.toLower() == "ctrl+f") {
        ui->widget->setVisible(true);
        ui->leFilterLineedit->setFocus();
        ui->leFilterLineedit->setSelection(0, ui->leFilterLineedit->text().length());
    } else if(key.toLower() == "esc") {
        ui->widget->setVisible(false);
        ui->leFilterLineedit->clear();
        static_cast<NTreeModel*>(ui->mTreeView->model())->setFilter("");
    } else {
        C5Widget::hotKey(key);
    }
}

void NTreeWidget::initParams(const QJsonObject &o)
{
    fInitParams = o;
}

void NTreeWidget::sum()
{
    auto *model = static_cast<NTreeModel*>(ui->mTreeView->model());
    ui->tblTotal->setColumnCount(model->columnCount());
    ui->tblTotal->setVisible(model->fColSum.count() > 0);
    QStringList l;
    l.append(QString::number(model->rowCount()));
    ui->tblTotal->setVerticalHeaderLabels(l);

    for(int i = 0; i < ui->tblTotal->columnCount(); i++) {
        if(model->fColSum.contains(i)) {
            ui->tblTotal->setItem(0, i, new QTableWidgetItem(float_str(model->fColSum[i], 2)));
        } else {
            ui->tblTotal->setItem(0, i, new QTableWidgetItem(""));
        }
    }
}

void NTreeWidget::queryStarted()
{
    mLoadingDlg = new NLoadingDlg(tr("Query"), this);
    mLoadingDlg->open();
}

void NTreeWidget::filterByColumn()
{
    const QSet<QString>& filterValues = static_cast<NTreeModel*>(ui->mTreeView->model())->uniqueValuesForColumn(
                                            fFilterColumn);
    QStringList sortedValues = filterValues.values();
    std::sort(sortedValues.begin(), sortedValues.end());

    if(C5FilterValues::filterValues(sortedValues, mUser)) {
        static_cast<NTreeModel*>(ui->mTreeView->model())->setColumnFilter(sortedValues.join("|"), fFilterColumn);
    }

    sum();
}

void NTreeWidget::queryError(const QString &error)
{
    sender()->deleteLater();
    LogWriterError(error);
    C5Message::error(error);
    mLoadingDlg->deleteLater();
}

void NTreeWidget::queryFinished(const QJsonObject &ba)
{
    QJsonObject jo = ba;
    QJsonObject jwidget = jo["widget"].toObject();
    mMainWindow->nTabTreeDesign(QIcon(jwidget["icon"].toString()), jwidget["title"].toString(), this);
    QJsonArray jcols = jo["cols"].toArray();
    QJsonArray jchildcols = jo["childcols"].toArray();
    QJsonArray colSum = jo["sum"].toArray();
    QJsonArray jdata = jo["rows"].toArray();
    QJsonArray jhiddencols = jo["hiddencols"].toArray();

    if(!fFilter) {
        QJsonArray jfilter = jo["filter"].toArray();
        fFilter = new NFilterDlg(this);
        fFilter->setProperty("host", mHost);
        fFilter->setup(jfilter);
        fFilter->mVersion = jwidget["version"].toInt(1);
    }

    auto *model = static_cast<NTreeModel* >(ui->mTreeView->model());
    model->setDatasource(jcols, jchildcols, jdata);
    model->fSumColumnsSpecial = jo["sumspecial"].toArray();
    model->mRowColors.clear();
    ui->mTreeView->expandAll();

    for(int i = 0; i < jo["rowcolors"].toArray().size(); i++) {
        model->mRowColors.append(jo["rowcolors"].toArray().at(i).toInt());
    }

    model->setSumColumns(colSum);
    sum();

    for(int i = 0; i < ui->mTreeView->model()->columnCount(); i++) {
        ui->mTreeView->resizeColumnToContents(i);
    }

    for(int i = 0; i < ui->tblTotal->columnCount(); i++) {
        ui->mTreeView->setColumnHidden(i, false);
        ui->tblTotal->setColumnHidden(i, false);
        ui->tblTotal->setColumnWidth(i, ui->mTreeView->columnWidth(i));
    }

    for(int i = 0; i < jhiddencols.size(); i++) {
        ui->mTreeView->setColumnHidden(jhiddencols[i].toInt(), true);
        ui->tblTotal->setColumnHidden(jhiddencols[i].toInt(), true);
    }

    mHandler->mHandlers = jo["handler"].toArray().toVariantList();

    if(!mToolWidget) {
        mToolWidget = new QWidget();
        fToolBar->addWidget(mToolWidget);
        mToolWidget->setLayout(new QGridLayout());
        mHandler->toolWidget(mToolWidget);
    }

    sender()->deleteLater();
    mLoadingDlg->deleteLater();
}

void NTreeWidget::exportToExcel()
{
    auto *model = static_cast<NTreeModel*>(ui->mTreeView->model());
    int colCount = model->columnCount();

    if(colCount == 0) {
        C5Message::info(tr("Empty report!"));
        return;
    }

    QXlsx::Document d;
    d.addSheet("Sheet1");
    // Заголовок
    QXlsx::Format hf;
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    hf.setFont(headerFont);
    hf.setBorderStyle(QXlsx::Format::BorderThin);
    hf.setPatternBackgroundColor(QColor(200, 200, 250));

    for(int c = 0; c < colCount; c++) {
        d.write(1, c + 1, model->headerData(c, Qt::Horizontal, Qt::DisplayRole), hf);
        d.setColumnWidth(c + 1, ui->mTreeView->columnWidth(c) / 7);
    }

    // Формат тела
    QXlsx::Format bf;
    bf.setBorderStyle(QXlsx::Format::BorderThin);
    bf.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    QXlsx::Format bfb;
    bfb.setBorderStyle(QXlsx::Format::BorderThin);
    bfb.setHorizontalAlignment(QXlsx::Format::AlignLeft);
    bfb.setFontBold(true);
    int currentExcelRow = 2;
    // Рекурсивный обход
    std::function<void(const QModelIndex&, int)> walk;
    walk = [&](const QModelIndex & parent, int depth) {
        int rowCount = model->rowCount(parent);

        for(int r = 0; r < rowCount; r++) {
            QModelIndex idx0 = model->index(r, 0, parent);

            for(int c = 0; c < colCount; c++) {
                QModelIndex idx = model->index(r, c, parent);
                QVariant v = model->data(idx, Qt::DisplayRole);
                d.write(currentExcelRow, c + 1, v, bf);
            }

            currentExcelRow++;

            // рекурсия — если есть дети
            if(model->rowCount(idx0) > 0)
                walk(idx0, depth + 1);
        }
    };
    // стартуем от root
    walk(QModelIndex(), 0);

    // Итоговая строка
    if(ui->tblTotal->isVisible()) {
        QXlsx::Format tf = hf;  // жирный как заголовок

        for(int c = 0; c < colCount; c++) {
            QVariant v = model->fColSum.contains(c)
                         ? model->fColSum[c]
                         : QVariant();
            d.write(currentExcelRow, c + 1, v, tf);
        }
    }

    QString filename = QFileDialog::getSaveFileName(nullptr, "", "", "*.xlsx");

    if(filename.isEmpty())
        return;

    d.saveAs(filename);
    QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
}

void NTreeWidget::refreshData()
{
    query();
}

void NTreeWidget::clearFilter()
{
    if(fFilter) {
        fFilter->clear();
    }

    query();
}

void NTreeWidget::setSearchParameters()
{
    if(!fFilter) {
        return;
    }

    if(fFilter->exec() == QDialog::Accepted) {
        query();
    }
}

void NTreeWidget::tableViewHeaderResized(int column, int oldSize, int newSize)
{
    Q_UNUSED(oldSize);
    ui->tblTotal->setColumnWidth(column, newSize);
}

void NTreeWidget::tblValueChanged(int v)
{
    ui->tblTotal->horizontalScrollBar()->setValue(v);
}

void NTreeWidget::tblValueChanged2(int v)
{
    ui->mTreeView->horizontalScrollBar()->setValue(v);
}

void NTreeWidget::on_leFilterLineedit_textEdited(const QString &arg1)
{
    static_cast<NTreeModel*>(ui->mTreeView->model())->setFilter(arg1);
    sum();
}

void NTreeWidget::on_mTreeView_doubleClicked(const QModelIndex &index)
{
    auto *node = static_cast<NTreeNode*>(index.internalPointer());

    if(!node)
        return;

    auto *m = static_cast<NTreeModel*>(ui->mTreeView->model());

    if(node->parent == m->m_root) {
        QVariant id = node->values[0].toInt();
        mHandler->handle(id);
    } else {
        // QString ingredient = node->values[0].toString(); // название продукта
        // openRecipeEditor(ingredient);
    }
}

void NTreeWidget::tableViewHeaderContextMenuRequested(const QPoint &point)
{
    fFilterColumn = ui->mTreeView->columnAt(point.x());
    QString colName = ui->mTreeView->model()->headerData(fFilterColumn, Qt::Horizontal, Qt::DisplayRole).toString();
    QMenu m;
    m.addAction(QIcon(":/filter_set.png"), QString("%1 '%2'").arg(tr("Set filter")).arg(colName), this,
                SLOT(filterByColumn()));
    m.addAction(QIcon(":/filter_clear.png"), QString("%1 '%2'").arg(tr("Remove filter")).arg(colName), this,
                SLOT(removeFilterForColumn()));
    m.addAction(QIcon(":/expand.png"), tr("Autofit columns widths"), this, SLOT(autofitColumns()));
    m.exec(ui->mTreeView->mapToGlobal(point));
}

void NTreeWidget::tableViewHeaderClicked(int index)
{
    static_cast<NTreeModel*>(ui->mTreeView->model())->sort(index);
}
